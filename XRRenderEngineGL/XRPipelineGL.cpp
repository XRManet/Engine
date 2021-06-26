#include "stdafx.h"
#include "XRPipelineGL.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

#include <XRFrameworkBase/XRSourceBuildSystem.h>

namespace
{
// Move to a project 'XRSourceBuildSystemGLSL'
void CompileShader(GLuint shader, GLsizei fileCount, const char** filename);
void LoadShader(GLuint shader, const uint32_t* ilBinary, uint32_t ilSize, GLint binaryFormat);
void BuildProgram(GLuint glProgram, GLuint glShaders[], GLuint counts);
}

XRPipelineGL::XRPipelineGL(XRPipelineStateDescription const* description)
	: XRPipeline(description)
{
	bool result = buildProgram(description->_shaderStageDescription);
	if (result == false)
		throw;

	result = createBindingInformation();
	if (result == false)
		throw;

	static GLboolean isSeparableShaderObjects = GLEW_ARB_separate_shader_objects;

	glGenProgramPipelines(1, &_glPipeline);
}

XRPipelineGL::~XRPipelineGL()
{
	glDeleteProgramPipelines(1, &_glPipeline);

	GL_CALL(glDeleteShader(_glVertexShader));
	GL_CALL(glDeleteShader(_glFragmentShader));
}

void XRPipelineGL::bind()
{
	glBindProgramPipeline(_glPipeline);
	
	if (_needUpdate == true)
	{
		glUseProgramStages(_glProgram, GL_VERTEX_SHADER_BIT, _glProgram);
		glUseProgramStages(_glProgram, GL_FRAGMENT_SHADER, _glProgram);

		_needUpdate = false;
	}
}

bool XRPipelineGL::buildProgram(XRShaderStageDescription const* shaderStageDescription)
{
	printf("\n=============================================\n");
	printf("Create programs\n\n");

	// TODO) separate
	GL_CALL(_glProgram = glCreateProgram());

	constexpr uint32_t NUM_MAX_STAGES = 5;
	uint32_t numStages = 0;
	GLuint shadersToBuild[NUM_MAX_STAGES] = { 0, };

	if (nullptr != shaderStageDescription->_vertexFilename)
	{
		GL_CALL(_glVertexShader = glCreateShader(GL_VERTEX_SHADER));

		const char* vertexShaders[]{ shaderStageDescription->_vertexFilename };
		CompileShader(_glVertexShader, std::size(vertexShaders), vertexShaders);

		shadersToBuild[numStages++] = _glVertexShader;
	}

	if (nullptr != shaderStageDescription->_fragmentFilename)
	{
		GL_CALL(_glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER));

		const char* fragmentShaders[]{ "SimpleFragment.glsl" };
		CompileShader(_glFragmentShader, std::size(fragmentShaders), fragmentShaders);

		shadersToBuild[numStages++] = _glFragmentShader;
	}

	BuildProgram(_glProgram, shadersToBuild, numStages);

	return true;
}

