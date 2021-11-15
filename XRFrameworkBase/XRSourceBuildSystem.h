#pragma once

#include <XRFrameworkBase/XRDeviceSystem.h>
#include <XRFrameworkBase/XRUtility.h>

struct XRShaderStageDescription;

struct XRBuildSystemAvailability
{
	STATIC_ENUM_BIT_BEGIN;
	ADD_BIT_ENUM(RuntimeCompile, runtimeCompile);
	ADD_BIT_ENUM(OfflineCompile, offlineCompile);
	ADD_BIT_ENUM(CompatibleObject, compatibleObject);
};

struct XRBuildConfiguration {};
struct XRBuildItemManifest
{
	XRShaderStageDescription const* _shaderStageDescription;
};

/**
 * @struct	XRCompileOptions
 *
 * @brief	XRSourceBuildSystem에서 사용되는 각 option
 *
 * @author	Jiman Jeong
 * @date	2021-09-09
 */

struct XRCompileOptions
{
	virtual uint64_t getOptionHash() = 0;
};

struct XRLoadOptions
{
	virtual uint64_t getOptionHash() = 0;
};

/**
 * @class	XRCompiledObject
 *
 * @brief	XRSourceBuildSystem을 통해 컴파일된 object.
 *
 * @author	Jiman Jeong
 * @date	2021-09-09
 */

class XRBaseExport XRCompiledObject
{
public:
};

class XRBaseExport XRExecutable
{
public:
};

class XRBaseExport XRLinkable
{
public:
};

class XRSourceBuildSystem;
class XRBaseExport XRCompiler
{
public:
	XRCompiler(XRSourceBuildSystem* buildSystem, XRBuildSystemAvailability availability)
		: _buildSystem(buildSystem)
		, _systemAvailability(availability)
	{

	}

public:
	virtual XRCompiledObject* Compile(const char* sourceFiles, XRCompileOptions*) = 0;
	virtual XRCompiledObject* Load(const char* objectFile, XRLoadOptions*) = 0;

public:
	virtual XRExecutable* BuildExecutable(XRBuildConfiguration*, XRBuildItemManifest*) = 0;
	virtual XRLinkable* BuildLibrary() = 0;

public:
	XRBuildSystemAvailability GetAvailability() { return _systemAvailability; }

private:
	XRBuildSystemAvailability _systemAvailability;

protected:
	XRSourceBuildSystem* _buildSystem;
};

struct XRSourceBuildSystemDesc
{
public:
	
};

class XRDeviceSystem;

class XRBaseExport XRSourceBuildSystem : public XRDeviceSystemAttachment
{
protected:
	XRCompiler* _compiler;
	std::string _rootDirectory;

public:
	XRSourceBuildSystem(XRCompiler* compiler) : _compiler(compiler), _compiledObjects() {}

public:
	XRCompiler* getCompiler() const { return _compiler; }


	// Separate
protected:
	std::unordered_map<uint64_t, XRCompiledObject*> _compiledObjects;

public:
	void registerCompiledObject(uint64_t uniqueKey, XRCompiledObject* compiledObject);
};

XRShaderBuildAPI(XRSourceBuildSystem*, xrLoadShaderBuildSystem)();
XRShaderBuildAPI(XRSourceBuildSystem*, xrGetShaderBuildSystem)();