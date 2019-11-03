//
//  XRPlatform.h
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#ifndef XRPlatform_h
#define XRPlatform_h

#include <vector>
#include <string>

namespace XRPlatform
{
	enum Names
	{
		NotImplemented,
		Windows,
		OSX,
		Count
	};

	struct XRBaseExport XRDSO
	{
		XRDSO() {}
		virtual ~XRDSO() {}

		bool isValid() { return false; }
	};
	template<unsigned PlatformName> struct XRDSOImpl;

	XRBaseExport XRDSO* LoadDSO(char const* dso_name);
	XRBaseExport void UnloadDSO(XRDSO* dso);

	XRBaseExport void* GetProcAddress(XRDSO* dso, char const* proc);
	XRBaseExport void ListDLLFunctions(XRDSO* dso, std::vector<std::string>& listOfFunctionNames);
};

#define XR_PLATFORM_UNDEFINED   0
#define XR_PLATFORM_WINDOWS     1
#define XR_PLATFORM_OSX         2

#if defined(_WIN32) || defined(_WIN64)
#define XR_PLATFORM XR_PLATFORM_WINDOWS
#else
#define XR_PLATFORM XR_PLATFORM_OSX
#endif

#endif /* XRPlatform_h */
