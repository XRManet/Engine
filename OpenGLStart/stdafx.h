// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdio.h>
#include <tchar.h>
#include <utility>
#include <string>

#include <memory>
#include <assert.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO) 나중엔 GL 관련부로 치워야함
#ifdef _DEBUG
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

#define GL_CALL(glCall) { glCall; GLenum error = glGetError(); assert(error == GL_NO_ERROR); }

#else
#define GL_CALL(x) x
#endif