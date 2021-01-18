// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

//////////////////////////// Platform-independent headers
#include <GL/glew.h>

#include <vector>
#include <string>
#include <assert.h>

//////////////////////////// Platform-dependent headers
#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#include "targetver.h"
#include <windows.h>
#elif defined(__APPLE__)
#endif

/* XRRenderExport
 * XRRENDERENGINEGL_EXPORTS is defined in settings for a project file per each platform-dependent IDE.
 */
#ifdef XRRENDERENGINEGL_EXPORTS
#if defined(_WIN32) || defined(_WIN64)
#define XRRenderExport extern "C" __declspec(dllexport)
#elif (__APPLE__)
#define XRRenderExport extern "C" __attribute__((visibility("default")))
#else
#error "Not considered yet"
#endif
#endif

/* XRBaseExport
 */
#ifndef XRFRAMEWORKBASE_EXPORTS
#if defined(_WIN32) || defined(_WIN64)
#define XRExport __declspec(dllexport)
#define XRImport extern __declspec(dllimport)

#define XRBaseExport __declspec(dllimport)
#define XRBaseSupply __declspec(dllimport)

#elif (__APPLE__)
#define XRExport __attribute__((visibility("default")))
#define XRImport extern __attribute__((visibility("default")))

#define XRBaseExport __attribute__((visibility("default")))
#define XRBaseSupply extern __attribute__((visibility("default")))

#else
#error "Not considered yet"
#endif
#endif

//////////////////////////// Variadic per each Engine Module

#include <XRFrameworkBase/XRUtility.h>

#if defined(_DEBUG) || defined(DEBUG)
#define GL_CALL(glCall) { glCall; GLenum error = glGetError(); if(error != GL_NO_ERROR) { printf("GL Error %x\n", error); assert(error == GL_NO_ERROR); } }
#else
#define GL_CALL(x) x
#endif
