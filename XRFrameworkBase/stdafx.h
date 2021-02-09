// XRFrameworkBase/stdafx.h
#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

#define XRExport __declspec(dllexport)
#define XRImport extern __declspec(dllimport)

#ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseExport __declspec(dllexport)
#define XRBaseSupply __declspec(dllexport)
#else
#define XRBaseExport __declspec(dllimport)
#define XRBaseSupply __declspec(dllimport)
#endif

#ifndef XRRHIOPENGL_EXPORTS
#define XRRenderExport extern "C" __declspec(dllimport)
#endif

#elif defined(__APPLE__)

#define XRExport __attribute__((visibility("default")))
#define XRImport extern __attribute__((visibility("default")))

#define XRBaseExport __attribute__((visibility("default")))
#define XRBaseHidden __attribute__((visibility("hidden")))

#ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseSupply __attribute__((visibility("default")))
#else // #ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseSupply extern __attribute__((visibility("default")))
#endif // #ifdef XRFRAMEWORKBASE_EXPORTS

#define XRRenderExport extern "C" __attribute__((visibility("default")))

#else
#error "Not considered yet"
#endif

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <assert.h>

#include <stdio.h>
#include <errno.h>

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
#endif
}

int const PAGE_SIZE = 4'096;
