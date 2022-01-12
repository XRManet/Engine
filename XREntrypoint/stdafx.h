// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include <XRFrameworkBase/stdafx.h>

#if defined(_WIN32) || defined(_WIN64)
#include <tchar.h>

#define XRExport __declspec(dllexport)
#define XRImport extern __declspec(dllimport)

#elif defined(__APPLE__)

#define XRExport __attribute__((visibility("default")))
#define XRImport extern __attribute__((visibility("default")))

#else
#error "Not considered yet"
#endif

#include <utility>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <XRFrameworkBase/XRUtility.h>

char const* GetGlErrorString(GLenum errorCode);

// TODO) 나중엔 GL 관련부로 치워야함
#if defined(_DEBUG) || defined(DEBUG)

#define GL_CALL(glCall) { glCall; GLenum error; while(error = glGetError(), error != GL_NO_ERROR) { printf("\n<OpenGL Error: %s>\n", GetGlErrorString(error)); assert(error == GL_NO_ERROR); } }
#define GL_CALL_WARN(glCall) { glCall; GLenum error; while(error = glGetError(), error != GL_NO_ERROR) { printf("\n<OpenGL Warning: %s>\n", GetGlErrorString(error)); } }

#else
#define GL_CALL(x) x
#define GL_CALL_WARN(x) x
#endif
