#include "stdafx.h"
#include "XRLoopManager.h"

#include <XRFrameworkBase/XRObject.h>
#include <XRFrameworkBase/XRScene.h>
#include "XRSceneManager.h"

XRInputLinkage g_input;

XRRenderingInfra<GLFW>::XRRenderingInfra(XRSize<float> const& size)
{
	static const GLuint versions[][2] = {
		{4, 6}, {4, 5}, {4, 3}, {4, 1}, {4, 0},
		{3, 3}, {3, 0},
		{2, 1}, {2, 0},
		{1, 1}, {1, 0}
	};
	GLuint version_try = 0;

	if (glfwInit() == 0)
		throw;

	while (1)
	{
		static const GLuint max_try = static_cast<GLuint>(sizeof(versions) / sizeof(versions[0]));
		if (version_try < max_try)
		{
			// Todo: Query for each platform
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versions[version_try][0]);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versions[version_try][1]);
			// Note: Use compatible profile because there is a bug in GLEW for Windows
#if XR_PLATFORM == XR_PLATFORM_WINDOWS
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#elif XR_PLATFORM == XR_PLATFORM_OSX
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
			// Note: To control with EGL may not be available.
			//glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
			glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
			glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#if defined(_DEBUG) || defined(DEBUG)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
		}
		else
		{
			glfwDefaultWindowHints();
		}

		_window = glfwCreateWindow(size._width, size._height,
			"Hello world!", NULL, NULL);

		if (_window != nullptr)
		{
			printf("OpenGL version: ");
			if (version_try <= max_try)
				printf("%d.%d\n", versions[version_try][0], versions[version_try][1]);
			else
				printf("system-default\n");

			glfwSetWindowUserPointer(_window, this);
			glfwMakeContextCurrent(_window);
			break;
		}
		else
		{
			++version_try;
		}
	}

	glfwSetKeyCallback(_window, InputKeyboard);
	glfwSetMouseButtonCallback(_window, InputMouse);
	glfwSetCursorPosCallback(_window, PositionMouse);
	glfwSetScrollCallback(_window, ScrollMouse);
}

XRRenderingInfra<GLFW>::~XRRenderingInfra()
{
	if (!_window) {
		glfwTerminate();
	}
}


void XRRenderingInfra<GLFW>::InputKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto _this = static_cast<XRRenderingInfra<GLFW>*>(glfwGetWindowUserPointer(window));

	if (key < 256)
		g_input.keyboardPressed[key] = (action == GLFW_REPEAT || action == GLFW_PRESS) ? 1 : 0;

	switch (key)
	{
	case GLFW_KEY_W:
	case GLFW_KEY_S:
	case GLFW_KEY_A:
	case GLFW_KEY_D:
	case GLFW_KEY_Q:
	case GLFW_KEY_E:
		if (action == GLFW_PRESS);
		else if (action == GLFW_RELEASE);
		else if (action == GLFW_REPEAT);
		break;
	}
}

void XRRenderingInfra<GLFW>::InputMouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			g_input.anchorX = g_input.curX;
			g_input.anchorY = g_input.curY;
			g_input.anchored = true;
		}
		else if (action == GLFW_RELEASE)
		{
			g_input.anchored = false;
		}
	}
}

void XRRenderingInfra<GLFW>::PositionMouse(GLFWwindow* window, double xpos, double ypos)
{
	int32_t windowSizeX = 0, windowSizeY = 0;
	glfwGetWindowSize(window, &windowSizeX, &windowSizeY);

	g_input.curX = xpos;
	g_input.curY = double(windowSizeY) - ypos;
}

void XRRenderingInfra<GLFW>::ScrollMouse(GLFWwindow* window, double xoffset, double yoffset)
{
	printf("%lf\n", yoffset);
	g_input.cameraStep += float(yoffset * .1f) ;
	if (g_input.cameraStep <= .1f)
		g_input.cameraStep = .1f;
}

bool XRRenderingInfra<GLFW>::WindowShouldClose()
{
	return glfwWindowShouldClose(_window);
}

void XRRenderingInfra<GLFW>::PollEvents()
{
	/* Swap front and back buffers */
	glfwSwapBuffers(_window);

	/* Poll for and process events */
	glfwPollEvents();
}

