#include "stdafx.h"
#include "XRLoopManager.h"

#include <XRFrameworkBase/XRObject.h>
#include <XRFrameworkBase/XRScene.h>
#include "XRSceneManager.h"

int32_t g_keyboardPressed[256] = { 0, };

void XRRenderingInfra<GLFW>::InputKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto _this = static_cast<XRRenderingInfra<GLFW>*>(glfwGetWindowUserPointer(window));

	if (key < 256)
		g_keyboardPressed[key] = (action == GLFW_REPEAT || action == GLFW_PRESS) ? 1 : 0;

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

double curX = 0, curY = 0;
double anchorX = 0, anchorY = 0;
bool anchored = false;
float cameraStep = .1f;

void XRRenderingInfra<GLFW>::InputMouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			anchorX = curX;
			anchorY = curY;
			anchored = true;
		}
		else if (action == GLFW_RELEASE)
		{
			anchored = false;
		}
	}
}

void XRRenderingInfra<GLFW>::PositionMouse(GLFWwindow* window, double xpos, double ypos)
{
	int32_t windowSizeX = 0, windowSizeY = 0;
	glfwGetWindowSize(window, &windowSizeX, &windowSizeY);

	curX = xpos;
	curY = double(windowSizeY) - ypos;
}

void XRRenderingInfra<GLFW>::ScrollMouse(GLFWwindow* window, double xoffset, double yoffset)
{
	printf("%lf\n", yoffset);
	cameraStep += float(yoffset * .1f) ;
	if (cameraStep <= .1f)
		cameraStep = .1f;
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

void XRRendererTest::Update()
{
	std::vector<unsigned char> uniformBufferData;
	{
		struct MatrixBlock
		{
			glm::mat4 view;
			glm::mat4 proj;
			glm::mat4 viewProj;
		};

		struct LightBlock
		{
			glm::vec4 position;
			glm::vec3 intensity;
			float attenuation;
		};

		const size_t offsetMatrixBlock = 0;
		const size_t sizeMatrixBlock = (sizeof(MatrixBlock) * 1);
		const size_t offsetLightBlock = offsetMatrixBlock + sizeMatrixBlock;
		const size_t sizeLightBlock = (sizeof(LightBlock) * 1);
		uniformBufferData.resize(sizeMatrixBlock + sizeLightBlock);

		programResources._indexedUniformBlockBindingInfo[UNIFORM_BINDING_NAME::Matrix];
		MatrixBlock& matrixBlock = reinterpret_cast<MatrixBlock&>(uniformBufferData[offsetMatrixBlock]);
		LightBlock& lightBlock = reinterpret_cast<LightBlock&>(uniformBufferData[offsetLightBlock]);

		glm::vec3 cameraMove{
			(g_keyboardPressed['D'] - g_keyboardPressed['A']),
			(g_keyboardPressed['E'] - g_keyboardPressed['Q']),
			(g_keyboardPressed['S'] - g_keyboardPressed['W'])
		};
		glm::vec3 alignedMove = glm::mat3_cast(scene->getCameras()[0].GetQuaternion()) * cameraMove * cameraStep;
		scene->getCameras()[0].Move(alignedMove);

		static bool anchorOrientation = false;
		static glm::quat orientation;
		if (true == anchored)
		{
			static const auto up = glm::vec3(0, 1, 0);
			static const auto right = glm::vec3(1, 0, 0);

			if (false == anchorOrientation)
			{
				orientation = scene->getCameras()[0].GetQuaternion();
				anchorOrientation = true;
			}
			auto axisXangle = glm::radians(-float(curX - anchorX)) * cameraStep;
			auto axisYangle = glm::radians(float(curY - anchorY)) * cameraStep;
			
			auto rotation = glm::rotate(glm::rotate(orientation, axisXangle, up), axisYangle, right);
			scene->getCameras()[0].SetQuaternion(rotation);
		}
		else anchorOrientation = false;

		matrixBlock.view = scene->getCameras()[0].GetInvTransform();
		matrixBlock.proj = scene->getCameras()[0].GetProjectionTransform();
		matrixBlock.viewProj = matrixBlock.proj * matrixBlock.view;

#define UPLOAD_METHOD_PER_DATA	0
#define UPLOAD_METHOD_ALL_ONCE	1
#define UPLOAD_METHOD			UPLOAD_METHOD_ALL_ONCE

#if UPLOAD_METHOD == UPLOAD_METHOD_PER_DATA
		std::vector<void*> dataAddress;
		dataAddress.resize(programResources._indexedUniformBlockBindingInfo.size());
		dataAddress[UNIFORM_BINDING_NAME::Matrix] = &matrixBlock;
		dataAddress[UNIFORM_BINDING_NAME::Light] = &lightBlock;

		uint32_t i = 0;
		for (auto ii = programResources._indexedUniformBlockBindingInfo.begin(); ii != programResources._indexedUniformBlockBindingInfo.end(); ++ii, ++i)
		{
			if (ii->isBound() == false) continue;
			GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, ii->_offset, ii->_uniformInfo->_blockSize, dataAddress[i]));
		}
#elif UPLOAD_METHOD == UPLOAD_METHOD_ALL_ONCE
		GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformBufferData.size(), uniformBufferData.data() ));
#endif
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

	renderer->Update();
	renderer->Render();
}
