// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

//////////////////////////// Platform-independent headers
#include <XRFrameworkBase/XRDefault.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//////////////////////////// Platform-dependent headers
// 
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
