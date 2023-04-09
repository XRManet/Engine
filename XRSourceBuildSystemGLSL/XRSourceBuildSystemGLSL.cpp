// XRSourceBuildSystemGLSL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "XRFrameworkBase/XRFrameworkBase.h"
#include "XRFrameworkBase/XRPipeline.h"
#include "framework.h"

#include <shaderc/shaderc.hpp>

namespace xr
{
//EXPLICIT_GEN_INDEXED_STRING_SYMBOL(XRGLSL);
}

static XRBuildSystemAvailability g_buildSystemAvailability {
	.runtimeCompile =	true,
	.offlineCompile =	false,
	.compatibleObject = false,
};

static const bool g_useVulkan = true;

XRSourceBuildSystemGLSL* g_lastShaderBuildSystem = nullptr;

struct SourceBuildSystemGLSLInitializer
{
	SourceBuildSystemGLSLInitializer()
	{
		if (g_useVulkan == false)
		{
			glewExperimental = GL_TRUE;
			GLenum result = glewInit();
			if (result != GLEW_OK)
			{
				assert(false && "It must be called after make context current.");
				throw;
			}
		}
		else
		{
			_dsoGlslang = XRPlatform::LoadDSO("shaderc_shared");
		}
		g_lastShaderBuildSystem = new XRSourceBuildSystemGLSL;
	}

	static SourceBuildSystemGLSLInitializer& GetInitializer()
	{
		static SourceBuildSystemGLSLInitializer __init;
		return __init;
	}

private:
	XRPlatform::XRDSO* _dsoGlslang;
};

#define DEFAULT_RESOURCE_PATH "Resources/Shaders/GLSL/"
constexpr uint32_t lenResourcePath = sizeof(DEFAULT_RESOURCE_PATH) - 1;

XRSourceBuildSystem* xrLoadShaderBuildSystem()
{
	SourceBuildSystemGLSLInitializer::GetInitializer();

	XRCompilerGLSL* compilerGlsl = static_cast<XRCompilerGLSL*>(g_lastShaderBuildSystem->getCompiler());
	compilerGlsl->_buildSystem = g_lastShaderBuildSystem;

	return g_lastShaderBuildSystem;
}


XRSourceBuildSystemGLSL::XRSourceBuildSystemGLSL()
	: XRSourceBuildSystem(new XRCompilerGLSL(g_buildSystemAvailability))
{
	_impl = new XRSourceBuildSystemBase(DEFAULT_RESOURCE_PATH);

	static auto recursiveDirectoryIterator = std::filesystem::recursive_directory_iterator(_impl->_rootDirectory);
	for (auto& entry : recursiveDirectoryIterator)
	{
		std::string filepath = entry.path().string();

		FILE* fp = nullptr;
		errno_t error = xr::fopen(&fp, filepath.c_str(), "r");

		assert(fp != nullptr);
		if (error != 0 || fp == nullptr) continue;

		bool result = (fseek(fp, 0, SEEK_END) == 0);
		if (result == true)
		{
			long lenShaderSource = ftell(fp);
			rewind(fp);

			char* shaderSource = new char[lenShaderSource + 1]();
			fread(shaderSource, sizeof(char), lenShaderSource, fp);

			int32_t lenFilename = filepath.length() - (lenResourcePath - 1);
			const char* filename = filepath.c_str() + (lenResourcePath - 1);

			if (g_useVulkan == false)
			{
				glNamedStringARB(GL_SHADER_INCLUDE_ARB,
					lenFilename, filename,
					static_cast<int>(lenShaderSource), shaderSource);
			}
			else
			{
				_glslSources.insert({ filename, shaderSource });
				delete[] shaderSource;
			}

			GLenum error = glGetError();
			if (error != GL_NO_ERROR)
			{
				printf("Error: %s\n", GetGlErrorString(error));
			}
		}

		fclose(fp);
	}
}

XRSourceBuildSystemGLSL::~XRSourceBuildSystemGLSL()
{
}

void XRSourceBuildSystemGLSL::registerCompiledObject(uint64_t uniqueKey, XRCompiledObject* compiledObject)
{
	_impl->registerCompiledObject(uniqueKey, compiledObject);
}

