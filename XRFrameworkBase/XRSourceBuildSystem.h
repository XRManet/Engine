#pragma once

#include <XRFrameworkBase/XRDeviceSystem.h>
#include <XRFrameworkBase/XRUtility.h>

struct XRShaderStageDescription;
class XRSourceBuildSystem;

XRBaseExport XRSourceBuildSystem* xrGetShaderBuildSystem();

XRShaderBuildAPI(XRSourceBuildSystem*, xrLoadShaderBuildSystem)();


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

class XRSourceBuildSystemBase
{
public:
	std::string _rootDirectory;
	std::unordered_map<uint64_t, XRCompiledObject*> _compiledObjects;

public:
	XRSourceBuildSystemBase(std::string&& rootDirectory) : _rootDirectory(move(rootDirectory)) {}
	virtual ~XRSourceBuildSystemBase() = default;

public:
	inline void registerCompiledObject(uint64_t uniqueKey, XRCompiledObject* compiledObject)
	{
		auto result = _compiledObjects.insert({ uniqueKey, nullptr });
		if (true == result.second)
		{
			result.first->second = compiledObject;
		}
	}
};

class XRSourceBuildSystem;
class XRBaseExport XRCompiler
{
public:
	XRCompiler(XRBuildSystemAvailability availability)
		: _systemAvailability(availability)
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
#ifdef XRSOURCEBUILDSYSTEMGLSL_EXPORTS
	friend XRSourceBuildSystem* xrLoadShaderBuildSystem();
#endif

protected:
	XRCompiler* _compiler;
	XRSourceBuildSystemBase* _impl;

public:
	XRSourceBuildSystem(XRCompiler* compiler);
	virtual ~XRSourceBuildSystem() = default;

public:
	XRCompiler* getCompiler() const { return _compiler; }

	virtual void registerCompiledObject(uint64_t uniqueKey, XRCompiledObject* compiledObject) = 0;
};
