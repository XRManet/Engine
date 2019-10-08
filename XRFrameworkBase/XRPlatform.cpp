//
//  XRPlatform.cpp
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#include "stdafx.h"

#include "XRPlatform.h"

#if defined(_WIN32) || defined(_WIN64)
#include "XRPlatformWindows.hpp"
static const XRPlatform::Names XRCurrentPlatformName = XRPlatform::Windows;
#else
#include "XRPlatformOSX.hpp"
static const XRPlatform::Names XRCurrentPlatformName = XRPlatform::OSX;
#endif

namespace XRPlatform
{
	XRDSO* LoadDSO(char const* dso_name)
	{
		XRDSO* ret = new XRDSOImpl<XRCurrentPlatformName>(dso_name);
		return ret;
	}

	void UnloadDSO(XRDSO* dso)
	{
		if (dso) delete dso;
	}

	void* GetProcAddress(XRDSO* dso, char const* proc)
	{
		auto platform_dso = static_cast<XRDSOImpl<XRCurrentPlatformName>*>(dso);
		return platform_dso->GetProcAddress(proc);
	}

	void ListDLLFunctions(XRDSO* dso, std::vector<std::string>& listOfFunctionNames)
	{
		auto platform_dso = static_cast<XRDSOImpl<XRCurrentPlatformName>*>(dso);
		return platform_dso->ListDllFunctions(listOfFunctionNames);
	}
}
