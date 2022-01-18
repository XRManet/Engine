#include "stdafx.h"
#include "XRPipelineGL.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

#include <XRFrameworkBase/XRSourceBuildSystem.h>
#include <XRSourceBuildSystemGLSL/XRSourceBuildSystemGLSL.h>

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
	XRSourceBuildSystem* glslBuildSystem = xrGetShaderBuildSystem();
	XRCompiler* glslCompiler = glslBuildSystem->getCompiler();

	XRBuildConfiguration buildConfiguration = {};
	XRBuildItemManifest buildItemManifest = {};
	buildItemManifest._shaderStageDescription = description->_shaderStageDescription;

	XRGLProgram* glProgram = static_cast<XRGLProgram*>(glslCompiler->BuildExecutable(&buildConfiguration, &buildItemManifest));
	if (glProgram == nullptr)
		throw;

	_glProgram = glProgram->_program;

	bool result = createBindingInformation();
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
		glUseProgramStages(_glProgram, GL_FRAGMENT_SHADER_BIT, _glProgram);

		_needUpdate = false;
	}
}

struct ProgramResourcesGL : public ProgramResources
{
	struct ProgramDesc
	{
		GLint _numActiveUniform = 0;
	};
	virtual void reflectProgram(GLint const glProgram, ProgramDesc& outProgramDesc) = 0;

	struct UniformDesc
	{
		constexpr static GLsizei BUF_SIZE = 256;
		
		GLchar _blockName[BUF_SIZE];

		GLint _blockIndex = 0;
		GLint _binding = 0;
		UniformInfo _uniformInfo;
	};
	virtual void reflectUniformDesc(GLint const glProgram, UniformDesc& outUniformDesc) = 0;
};

struct ProgramResourcesGL4 : public ProgramResourcesGL
{
	virtual void reflectProgram(GLint const glProgram, ProgramDesc& outProgramDesc) override final
	{
		GL_CALL(glGetProgramInterfaceiv(glProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &_numActiveUniformBlocks));
		GL_CALL(glGetProgramInterfaceiv(glProgram, GL_UNIFORM_BLOCK, GL_MAX_NUM_ACTIVE_VARIABLES, &_maxNumVariablesInActiveUniformBlock));
		
		GL_CALL(glGetProgramInterfaceiv(glProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &outProgramDesc._numActiveUniform));
	}

	virtual void reflectUniformDesc(GLint const glProgram, UniformDesc& outUniformDesc) override final
	{
		GLenum props[] = {
			GL_BUFFER_DATA_SIZE,
			GL_BUFFER_BINDING,
			GL_NUM_ACTIVE_VARIABLES,
			GL_ACTIVE_VARIABLES,
		};
		
		const GLint blockIndex = outUniformDesc._blockIndex;
		GL_CALL(glGetProgramResourceName(glProgram, GL_UNIFORM_BLOCK, blockIndex, UniformDesc::BUF_SIZE, nullptr, outUniformDesc._blockName));

		outUniformDesc._uniformInfo._activeBlockIndex = blockIndex;
		glGetProgramResourceiv(glProgram, GL_UNIFORM_BLOCK, blockIndex, 1, &props[0], 1, nullptr, &outUniformDesc._uniformInfo._blockSize);

		glGetProgramResourceiv(glProgram, GL_UNIFORM_BLOCK, blockIndex, 1, &props[1], 1, nullptr, &outUniformDesc._binding);

		GLint numActiveVariables = 0;
		glGetProgramResourceiv(glProgram, GL_UNIFORM_BLOCK, blockIndex, 1, &props[2], 1, nullptr, &numActiveVariables);

		GLsizei length = 0;
		std::vector<GLint> arrayUniformIndices(numActiveVariables);
		glGetProgramResourceiv(glProgram, GL_UNIFORM_BLOCK, blockIndex, 1, &props[3], arrayUniformIndices.size(), &length, arrayUniformIndices.data());

		GLchar uniformName[UniformDesc::BUF_SIZE];
		for (GLint j = 0; j < length; ++j)
		{
			GL_CALL(glGetProgramResourceName(glProgram, GL_UNIFORM, arrayUniformIndices[j], UniformDesc::BUF_SIZE, nullptr, uniformName));
			outUniformDesc._uniformInfo._uniformIndices[uniformName] = j;
		}

#if defined(DEBUG_MESSAGE_REFLECT_SHADER)
		static constexpr GLenum activeUniformQueryProps[] {
			GL_TYPE, GL_ARRAY_SIZE, GL_BLOCK_INDEX, GL_OFFSET,
			GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR, GL_ATOMIC_COUNTER_BUFFER_INDEX,
		};
		static char const* stringName[] {
			"Type", "Num", "BlockIndex", "Offset",
			"ArrayStride", "MatrixStride", "IsRowMajor", "AtomicCounterBufferIndex",
		};
		constexpr uint32_t QUERY_COUNT = sizeof(activeUniformQueryProps) / sizeof(activeUniformQueryProps[0]);
		GLint uniformQueryResult[QUERY_COUNT];

		for (GLint j = 0; j < length; ++j)
		{
			GL_CALL(glGetProgramResourceName(glProgram, GL_UNIFORM, arrayUniformIndices[j], UniformDesc::BUF_SIZE, nullptr, uniformName));
			outUniformDesc._uniformInfo._uniformIndices[uniformName] = j;

			glGetProgramResourceiv(glProgram, GL_UNIFORM, arrayUniformIndices[j], QUERY_COUNT, activeUniformQueryProps, QUERY_COUNT, nullptr, uniformQueryResult);
			for (GLint j = 0; j < QUERY_COUNT; ++j)
			{
				printf("%s: %d, ", stringName[j], uniformQueryResult[j]);
			}
			printf("\n");
		}
#endif // defined(DEBUG_MESSAGE_REFLECT_SHADER)
	}
};

