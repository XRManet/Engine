#pragma once

#include <XRFrameworkBase/XRDefault.h>
#include <XRFrameworkBase/XRSourceBuildSystem.h>
#include <XRFrameworkBase/XRIndexedString.h>

struct XRGLSL; // using for IndexedString<XRGLSL>

struct XRShaderStageDescription;
class XRSourceBuildSystemGLSL;

struct XRGLCompileOptions : public XRCompileOptions
{
	GLenum _compileType = 0;

	uint64_t getOptionHash() override final;
};

class XRGLShaderObject : public XRCompiledObject
{
public:
	xr::IndexedString<XRGLSL> _sourcePath;
	GLuint _shader;

public:
	XRGLShaderObject(GLenum shaderType, xr::IndexedString<XRGLSL> sourcePath);
	virtual ~XRGLShaderObject();
};

class XRGLProgram : public XRExecutable
{
public:
	GLuint _program;

public:
	XRGLProgram();
	virtual ~XRGLProgram();
};

class XRCompilerGLSL : public XRCompiler
{
public:
	XRCompilerGLSL(XRSourceBuildSystemGLSL* glslBuildSystem, XRBuildSystemAvailability availability);

public:
	void LoadSourceFiles();

public:
	XRCompiledObject* Compile(const char* sourceFilePath, XRCompileOptions*) override final;
	XRCompiledObject* Load(const char* objectFilePath, XRLoadOptions*) override final;

public:
	XRExecutable* BuildExecutable(XRBuildConfiguration*, XRBuildItemManifest*) override final;
	XRLinkable* BuildLibrary() override final;
};

class XRSourceBuildSystemGLSL : public XRSourceBuildSystem
{
public:
	XRSourceBuildSystemGLSL();
	virtual ~XRSourceBuildSystemGLSL();
};