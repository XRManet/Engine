#pragma once

#if defined(_WIN32) || defined(_WIN64)

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <tchar.h>

#define XRExport __declspec(dllexport)
#define XRImport extern __declspec(dllimport)
#define XRHidden

#ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseExport __declspec(dllexport)
#define XRBaseSupply __declspec(dllexport)
#else
#define XRBaseExport __declspec(dllimport)
#define XRBaseSupply __declspec(dllimport)
#endif

#ifdef XRRENDERENGINE_EXPORTS
#define XRRenderExport extern "C" __declspec(dllexport)
#define XRRenderAPI(ReturnType, ApiName) extern "C" __declspec(dllexport) ReturnType ApiName
#else
#define XRRenderExport extern "C" __declspec(dllimport)
#define XRRenderAPI(ReturnType, ApiName) extern ReturnType (*ApiName)
#endif

#ifdef XRSOURCEBUILDSYSTEM_EXPORTS
#define XRShaderBuildExport __declspec(dllexport)
#define XRShaderBuildAPI(ReturnType, ApiName) extern "C" __declspec(dllexport) ReturnType ApiName
#else
#define XRShaderBuildExport __declspec(dllimport)
#define XRShaderBuildAPI(ReturnType, ApiName) extern ReturnType (*ApiName)
#endif

#elif defined(__APPLE__)

#define XRExport __attribute__((visibility("default")))
#define XRImport extern __attribute__((visibility("default")))
#define XRHidden __attribute__((visibility("hidden")))

#define XRBaseExport __attribute__((visibility("default")))
#define XRBaseHidden __attribute__((visibility("hidden")))

#ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseSupply __attribute__((visibility("default")))
#else // #ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseSupply extern __attribute__((visibility("default")))
#endif // #ifdef XRFRAMEWORKBASE_EXPORTS

#define XRRenderExport extern "C" __attribute__((visibility("default")))
#ifdef XRRENDERENGINE_EXPORTS
#define XRRenderAPI(ReturnType, ApiName) extern "C" __attribute__((visibility("default"))) ReturnType ApiName
#else
#define XRRenderAPI(ReturnType, ApiName) extern "C" __attribute__((visibility("default"))) ReturnType (*ApiName)
#endif

#define XRShaderBuildExport extern "C" __attribute__((visibility("default")))
#ifdef XRSOURCEBUILDSYSTEM_EXPORTS
#define XRShaderBuildAPI(ReturnType, ApiName) extern "C" __attribute__((visibility("default"))) ReturnType ApiName
#else
#define XRShaderBuildAPI(ReturnType, ApiName) extern "C" __attribute__((visibility("default"))) ReturnType (*ApiName)
#endif

#else
#error "Not considered yet"
#endif

constexpr int const PAGE_SIZE = 4'096;


#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <thread>
#include <functional>

#include <string>
#include <memory>

#include <utility>

#include <assert.h>
#include <errno.h>
#include <stdio.h>



namespace xr
{
#if defined(_WIN32)
	inline char* strtok(char* string, const char* delimiter, char** context)
	{
		return ::strtok_s(string, delimiter, context);
	}
	inline errno_t fopen(FILE** fp, const char* filename, const char* mode)
	{
		return ::fopen_s(fp, filename, mode);
	}
#elif XR_PLATFORM == XR_PLATFORM_OSX
	inline char* strtok(char* string, const char* delimiter, char** context)
	{
		return ::strtok_r(string, delimiter, context);
	}
	inline errno_t fopen(FILE** fp, const char* filename, const char* mode)
	{
		if (fp == nullptr || filename == nullptr || mode == nullptr)
			return EINVAL;

		*fp = ::fopen(filename, mode);
		if (nullptr != fp)
			return 0;
		return errno;
	}

#define sprintf_s 							snprintf
#define strncpy_s(dst, size, src, len) 		strncpy(dst, src, len);
#define strcpy_s(dst, size, src)			strcpy(dst, src);

#define OutputDebugStringA 					printf
#endif
}
