// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <GL/glew.h>

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

#endif

#include <assert.h>

#ifdef XRRENDERENGINEGL_EXPORTS
#define XRRenderExport extern "C" __declspec(dllexport)
#endif

#ifndef XRFRAMEWORKBASE_EXPORTS
#define XRBaseExport __declspec(dllimport)
#endif

#ifdef _DEBUG
#define GL_CALL(glCall) { glCall; GLenum error = glGetError(); assert(error == GL_NO_ERROR); }
#else
#define GL_CALL(x) x
#endif
