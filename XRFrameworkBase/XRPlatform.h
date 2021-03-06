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

///< XR_PLATFORM.
#define XR_PLATFORM_UNDEFINED   0
#define XR_PLATFORM_WINDOWS     1
#define XR_PLATFORM_OSX         2

///< XR_BUILD_SYSTEM.
#define XR_BUILD_SYSTEM_UNDEFINED		0
#define XR_BUILD_SYSTEM_MSBUILD_VS		1
#define XR_BUILD_SYSTEM_XCODE			2

#if defined(_WIN32) || defined(_WIN64)
#define XR_PLATFORM				XR_PLATFORM_WINDOWS
#if defined(_MSC_BUILD)
#define XR_BUILD_SYSTEM			XR_BUILD_SYSTEM_MSBUILD_VS
#define XR_BUILD_SYSTEM_MSBUILD_VS_2019 (_MSC_VER >= 1920)
#define XR_BUILD_SYSTEM_MSBUILD_VS_2017 (_MSC_VER >= 1910)
#endif // defined(_MSC_BUILD)
#else
#define XR_PLATFORM				XR_PLATFORM_OSX
#define XR_BUILD_SYSTEM			XR_BUILD_SYSTEM_XCODE
#endif

#endif /* XRPlatform_h */