struct ProgramResourcesGL3 : public ProgramResourcesGL
{
	virtual void reflectProgram(GLint const glProgram, ProgramDesc& outProgramDesc) override final
	{
		GL_CALL(glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_BLOCKS, &_numActiveUniformBlocks));
		GL_CALL(glGetProgramiv(glProgram, GL_ACTIVE_UNIFORMS, &outProgramDesc._numActiveUniform));

		_maxNumVariablesInActiveUniformBlock = 0;
	}

	virtual void reflectUniformDesc(GLint const glProgram, UniformDesc& outUniformDesc) override final
	{
		const GLint blockIndex = outUniformDesc._blockIndex;

		GL_CALL(glGetActiveUniformBlockName(glProgram, blockIndex, UniformDesc::BUF_SIZE, nullptr, outUniformDesc._blockName));

		outUniformDesc._uniformInfo._activeBlockIndex = blockIndex;
		GL_CALL(glGetActiveUniformBlockiv(glProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &outUniformDesc._uniformInfo._blockSize));

		GL_CALL(glGetActiveUniformBlockiv(glProgram, blockIndex, GL_UNIFORM_BLOCK_BINDING, &outUniformDesc._binding));

		GLenum pnames[] = {
			GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, 0,
		};
		GLint numActiveVariables = 0;
		GL_CALL(glGetActiveUniformBlockiv(glProgram, blockIndex, pnames[0], &numActiveVariables));

		std::vector<GLuint> arrayUniformIndices(numActiveVariables);
		GL_CALL(glGetActiveUniformBlockiv(glProgram, blockIndex, pnames[1], (GLint*)(arrayUniformIndices.data())));

		GLchar uniformName[UniformDesc::BUF_SIZE];
		for (GLint j = 0; j < numActiveVariables; ++j)
		{
			glGetActiveUniformName(glProgram, arrayUniformIndices[j], UniformDesc::BUF_SIZE, nullptr, uniformName);
			outUniformDesc._uniformInfo._uniformIndices[uniformName] = j;
		}

#if defined(DEBUG_MESSAGE_REFLECT_SHADER)
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

		for (GLint j = 0; j < numActiveVariables; ++j)
		{
			glGetActiveUniformName(glProgram, arrayUniformIndices[j], UniformDesc::BUF_SIZE, nullptr, uniformName);
			outUniformDesc._uniformInfo._uniformIndices[uniformName] = j;

			for (GLint k = 0; k < queryCount; ++k)
			{
				glGetActiveUniformsiv(glProgram, 1, &arrayUniformIndices[j], activeUniformQueryPnames[k].pname, &uniformQueryResult[k]);
				printf("%s: %d, ", activeUniformQueryPnames[k].stringName, uniformQueryResult[k]);
			}
			printf("\n");
		}
#endif // defined(DEBUG_MESSAGE_REFLECT_SHADER)
	}
};