#if TEST_CODE// TEST_CODE

void XRRendererTest::Initialize()
{
    static int isProgramInterfaceQueriable = glfwExtensionSupported("GL_ARB_program_interface_query");
	static bool doQueryProgramInterface = (isProgramInterfaceQueriable == GLFW_TRUE)
                                || (glfwGetProcAddress("glGetProgramInterfaceiv") != nullptr);
    
    static int isUniformBufferObjectQueriable = glfwExtensionSupported("GL_ARB_uniform_buffer_object");
    static bool doQueryUniformBuffer = (isUniformBufferObjectQueriable == GLFW_TRUE)
                                || (glfwGetProcAddress("glGetActiveUniformsiv") != nullptr);

    GL_CALL(glUseProgram(_glProgram));
    
	if (doQueryProgramInterface == true)
	{
		printf("=============================================\n");
		printf("Reflect programs\n\n");

		GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &programResources._numActiveUniformBlocks));
		printf("Number of uniform block resources: %d\n", programResources._numActiveUniformBlocks);
		GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM_BLOCK, GL_MAX_NUM_ACTIVE_VARIABLES, &programResources._maxNumVariablesInActiveUniformBlock));
		printf("Max number of uniform block variables: %d\n", programResources._maxNumVariablesInActiveUniformBlock);
		GLint numActiveUniform = 0;
		GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniform));
		printf("Number of uniform resources:  %d\n", numActiveUniform);
    }
    else
    {
		// Directly set
        programResources._activeUniformBlocks["Materials"] = {};
        programResources._activeUniformBlocks["LightBlock"] = {};
        programResources._activeUniformBlocks["MatrixBlock"] = {};
        programResources._numActiveUniformBlocks = static_cast<GLint>(programResources._activeUniformBlocks.size());
    }
    programResources._indexedActiveUniformBlocks.reserve(programResources.GetActiveUniformBlocks());
    
    if(doQueryUniformBuffer == true)
    {
        printf("=============================================\n");
        printf("Reflect uniform blocks\n\n");
        
        GLuint uniformIndexTransform = -1;
        
		GLsizei length;
		GLint resultParams[64] = { -1, -1 };
		constexpr GLsizei maxResultSize = sizeof(resultParams) / sizeof(resultParams[0]);
		struct ActiveUniformBlockRange
		{
			GLint numActiveVariables;
			GLuint uniformIndices[1];
		};
        
        GLenum props[3] = {
            GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES, 0,
        };
        GLenum pnames[3] = {
            GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, 0,
        };
        GLint numUniformBlocks = static_cast<GLint>(programResources.GetActiveUniformBlocks());
		for (GLint i = 0; i < numUniformBlocks; ++i)
		{
			const GLsizei bufSize = 256;

			GLchar blockName[bufSize];
			//GL_CALL(glGetProgramResourceName(_glProgram, GL_UNIFORM_BLOCK, i, bufSize, nullptr, blockName));
            glGetActiveUniformBlockName(_glProgram, i, bufSize, nullptr, blockName);
            if(glGetError() != GL_NO_ERROR)
            {
                programResources._numActiveUniformBlocks = i;
                break;
            }
			printf("Uniform Block Name: %s, (%d/%d)\n", blockName, i, numUniformBlocks);
            
			if (doQueryProgramInterface)
			{
				programResources._activeUniformBlocks[blockName] = {};
			}
			else
			{
				assert(programResources._activeUniformBlocks.find(blockName) != programResources._activeUniformBlocks.end());
			}
			programResources._indexedActiveUniformBlocks.push_back(blockName);
            
            auto& uniformBlock = programResources._activeUniformBlocks[blockName];
            uniformBlock._activeBlockIndex = i;

			GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlock._blockSize));

			auto& blockRange = reinterpret_cast<ActiveUniformBlockRange&>(resultParams);
			//glGetProgramResourceiv(_glProgram, GL_UNIFORM_BLOCK, i, 2, props, maxResultSize, &length, resultParams);
            GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, pnames[0], &resultParams[0]));
            GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, pnames[1], &resultParams[1]));

			GLchar uniformName[bufSize];
            static constexpr GLenum activeUniformQueryProps[] {
                GL_TYPE,
				GL_ARRAY_SIZE,
				GL_BLOCK_INDEX,
				GL_OFFSET,
				GL_ARRAY_STRIDE,
				GL_MATRIX_STRIDE,
				GL_IS_ROW_MAJOR,
				GL_ATOMIC_COUNTER_BUFFER_INDEX,
            };
			static constexpr struct {
				GLenum pname;
				const char* stringName;
			} activeUniformQueryPnames[] = {
				{ GL_UNIFORM_TYPE,						  "Type"},
				{ GL_UNIFORM_SIZE,						  "Num"},
				{ GL_UNIFORM_BLOCK_INDEX,				  "BlockIndex"},
				{ GL_UNIFORM_OFFSET,					  "Offset"},
				{ GL_UNIFORM_ARRAY_STRIDE,				  "ArrayStride"},
				{ GL_UNIFORM_MATRIX_STRIDE,				  "MatrixStride"},
				{ GL_UNIFORM_IS_ROW_MAJOR,				  "IsRowMajor"},
				{ GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX, "AtomicCounterBufferIndex"},
			};
			constexpr uint32_t queryCount = sizeof(activeUniformQueryPnames) / sizeof(activeUniformQueryPnames[0]);
			GLint uniformQueryResult[queryCount];
			printf("Uniform NumActiveVariables: %d\n", blockRange.numActiveVariables);
			for (GLint i = 0; i < blockRange.numActiveVariables; ++i)
			{
                GL_CALL(glGetActiveUniformName(_glProgram, blockRange.uniformIndices[i], bufSize, nullptr, uniformName));
				//GL_CALL(glGetProgramResourceName(_glProgram, GL_UNIFORM, blockRange.uniformIndices[i], bufSize, nullptr, uniformName));
				printf("\tUniform index [%d] Name: %s, ", blockRange.uniformIndices[i], uniformName);
                uniformBlock._uniformIndices[uniformName] = i;

				//glGetProgramResourceiv(_glProgram, GL_UNIFORM, blockRange.uniformIndices[i], queryCount, activeUniformQueryProps, queryCount, nullptr, uniformQueryResult);
				for (GLint j = 0; j < queryCount; ++j)
				{
					GL_CALL_WARN(glGetActiveUniformsiv(_glProgram, 1, &blockRange.uniformIndices[i], activeUniformQueryPnames[j].pname, &uniformQueryResult[j]));
					printf("%s: %d, ", activeUniformQueryPnames[j].stringName, uniformQueryResult[j]);
				}
				printf("\n");
			}

			//GLint variablesQueryResult[64] = {0, };
			//for (GLint j = 0; j < queryCount; ++j)
			//{
			//	glGetActiveUniformsiv(_glProgram, blockRange.numActiveVariables, blockRange.uniformIndices, activeUniformQueries[j], variablesQueryResult);
			//}
		}

		printf("\n=============================================\n");
		printf("Create uniform buffers with reflection\n\n");

		_uniformBuffers.resize(programResources.GetActiveUniformBlocks());
		glGenBuffers(static_cast<GLint>(programResources.GetActiveUniformBlocks()), _uniformBuffers.data());

		auto& ubMatrixBlock = programResources._activeUniformBlocks["MatrixBlock"];
		GL_CALL(glUniformBlockBinding(_glProgram, ubMatrixBlock._activeBlockIndex, UNIFORM_BINDING_NAME::Matrix));
		if (0) // After bind uniform block with buffer, we can get the binding buffer name by following below.
		{
			GLint currentBindingPoint = -1;
			GL_CALL(glGetActiveUniformBlockiv(_glProgram, uniformIndexTransform, GL_UNIFORM_BLOCK_BINDING, &currentBindingPoint));
		}

		auto& ubLightBlock = programResources._activeUniformBlocks["LightBlock"];
		GL_CALL(glUniformBlockBinding(_glProgram, ubLightBlock._activeBlockIndex, UNIFORM_BINDING_NAME::Light));

		programResources._indexedUniformBlockBindingInfo.resize(UNIFORM_BINDING_NAME::Count);

		GLsizeiptr offset = 0;
        offset = NEXT_ALIGN_2(offset + ubMatrixBlock._blockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);
        offset = NEXT_ALIGN_2(offset + ubLightBlock._blockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);
        
        GLsizeiptr uniformBufferSize = offset;
        GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffers[0]));
        GL_CALL(glBufferData(GL_UNIFORM_BUFFER, uniformBufferSize, nullptr, GL_DYNAMIC_DRAW));