// This is the constructor of a class that has been exported.
static int isProgramInterfaceQueriable = 0;
static int isUniformBufferObjectQueriable = 0;
static int isShadingLanguageSupportInclude = 0;
XRCompilerGLSL::XRCompilerGLSL(XRBuildSystemAvailability availability)
    : XRCompiler(availability)
{
	if (g_useVulkan == false)
	{
		isProgramInterfaceQueriable = glfwExtensionSupported("GL_ARB_program_interface_query");
		static bool doQueryProgramInterface = (isProgramInterfaceQueriable == GLFW_TRUE)
			|| (glfwGetProcAddress("glGetProgramInterfaceiv") != nullptr);

		isUniformBufferObjectQueriable = glfwExtensionSupported("GL_ARB_uniform_buffer_object");
		static bool doQueryUniformBuffer = (isUniformBufferObjectQueriable == GLFW_TRUE)
			|| (glfwGetProcAddress("glGetActiveUniformsiv") != nullptr);

		isShadingLanguageSupportInclude = glfwExtensionSupported("GL_ARB_shading_language_include");
		static bool doQueryNamedString = (isShadingLanguageSupportInclude == GLFW_TRUE)
			|| (glfwGetProcAddress("glNamedStringARB") != nullptr);
	}
	else
	{
	}
    return;
}

#pragma optimize("", off)

bool CheckShaderState(GLuint shader)
{
	GLint result = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0)
		{
			GLchar* failedLog = new GLchar[logLength]();
			glGetShaderInfoLog(shader, logLength, &logLength, failedLog);

			printf("%s", failedLog);
		}
	}

	return result == GL_TRUE;
}

void XRCompilerGLSL::LoadSourceFiles()
{

}

XRCompiledObject* XRCompilerGLSL::Compile(const char* cstrSourcePath, XRCompileOptions* compileOptions)
{
	xr::IndexedString<XRGLSL> sourcePath = cstrSourcePath;

	XRCompiledObject* shaderObject = nullptr;
	
	bool result = true;
	if (g_useVulkan == false)
	{
		XRGLCompileOptions* glCompileOptions = static_cast<XRGLCompileOptions*>(compileOptions);
		XRGLShaderObject* glShaderObject = new XRGLShaderObject(glCompileOptions->_compileType, sourcePath);
		GLenum error = glGetError();
		assert(GL_NO_ERROR == error);

		if (isShadingLanguageSupportInclude == true)
		{
			char RESOURCE_PATH[256] = "/";
			char* resourcePath = RESOURCE_PATH;
			strcpy_s(RESOURCE_PATH + 1, 255, cstrSourcePath);

			GLint sourceLength = 0;
			glGetNamedStringivARB(sourcePath.length() + 1, resourcePath, GL_NAMED_STRING_LENGTH_ARB, &sourceLength);
			error = glGetError();
			assert(GL_NO_ERROR == error);

			char* buffer = new char[sourceLength + 1]();
			glGetNamedStringARB(sourcePath.length() + 1, resourcePath, sourceLength + 1, &sourceLength, buffer);
			error = glGetError();
			assert(GL_NO_ERROR == error);

			glShaderSource(glShaderObject->_shader, 1, &buffer, &sourceLength);
			error = glGetError();
			assert(GL_NO_ERROR == error);

			const char* includePaths[] = { "/", };
#if XR_PLATFORM == XR_PLATFORM_WINDOWS
			int includePathLengths[] = { 1, };
#elif XR_PLATFORM == XR_PLATFORM_OSX
			int includePathLengths[] = { 2, };
#endif // XR_PLATFORM

			glCompileShaderIncludeARB(glShaderObject->_shader, 1, includePaths, includePathLengths);
			error = glGetError();
			assert(GL_NO_ERROR == error);

			delete[] buffer;
		}
		else
		{
			char RESOURCE_PATH[256] = DEFAULT_RESOURCE_PATH;
			char* shaderSource = nullptr;
			int size = 0;

			strcpy_s(RESOURCE_PATH + lenResourcePath, 256 - lenResourcePath, cstrSourcePath);
			FILE* fp = nullptr;
			errno_t error = xr::fopen(&fp, RESOURCE_PATH, "r");

			assert(fp != nullptr);
			if (error == 0 && fp != nullptr)
			{
				result = (fseek(fp, 0, SEEK_END) == 0);
				if (result == true)
				{
					long filesize = ftell(fp);
					rewind(fp);

					char* buffer = new char[filesize + 1]();
					fread(buffer, sizeof(char), filesize, fp);

					shaderSource = buffer;
					size = static_cast<int>(filesize);
				}

				if (result == true)
				{
					constexpr uint32_t numSourceFiles = 1;
					glShaderSource(glShaderObject->_shader, numSourceFiles, &shaderSource, &size);
					glCompileShader(glShaderObject->_shader);

					delete[] shaderSource;
				}

				fclose(fp);
			}
			else
			{
				result = false;
			}
		}

		if (result == true)
			result = CheckShaderState(glShaderObject->_shader);

		shaderObject = glShaderObject;
	}
	else
	{
		shaderc::Compiler test_compiler;

		auto glslBuildSystem = static_cast<XRSourceBuildSystemGLSL*>(_buildSystem);
		auto& source = glslBuildSystem->getSource(cstrSourcePath);
		test_compiler.CompileGlslToSpv(source, shaderc_shader_kind::shaderc_vertex_shader, cstrSourcePath);
	}

	if (result == true)
	{
		uint64_t compiledObjectId = 0
			| (uint64_t(sourcePath.getIndex()) << 48ull)
			| compileOptions->getOptionHash();

		_buildSystem->registerCompiledObject(compiledObjectId, shaderObject);
	}
	else
	{
		if (shaderObject) { delete shaderObject; shaderObject = nullptr; }
	}

	return shaderObject;
}