bool XRPipelineGL::createBindingInformation()
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

		GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &_programResources._numActiveUniformBlocks));
		printf("Number of uniform block resources: %d\n", _programResources._numActiveUniformBlocks);
		GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM_BLOCK, GL_MAX_NUM_ACTIVE_VARIABLES, &_programResources._maxNumVariablesInActiveUniformBlock));
		printf("Max number of uniform block variables: %d\n", _programResources._maxNumVariablesInActiveUniformBlock);
		GLint numActiveUniform = 0;
		GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniform));
		printf("Number of uniform resources:  %d\n", numActiveUniform);
	}
	else
	{
		// Do not come here
		assert(false);

		// Directly set
		_programResources._activeUniformBlocks["Materials"] = {};
		_programResources._activeUniformBlocks["LightBlock"] = {};
		_programResources._activeUniformBlocks["MatrixBlock"] = {};
		_programResources._numActiveUniformBlocks = static_cast<GLint>(_programResources._activeUniformBlocks.size());
	}

	_programResources._indexedActiveUniformBlocks.resize(_programResources.GetActiveUniformBlocks());

	if (doQueryUniformBuffer == true)
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

		GLint numUniformBlocks = static_cast<GLint>(_programResources.GetActiveUniformBlocks());
		_programResources._indexedActiveUniformBlocks.reserve(numUniformBlocks);
		_programResources._indexedUniformBlockBindingInfo.resize(numUniformBlocks);

		for (GLint i = 0; i < numUniformBlocks; ++i)
		{
			const GLsizei bufSize = 256;

			GLchar blockName[bufSize];
			//GL_CALL(glGetProgramResourceName(_glProgram, GL_UNIFORM_BLOCK, i, bufSize, nullptr, blockName));
			glGetActiveUniformBlockName(_glProgram, i, bufSize, nullptr, blockName);
			if (glGetError() != GL_NO_ERROR)
			{
				_programResources._numActiveUniformBlocks = i;
				break;
			}
			printf("Uniform Block Name: %s, (%d/%d)\n", blockName, i, numUniformBlocks);

			if (doQueryProgramInterface)
			{
				_programResources._activeUniformBlocks[blockName] = {};
			}
			else
			{
				assert(_programResources._activeUniformBlocks.find(blockName) != _programResources._activeUniformBlocks.end());
			}

			_programResources._indexedActiveUniformBlocks.push_back(blockName);
			ProgramResources::UniformBlockBindingInfo& bindingInfo = _programResources._indexedUniformBlockBindingInfo[i];

			auto& uniformBlock = _programResources._activeUniformBlocks[blockName];
			uniformBlock._activeBlockIndex = i;

			bindingInfo._uniformBlock = &uniformBlock;
			
			// Todo: 하나의 버퍼에 여러 uniform block을 넣을 경우, 각각에 대한 offset이 필요
			bindingInfo._bufferId = 0;
			bindingInfo._offset = 0;

			GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlock._blockSize));
			GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, GL_UNIFORM_BLOCK_BINDING, &bindingInfo._binding));

			auto& blockRange = reinterpret_cast<ActiveUniformBlockRange&>(resultParams);
			//glGetProgramResourceiv(_glProgram, GL_UNIFORM_BLOCK, i, 2, props, maxResultSize, &length, resultParams);
			GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, pnames[0], &resultParams[0]));
			GL_CALL(glGetActiveUniformBlockiv(_glProgram, i, pnames[1], &resultParams[1]));

			GLchar uniformName[bufSize];
			static constexpr GLenum activeUniformQueryProps[]{
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

#if _DEBUG && 0
		auto& ubMatrixBlock = _programResources._activeUniformBlocks["MatrixBlock"];
		auto& ubLightBlock = _programResources._activeUniformBlocks["LightBlock"];

		// Active Uniform Block에 Binding을 직접 부여하는 API.
		// GLSL layout을 명시적으로 정했든, 자동 부여 되었든 Program 내에 이미 결정된 binding을 변경할 수 있다.
		GL_CALL(glUniformBlockBinding(_glProgram, ubMatrixBlock._activeBlockIndex, UNIFORM_BINDING_NAME::Matrix));
		GL_CALL(glUniformBlockBinding(_glProgram, ubLightBlock._activeBlockIndex, UNIFORM_BINDING_NAME::Light));

		if (0) // After bind uniform block with buffer, we can get the binding buffer name by following below.
		{
			GLint currentBindingPoint = -1;
			GL_CALL(glGetActiveUniformBlockiv(_glProgram, ubMatrixBlock._activeBlockIndex, GL_UNIFORM_BLOCK_BINDING, &currentBindingPoint));
		}
#endif

#if _DEBUG && 0
		struct BufferRange
		{
			GLint64 start = 0;
			GLint64 size = 0;
		} bufferRange;
		GL_CALL(glGetInteger64i_v(GL_UNIFORM_BUFFER_START, UNIFORM_BINDING_NAME::Matrix, &bufferRange.start));
		GL_CALL(glGetInteger64i_v(GL_UNIFORM_BUFFER_SIZE, UNIFORM_BINDING_NAME::Matrix, &bufferRange.size));
#endif
	}
	else
	{
		printf("Can not use buffer as an uniform buffer. XREngine couldn't support this environment.\n\n");
	}

	return true;
}

namespace
{
// Move to a project 'XRSourceBuildSystemGLSL'
#define DEFAULT_RESOURCE_PATH "Resources/Shaders/OpenGL/"
constexpr size_t RESOURCE_PATH_END = sizeof(DEFAULT_RESOURCE_PATH) - 1;

static bool CheckShaderState(GLuint shader)
{
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

	return result == GL_TRUE;
}

void CompileShader(GLuint shader, GLsizei fileCount, const char** filename)
{
	char RESOURCE_PATH[256] = DEFAULT_RESOURCE_PATH;
	char** shaderSources = new char* [fileCount]();
	int* sizes = new int[fileCount]();

	for (GLsizei i = 0; i < fileCount; ++i)
	{
		strcpy_s(RESOURCE_PATH + RESOURCE_PATH_END, 256 - RESOURCE_PATH_END, filename[i]);
		FILE* fp = nullptr;
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

	bool result = CheckShaderState(shader);

	for (GLsizei i = 0; i < fileCount; ++i)
	{
		delete[] shaderSources[i];
	}
	delete[] shaderSources;
	delete[] sizes;
}

void LoadShader(GLuint shader, const uint32_t* ilBinary, uint32_t ilSize, GLint binaryFormat)
{
	assert(binaryFormat == GL_SHADER_BINARY_FORMAT_SPIR_V_ARB);
	glShaderBinary(1, &shader, binaryFormat, ilBinary, ilSize);

	/* Specialize의 경우 permutation을 사용하기에 적합한 시스템인 것으로 보임.
	 * 사용하지 않은 경우엔 기본값으로 사용됨.
	const GLuint constantIndices[] = { 0, 7, 3 };
	const GLuint constantValues[] = { 1, 42, 0x3F800000 };
	glSpecializeShaderARB(shader,
		"main",
		3,
		constantIndices,
		constantValues);
	*/

	bool result = CheckShaderState(shader);
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