#if defined(_DEBUG) || defined(DEBUG)
        // Note: Core profile needs allocation first, then binding each range to corresponding index as we wish.
        {
            GLint allocatedSize = 0;
            glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &allocatedSize);
            assert(allocatedSize == uniformBufferSize);
        }
#endif
        
        offset = 0;
        programResources._indexedUniformBlockBindingInfo[UNIFORM_BINDING_NAME::Matrix] = {
            UNIFORM_BINDING_NAME::Matrix, static_cast<GLuint>(offset), &ubMatrixBlock };
        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, UNIFORM_BINDING_NAME::Matrix, _uniformBuffers[0], offset, ubMatrixBlock._blockSize));
        offset = NEXT_ALIGN_2(offset + ubMatrixBlock._blockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);

		programResources._indexedUniformBlockBindingInfo[UNIFORM_BINDING_NAME::Light] = {
            UNIFORM_BINDING_NAME::Light, static_cast<GLuint>(offset), &ubLightBlock };
		GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, UNIFORM_BINDING_NAME::Light, _uniformBuffers[0], offset, ubLightBlock._blockSize));
        offset = NEXT_ALIGN_2(offset + ubLightBlock._blockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);

        struct BufferRange
        {
            GLint64 start = 0;
            GLint64 size = 0;
        } bufferRange;
        GL_CALL(glGetInteger64i_v(GL_UNIFORM_BUFFER_START, UNIFORM_BINDING_NAME::Matrix, &bufferRange.start));
        GL_CALL(glGetInteger64i_v(GL_UNIFORM_BUFFER_SIZE, UNIFORM_BINDING_NAME::Matrix, &bufferRange.size));
        
		printf("\n=============================================\n");
		printf("Create primitives\n\n");
	}
	else
	{
		printf("Can not use buffer as an uniform buffer. XREngine couldn't support this environment.\n\n");
	}
}