XRCompiledObject* XRCompilerGLSL::Load(const char* objectFile, XRLoadOptions*)
{
	return nullptr;
}

enum XRShaderStage
{
	Vertex,
	Fragment,
	NumMaxShaderStages
};

XRExecutable* XRCompilerGLSL::BuildExecutable(XRBuildConfiguration*, XRBuildItemManifest* buildItemManifest)
{
	XRGLProgram* xrGlProgram = new XRGLProgram;

	glProgramParameteri(xrGlProgram->_program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	glProgramParameteri(xrGlProgram->_program, GL_PROGRAM_SEPARABLE, GL_TRUE);

	XRShaderStageDescription const* shaderStageDescription = buildItemManifest->_shaderStageDescription;

	uint32_t numStages = 0;
	XRGLShaderObject* shaderObjects[NumMaxShaderStages] = { 0, };

	if (nullptr != shaderStageDescription->_vertexFilename)
	{
		XRGLCompileOptions compileOption = {};
		compileOption._compileType = GL_VERTEX_SHADER;

		XRGLShaderObject* vertexShader = static_cast<XRGLShaderObject*>(Compile(shaderStageDescription->_vertexFilename, &compileOption));

		shaderObjects[XRShaderStage::Vertex] = vertexShader;
		numStages++;
	}

	if (nullptr != shaderStageDescription->_fragmentFilename)
	{
		XRGLCompileOptions compileOption = {};
		compileOption._compileType = GL_FRAGMENT_SHADER;

		XRGLShaderObject* fragmentShader = static_cast<XRGLShaderObject*>(Compile(shaderStageDescription->_fragmentFilename, &compileOption));

		shaderObjects[XRShaderStage::Fragment] = fragmentShader;
		numStages++;
	}

	if (numStages > 0)
	{
		for (GLuint i = 0; i < numStages; ++i)
		{
			glAttachShader(xrGlProgram->_program, shaderObjects[i]->_shader);
		}

		glLinkProgram(xrGlProgram->_program);

		GLint result = GL_FALSE;
		glGetProgramiv(xrGlProgram->_program, GL_LINK_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLint logLength = 0;
			glGetProgramiv(xrGlProgram->_program, GL_INFO_LOG_LENGTH, &logLength);

			if (logLength > 0)
			{
				GLchar* failedLog = new GLchar[logLength]();
				glGetProgramInfoLog(xrGlProgram->_program, logLength, &logLength, failedLog);

				printf("Link Error: %s", failedLog);
			}
		}

		for (GLuint i = 0; i < numStages; ++i)
		{
			glDetachShader(xrGlProgram->_program, shaderObjects[i]->_shader);
		}
	}

	return xrGlProgram;
}

XRLinkable* XRCompilerGLSL::BuildLibrary()
{
	return nullptr;
}

XRGLProgram::XRGLProgram()
{
	_program = glCreateProgram();
}

XRGLProgram::~XRGLProgram()
{
	glDeleteProgram(_program);
}

XRGLShaderObject::XRGLShaderObject(GLenum shaderType, xr::IndexedString<XRGLSL> sourcePath)
	: _sourcePath(sourcePath)
{
	_shader = glCreateShader(shaderType);
}

XRGLShaderObject::~XRGLShaderObject()
{
	glDeleteShader(_shader);
}

uint64_t XRGLCompileOptions::getOptionHash()
{
	uint64_t compileTypeValue = 0;
	switch (_compileType)
	{
	case GL_VERTEX_SHADER:			compileTypeValue = 1; break;
	case GL_TESS_CONTROL_SHADER:	compileTypeValue = 2; break;
	case GL_TESS_EVALUATION_SHADER:	compileTypeValue = 3; break;
	case GL_GEOMETRY_SHADER:		compileTypeValue = 4; break;
	case GL_FRAGMENT_SHADER:		compileTypeValue = 5; break;
	case GL_COMPUTE_SHADER:			compileTypeValue = 6; break;
	}

	uint64_t optionHash = 0
		| compileTypeValue;

	return optionHash;
}
