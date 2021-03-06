// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

#ifdef XRFRAMEWORKBASE_EXPORTS
#define XRBaseExport __declspec(dllexport)
#else
#define XRBaseExport __declspec(dllimport)
#endif
#define XRBaseHidden

#ifndef XRRENDERENGINEGL_EXPORTS
#define XRRenderExport extern __declspec(dllimport)
#endif

#else
#define XRBaseExport __attribute__((visibility("default")))
#define XRBaseHidden __attribute__((visibility("hidden")))

#define XRRenderExport extern "C" __attribute__((visibility("default")))

#endif

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include <assert.h>

#include <stdio.h>
#include <errno.h>

#include "XRFrameworkBaseRenderEngine.h"

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
