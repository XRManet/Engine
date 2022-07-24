// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

//////////////////////////// Platform-independent headers
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

//////////////////////////// Platform-dependent headers
#include <XRFrameworkBase/stdafx.h>

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

char const* GetGlErrorString(GLenum errorCode);

#if defined(_DEBUG) || defined(DEBUG)

#if TEST_BLOCK_JIMAN_1
template<typename glCall>
auto GL_CALL_AUTO(glCall glFunc) -> decltype(glFunc())
{
	auto res = glFunc();
	GLenum error = glGetError();
	assert(error == GL_NO_ERROR);
	return res;
}

template<typename glVoidCall>
void GL_CALL_VOID(glVoidCall glFunc)
{
	glFunc();
	GLenum error = glGetError();
	assert(error == GL_NO_ERROR);
}

#define GL_CALL_(glCall) [&] () -> decltype(glCall) { \
  auto res = glCall; \
  GLenum error = glGetError(); \
  assert(error == GL_NO_ERROR); \
  return res; \
}()
#endif

#define GL_CALL(glCall) { glCall; GLenum error; while(error = glGetError(), error != GL_NO_ERROR) { printf("\n<OpenGL Error: %s>\n", GetGlErrorString(error)); assert(error == GL_NO_ERROR); } }
#define GL_CALL_WARN(glCall) { glCall; GLenum error; while(error = glGetError(), error != GL_NO_ERROR) { printf("\n<OpenGL Warning: %s>\n", GetGlErrorString(error)); } }
#else
#define GL_CALL(x) x
#define GL_CALL_WARN(x) x
#endif