struct ProgramResourcesGLFallback : public ProgramResourcesGL
{
	virtual void reflectProgram(GLint const glProgram, ProgramDesc& outProgramDesc) override final
	{
		// Do not come here
		assert(false);
		printf("Can not use buffer as an uniform buffer. XREngine couldn't support this environment.\n\n");

		// Directly set
		_activeUniformBlocks["Materials"] = {};
		_activeUniformBlocks["LightBlock"] = {};
		_activeUniformBlocks["MatrixBlock"] = {};
		_numActiveUniformBlocks = static_cast<GLint>(_activeUniformBlocks.size());

		_maxNumVariablesInActiveUniformBlock = 0;
	}

	virtual void reflectUniformDesc(GLint const glProgram, UniformDesc& outUniformDesc) override final
	{
		assert(false);
	}
};

bool XRPipelineGL::createBindingInformation()
{
	static int isProgramInterfaceQueriable = glfwExtensionSupported("GL_ARB_program_interface_query");
	static bool doQueryProgramInterface = (isProgramInterfaceQueriable == GLFW_TRUE)
		|| (glfwGetProcAddress("glGetProgramInterfaceiv") != nullptr);

	static int isUniformBufferObjectQueriable = glfwExtensionSupported("GL_ARB_uniform_buffer_object");
	static bool doQueryUniformBuffer = (isUniformBufferObjectQueriable == GLFW_TRUE)
		|| (glfwGetProcAddress("glGetActiveUniformsiv") != nullptr);

	ProgramResourcesGL* programResourcesGL = nullptr;
	if (doQueryProgramInterface == true)
		programResourcesGL = new (&_programResources) ProgramResourcesGL4();
	else if (doQueryUniformBuffer == true)
		programResourcesGL = new (&_programResources) ProgramResourcesGL3();
	else
		programResourcesGL = new (&_programResources) ProgramResourcesGLFallback();


	GL_CALL(glUseProgram(_glProgram));

	printf("=============================================\n");
	printf("Reflect programs\n\n");

	ProgramResourcesGL::ProgramDesc programDesc;
	GLint& numActiveUniform = programDesc._numActiveUniform;
	programResourcesGL->reflectProgram(_glProgram, programDesc);

	printf("Number of uniform block resources: %d\n", _programResources._numActiveUniformBlocks);
	printf("Max number of uniform block variables: %d\n", _programResources._maxNumVariablesInActiveUniformBlock);
	printf("Number of uniform resources:  %d\n", numActiveUniform);

	_programResources._indexedUniformBindingInfo.resize(_programResources.GetActiveUniformBlocks());
	_programResources._indexedActiveUniformNames.resize(_programResources.GetActiveUniformBlocks());

	printf("=============================================\n");
	printf("Reflect uniform blocks\n\n");

	GLsizei length;
	GLint resultParams[64] = { -1, -1 };
	constexpr GLsizei maxResultSize = sizeof(resultParams) / sizeof(resultParams[0]);
	struct ActiveUniformBlockRange
	{
		GLint numActiveVariables;
		GLuint uniformIndices[1];
	};

	GLint numUniformBlocks = static_cast<GLint>(_programResources.GetActiveUniformBlocks());

	
	ProgramResourcesGL::UniformDesc uniformDesc;
	for (GLint i = 0; i < numUniformBlocks; ++i)
	{
		uniformDesc._blockIndex = i;
		programResourcesGL->reflectUniformDesc(_glProgram, uniformDesc);

		auto result = _programResources._activeUniformBlocks.insert({ uniformDesc._blockName, {} });
		assert(result.second == true);

		UniformInfo& uniformInfo = result.first->second;
		uniformInfo = std::move(uniformDesc._uniformInfo);

		_programResources._indexedActiveUniformNames[i] = uniformDesc._blockName;
		_programResources._indexedUniformBindingInfo[i]._uniformInfo = &uniformInfo;
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

	struct BufferRange
	{
		GLint64 start = 0;
		GLint64 size = 0;
	} bufferRange;
	GL_CALL(glGetInteger64i_v(GL_UNIFORM_BUFFER_START, UNIFORM_BINDING_NAME::Matrix, &bufferRange.start));
	GL_CALL(glGetInteger64i_v(GL_UNIFORM_BUFFER_SIZE, UNIFORM_BINDING_NAME::Matrix, &bufferRange.size));
#endif // _DEBUG

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
	//glCompileShaderIncludeARB();

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