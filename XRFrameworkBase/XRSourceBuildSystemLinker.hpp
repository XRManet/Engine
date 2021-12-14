#pragma once
#include "XRPlatform.h"

#include "XRSourceBuildSystem.h"

XRSourceBuildSystem* (*xrLoadShaderBuildSystem)() = nullptr;

#define XRSHADER_BUILD_SYSTEM_DEFAULT    XR_DYNAMIC_LIBRARY(XRSourceBuildSystemGLSL)

#ifndef XRSHADER_BUILD_SYSTEM
#define XRSHADER_BUILD_SYSTEM             XRSHADER_BUILD_SYSTEM_DEFAULT
#endif

static struct XRShaderBuildSystemLinker
{
	XRPlatform::XRDSO* _dso;

	XRShaderBuildSystemLinker()
	{
		constexpr static const char* shaderBuildSystemName = XRSHADER_BUILD_SYSTEM;
		_dso = XRPlatform::LoadDSO(shaderBuildSystemName);
		assert(_dso != nullptr);

		// Todo) List up and check availability
		std::vector<std::string> dllLists;
		XRPlatform::ListDLLFunctions(_dso, dllLists);

		GetProcAddress(xrLoadShaderBuildSystem, "xrLoadShaderBuildSystem");
	}

	~XRShaderBuildSystemLinker()
	{
		XRPlatform::UnloadDSO(_dso);
	}

private:
	template<typename func>
	void GetProcAddress(func& outFunction, const char* name)
	{
		outFunction = reinterpret_cast<func>(XRPlatform::GetProcAddress(_dso, name));
		assert(outFunction != nullptr);
	}
} SourceBuildSystemLinkerStarter;