#include <XRRenderEngineGL/XRModelGL.h>
#include <XRRenderEngineGL/XRPipelineGL.h>

void XRRendererTest::Render()
{
    glClearColor(1, 1, 1, 1);
    
    glClear(GL_COLOR_BUFFER_BIT);

	GL_CALL(glUseProgram(_glProgram));

	XRPipeline* pipeline;

	XRObjectGroup const* teapotGroup = scene->getObjectGroup("teapots_1");
	teapotGroup->draw();
}

#endif // TEST_CODE

#include <XRFrameworkBase/XRRenderer.h>

XRFrameWalker::XRFrameWalker()
{
    // TODO) select a strategy by reading from manifest.
    //_renderer = new XRRenderer();
	GL_SHADER_BINARY_FORMAT_SPIR_V;
}

void XRFrameWalker::Initialize()
{
	auto scene = XRSceneManager::GetInstance()->GetPrimaryScene();
	auto renderer = XRSceneManager::GetInstance()->GetCurrentRenderer();

	scene->LinkInput(&g_input);

	// Note(jiman): 임시로 ResourceManager를 씬에서 얻게 함. 나중에 전역으로 변경
	auto resourceManager = scene->GetResourceManager();
	renderer->Initialize(resourceManager);
}

void XRFrameWalker::UpdateFrame()
{
    auto scene = XRSceneManager::GetInstance()->GetPrimaryScene();
	auto renderer = XRSceneManager::GetInstance()->GetCurrentRenderer();
    
	// scene에서 renderer에다가 register하는 걸 매 프레임 처리하기 때문에 매번 Reset해주어야 하는데,
	// 역시 graph 객체를 별도로 관리하고, XRRenderer::Reset()는 아예 없애버리는 게 나을 것 같네요.
	// scene->Render(out_graph);
	// renderer->Update(in_graph);
	renderer->Reset();

	scene->Update(0);
	scene->Render(renderer);

	renderer->Update(scene);
	renderer->Render();
}
