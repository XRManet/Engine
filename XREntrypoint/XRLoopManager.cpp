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

namespace
{
	// Move to XRPipelineGL
#define DEFAULT_RESOURCE_PATH "Resources/Shaders/OpenGL/"
    constexpr size_t RESOURCE_PATH_END = sizeof(DEFAULT_RESOURCE_PATH) - 1;
    
    void CompileShader(GLuint shader, GLsizei fileCount, const char** filename)
    {
        char RESOURCE_PATH[256] = DEFAULT_RESOURCE_PATH;
        char** shaderSources = new char*[fileCount]();
        int* sizes = new int[fileCount]();
        
        for (GLsizei i = 0; i < fileCount; ++i)
        {
            strcpy(RESOURCE_PATH + RESOURCE_PATH_END, filename[i]);
			FILE *fp = nullptr;
			errno_t error = xr::fopen(&fp, RESOURCE_PATH, "r");
            
            assert(fp != nullptr);
            if (error != 0 || fp == nullptr) continue;
            
            bool result = (fseek(fp, 0, SEEK_END) == 0);
            if (result == true)
            {
                long size = ftell(fp);
                rewind(fp);
                
                char* buffer = new char[size + 1]();
                
                fread(buffer, sizeof(char), size, fp);
                
                shaderSources[i] = buffer;
                sizes[i] = static_cast<int>(size);
            }
            
            fclose(fp);
        }
        
        GL_CALL(glShaderSource(shader, fileCount, shaderSources, sizes));
        GL_CALL(glCompileShader(shader));
        
        GLint result = GL_FALSE;
        GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE)
        {
            GLint logLength = 0;
            GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));
            
            if (logLength > 0)
            {
                GLchar* failedLog = new GLchar[logLength]();
                GL_CALL(glGetShaderInfoLog(shader, logLength, &logLength, failedLog));
                
                printf("%s", failedLog);
            }
        }
        
        for (GLsizei i = 0; i < fileCount; ++i)
        {
            delete shaderSources[i];
        }
        delete[] shaderSources;
        delete[] sizes;
    }
    
    void BuildProgram(GLuint glProgram, GLuint glShaders[], GLuint counts)
    {
        for (GLuint i = 0; i < counts; ++i)
        {
            GL_CALL(glAttachShader(glProgram, glShaders[i]));
        }
        
        GL_CALL(glLinkProgram(glProgram));
        
        GLint result = GL_FALSE;
        GL_CALL(glGetProgramiv(glProgram, GL_LINK_STATUS, &result));
        if (result == GL_FALSE)
        {
            GLint logLength = 0;
            GL_CALL(glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &logLength));
            
            if (logLength > 0)
            {
                GLchar* failedLog = new GLchar[logLength]();
                GL_CALL(glGetProgramInfoLog(glProgram, logLength, &logLength, failedLog));
                
                printf("%s", failedLog);
            }
        }
        
        for (GLuint i = 0; i < counts; ++i)
        {
            GL_CALL(glDetachShader(glProgram, glShaders[i]));
        }
    }
}

const char* GetGlMeaningDebugType(GLenum debugType)
{
#if defined(GL_KHR_debug)
    switch (debugType)
    {
        case GL_DEBUG_TYPE_ERROR:               return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated behavior";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined behavior";
        case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
        case GL_DEBUG_TYPE_OTHER:               return "Other";
        case GL_DEBUG_TYPE_MARKER:              return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push group";
        case GL_DEBUG_TYPE_POP_GROUP:           return "Pop group";
    }
#endif
    return "<ERROR>";
}

const char* GetGlMeaningSeverity(GLenum severity)
{
#if defined(GL_KHR_debug)
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION:    return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_HIGH:            return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM:          return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW:             return "LOW";
    }
#endif
    return "<ERROR>";
}

void GLAPIENTRY MessageCallback(
                                GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = %s, severity = %s, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            GetGlMeaningDebugType(type), GetGlMeaningSeverity(severity), message);
	assert(type != GL_DEBUG_TYPE_ERROR);
}

static GLint MAX_UNIFORM_BUFFER_BINDINGS = 0;
static GLint MAX_UNIFORM_LOCATIONS = 0;
static GLint UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
static GLint MAX_UNIFORM_BLOCK_SIZE = 0;

static GLint MAX_VARYING_COMPONENTS = 0;

static GLint MAX_TEXTURE_IMAGE_UNITS = 0;
static GLint MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0;
static GLint MAX_COMPUTE_TEXTURE_IMAGE_UNITS = 0;

struct UniformBlock
{
	uint32_t _uniformBlockIndex;
	int32_t _uniformBlockSize;
	std::unordered_map<std::string, uint32_t> _uniformIndices;
};

struct ProgramResources
{
	GLint _maxNumVariablesInActiveUniformBlock = 16;
	GLint _numActiveUniformBlocks = 0;
	std::unordered_map<std::string, UniformBlock> _activeUniformBlocks;

	struct UniformBlockBindingInfo
	{
		uint32_t			_binding = 0;
		uint32_t			_offset = 0;
		const UniformBlock*	_uniformBlock = nullptr;

		bool isBound() { return _uniformBlock != nullptr; }
	};
	std::vector<std::string> _indexedActiveUniformBlocks;
	std::vector<UniformBlockBindingInfo> _indexedUniformBlockBindingInfo;

public:
	size_t GetActiveUniformBlocks() const { return _numActiveUniformBlocks; }
};
ProgramResources programResources;

