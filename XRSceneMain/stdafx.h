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

#ifdef XRSCENEMAIN_EXPORTS
#define XRExport __declspec(dllexport)
#else
#define XRExport __declspec(dllimport)
#endif
#define XRHidden

#else
#define XRExport __attribute__((visibility("default")))
#define XRHidden __attribute__((visibility("hidden")))
#endif


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

