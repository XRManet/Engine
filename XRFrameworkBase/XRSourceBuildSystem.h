#pragma once

#include <XRFrameworkBase/XRDeviceSystem.h>
#include <XRFrameworkBase/XRUtility.h>

struct XRBuildSystemAvailability
{
	STATIC_ENUM_BIT_BEGIN;
	ADD_BIT_ENUM(RuntimeCompile, runtimeCompile);
	ADD_BIT_ENUM(OfflineCompile, offlineCompile);
	ADD_BIT_ENUM(CompatibleObject, compatibleObject);
};

class XRCompiledObject
{
public:
};

class XRExecutable
{
public:
};

class XRLinkable
{
public:
};

class XRCompiler
{
public:
	XRCompiler(XRBuildSystemAvailability availability)
		: _systemAvailability(availability)
	{

	}

public:
	virtual XRCompiledObject* Compile(uint32_t numSourceFiles, const char** sourceFiles) = 0;
	virtual XRCompiledObject* Load(const char* objectFile) = 0;

public:
	virtual XRExecutable* BuildExecutable() = 0;
	virtual XRLinkable* BuildLibrary() = 0;

public:
	XRBuildSystemAvailability GetAvailability() { return _systemAvailability; }

private:
	XRBuildSystemAvailability _systemAvailability;
};

struct XRSourceBuildSystemDesc
{
public:
	
};

class XRDeviceSystem;

class XRSourceBuildSystem : public XRDeviceSystemAttachment
{
protected:
	XRCompiler* _compiler;
};

XRSourceBuildSystem* xrLoadSourceBuildSystem();