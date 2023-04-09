#pragma once
#include "XRPlatform.h"

#include "XRSourceBuildSystem.h"

XRSourceBuildSystem* (*xrLoadShaderBuildSystem)() = nullptr;

#define XRSHADER_BUILD_SYSTEM_DEFAULT    XR_DYNAMIC_LIBRARY(XRSourceBuildSystemGLSL)

const char* g_xrShaderBuildSystemNameDso = nullptr;

static struct XRShaderBuildSystemLinker
{
	XRPlatform::XRDSO* _dso;

	XRShaderBuildSystemLinker()
	{
		_dso = XRPlatform::LoadDSO(g_xrShaderBuildSystemNameDso);
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