void XRRendererTest::Initialize()
{
    assert(glGetError() == GL_NO_ERROR);
    
    if(glfwExtensionSupported("GL_KHR_debug") == GLFW_TRUE)
    {
        GL_CALL(glEnable(GL_DEBUG_OUTPUT));
        
        if(glfwGetProcAddress("glDebugMessageCallback"))
            glDebugMessageCallback(MessageCallback, 0);
    }
	
	if (glfwExtensionSupported("GL_ARB_vertex_attrib_binding") == GLFW_TRUE)
	{
	}

	printf("\n=============================================\n");
	printf("Query OpenGL System Informations\n\n");

	GL_CALL_WARN(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &MAX_UNIFORM_BUFFER_BINDINGS));
	printf("Max uniform block bindings: %d\n", MAX_UNIFORM_BUFFER_BINDINGS);
	GL_CALL_WARN(glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &MAX_UNIFORM_LOCATIONS));
	printf("Max uniform locations: %d\n", MAX_UNIFORM_LOCATIONS);
	GL_CALL_WARN(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UNIFORM_BUFFER_OFFSET_ALIGNMENT));
	printf("Uniform buffer offset alignment: %d\n", UNIFORM_BUFFER_OFFSET_ALIGNMENT);
	GL_CALL_WARN(glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &MAX_UNIFORM_BLOCK_SIZE));
	printf("Max size of an uniform block: %d\n", MAX_UNIFORM_BLOCK_SIZE);

	printf("\n");
	GL_CALL_WARN(glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &MAX_VARYING_COMPONENTS));
	printf("Max varying components: %d\n", MAX_VARYING_COMPONENTS);

	printf("\n");
	GL_CALL_WARN(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_IMAGE_UNITS));
	printf("Max texture image units: %d\n", MAX_TEXTURE_IMAGE_UNITS);
	GL_CALL_WARN(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_COMBINED_TEXTURE_IMAGE_UNITS));
	printf("Max combined texture image units: %d\n", MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	GL_CALL_WARN(glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &MAX_COMPUTE_TEXTURE_IMAGE_UNITS));
	printf("Max compute texture image units: %d\n", MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
    
	printf("\n=============================================\n");
	printf("Create programs\n\n");

    // TODO) separate
    GL_CALL(_glProgram = glCreateProgram());
    
    GL_CALL(_glVertexShader = glCreateShader(GL_VERTEX_SHADER));
    GL_CALL(_glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER));
    
    const char* vertexShaders[]{ "SimpleVertex.glsl" };
    CompileShader(_glVertexShader, std::size(vertexShaders), vertexShaders);
    
    const char* fragmentShaders[]{ "SimpleFragment.glsl" };
    CompileShader(_glFragmentShader, std::size(fragmentShaders), fragmentShaders);
    
    GLuint shaders[] = {
        _glVertexShader,
        _glFragmentShader
    };
    BuildProgram(_glProgram, shaders, std::size(shaders));
    
    GL_CALL(glDeleteShader(_glVertexShader));
    GL_CALL(glDeleteShader(_glFragmentShader));

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
            uniformBlock._uniformBlockIndex = i;

			GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlock._uniformBlockSize));

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
		GL_CALL(glUniformBlockBinding(_glProgram, ubMatrixBlock._uniformBlockIndex, UNIFORM_BINDING_NAME::Matrix));
		if (0) // After bind uniform block with buffer, we can get the binding buffer name by following below.
		{
			GLint currentBindingPoint = -1;
			GL_CALL(glGetActiveUniformBlockiv(_glProgram, uniformIndexTransform, GL_UNIFORM_BLOCK_BINDING, &currentBindingPoint));
		}

		auto& ubLightBlock = programResources._activeUniformBlocks["LightBlock"];
		GL_CALL(glUniformBlockBinding(_glProgram, ubLightBlock._uniformBlockIndex, UNIFORM_BINDING_NAME::Light));

		programResources._indexedUniformBlockBindingInfo.resize(UNIFORM_BINDING_NAME::Count);

		GLsizeiptr offset = 0;
        offset = NEXT_ALIGN_2(offset + ubMatrixBlock._uniformBlockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);
        offset = NEXT_ALIGN_2(offset + ubLightBlock._uniformBlockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);
        
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
        GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, UNIFORM_BINDING_NAME::Matrix, _uniformBuffers[0], offset, ubMatrixBlock._uniformBlockSize));
        offset = NEXT_ALIGN_2(offset + ubMatrixBlock._uniformBlockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);

		programResources._indexedUniformBlockBindingInfo[UNIFORM_BINDING_NAME::Light] = {
            UNIFORM_BINDING_NAME::Light, static_cast<GLuint>(offset), &ubLightBlock };
		GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, UNIFORM_BINDING_NAME::Light, _uniformBuffers[0], offset, ubLightBlock._uniformBlockSize));
        offset = NEXT_ALIGN_2(offset + ubLightBlock._uniformBlockSize, UNIFORM_BUFFER_OFFSET_ALIGNMENT);

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
			GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, ii->_offset, ii->_uniformBlock->_uniformBlockSize, dataAddress[i]));
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

#include <XRFrameworkBase/XRRenderer.h>

XRFrameWalker::XRFrameWalker()
{
    // TODO) select a strategy by reading from manifest.
    //_renderer = new XRRenderer();
}

void XRFrameWalker::UpdateFrame()
{
    auto scene = XRSceneManager::GetInstance()->GetPrimaryScene();
	auto renderer = XRSceneManager::GetInstance()->GetCurrentRenderer();
    
    scene->Update(0);
	scene->Render(renderer);
	renderer->Update();
	renderer->Render();
}
