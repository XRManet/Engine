// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#endif

#include <stdio.h>
#include <utility>
#include <string>

#include <memory>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
