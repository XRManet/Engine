﻿// XRSourceBuildSystemGLSL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "XRFrameworkBase/XRFrameworkBase.h"
#include "XRFrameworkBase/XRPipeline.h"
#include "framework.h"

namespace xr
{
//EXPLICIT_GEN_INDEXED_STRING_SYMBOL(XRGLSL);
}

static XRBuildSystemAvailability g_buildSystemAvailability{
	.runtimeCompile = true,
	.offlineCompile = false,
	.compatibleObject = false,
};

XRSourceBuildSystem* g_lastShaderBuildSystem = nullptr;

XRSourceBuildSystem* xrLoadShaderBuildSystem()
{
	return g_lastShaderBuildSystem;
}

struct SourceBuildSystemGLSLInitializer
{
	SourceBuildSystemGLSLInitializer()
	{
		glewExperimental = GL_TRUE;
		GLenum result = glewInit();
		if (result != GLEW_OK)
		{
			assert(false && "It must be called after make context current.");
			throw;
		}

		g_lastShaderBuildSystem = new XRSourceBuildSystemGLSL;
	}

	static SourceBuildSystemGLSLInitializer& GetInitializer()
	{
		static SourceBuildSystemGLSLInitializer __init;
		return __init;
	}
};

XRSourceBuildSystem* xrGetShaderBuildSystem()
{
	SourceBuildSystemGLSLInitializer::GetInitializer();
	return g_lastShaderBuildSystem;
}

#define DEFAULT_RESOURCE_PATH "Resources/Shaders/OpenGL/"
constexpr uint32_t lenResourcePath = sizeof(DEFAULT_RESOURCE_PATH) - 1;

XRSourceBuildSystemGLSL::XRSourceBuildSystemGLSL()
	: XRSourceBuildSystem(new XRCompilerGLSL(this, g_buildSystemAvailability))
{
	_rootDirectory = DEFAULT_RESOURCE_PATH;

	for (auto& entry : std::filesystem::recursive_directory_iterator(_rootDirectory))
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

			glNamedStringARB(GL_SHADER_INCLUDE_ARB,
				lenFilename, filename,
				static_cast<int>(lenShaderSource), shaderSource);
		}

		fclose(fp);
	}
}

XRSourceBuildSystemGLSL::~XRSourceBuildSystemGLSL()
{
}

// This is the constructor of a class that has been exported.
static int isProgramInterfaceQueriable = 0;
static int isUniformBufferObjectQueriable = 0;
static int isShadingLanguageSupportInclude = 0;
XRCompilerGLSL::XRCompilerGLSL(XRSourceBuildSystemGLSL* glslBuildSystem, XRBuildSystemAvailability availability)
    : XRCompiler(glslBuildSystem, availability)
{

	isProgramInterfaceQueriable = glfwExtensionSupported("GL_ARB_program_interface_query");
	static bool doQueryProgramInterface = (isProgramInterfaceQueriable == GLFW_TRUE)
		|| (glfwGetProcAddress("glGetProgramInterfaceiv") != nullptr);

	isUniformBufferObjectQueriable = glfwExtensionSupported("GL_ARB_uniform_buffer_object");
	static bool doQueryUniformBuffer = (isUniformBufferObjectQueriable == GLFW_TRUE)
		|| (glfwGetProcAddress("glGetActiveUniformsiv") != nullptr);

	isShadingLanguageSupportInclude = glfwExtensionSupported("GL_ARB_shading_language_include");
	static bool doQueryNamedString = (isUniformBufferObjectQueriable == GLFW_TRUE)
		|| (glfwGetProcAddress("glNamedStringARB") != nullptr);
	
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

	XRGLCompileOptions* glCompileOptions = static_cast<XRGLCompileOptions*>(compileOptions);
	XRGLShaderObject* shaderObject = new XRGLShaderObject(glCompileOptions->_compileType, sourcePath);

	bool result = true;
	if (isShadingLanguageSupportInclude == true)
	{
		char RESOURCE_PATH[256] = "/";
		char* resourcePath = RESOURCE_PATH;
		strcpy_s(RESOURCE_PATH + 1, 255, cstrSourcePath);

		GLint sourceLength = 0;
		glGetNamedStringivARB(sourcePath.length() + 1, resourcePath, GL_NAMED_STRING_LENGTH_ARB, &sourceLength);

		char* buffer = new char[sourceLength + 1]();
		glGetNamedStringARB(sourcePath.length() + 1, resourcePath, sourceLength + 1, &sourceLength, buffer);

		const char* includePath = "/";
		glShaderSource(shaderObject->_shader, 1, &buffer, &sourceLength);
		glCompileShaderIncludeARB(shaderObject->_shader, 1, &includePath, nullptr);

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
				glShaderSource(shaderObject->_shader, numSourceFiles, &shaderSource, &size);
				glCompileShader(shaderObject->_shader);

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
		result = CheckShaderState(shaderObject->_shader);

	if (result == true)
	{
		uint64_t compiledObjectId = 0
			| (uint64_t(sourcePath.getIndex()) << 48ull)
			| glCompileOptions->getOptionHash();

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