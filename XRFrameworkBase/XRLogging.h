#pragma once

#include "stdafx.h"

// Todo) 디버그 로깅 분리
#if defined(_DEBUG) || defined(DEBUG)
#if defined(_WINDOWS)
void DebugOutput(const char* szFormat, ...)
{
  char szBuff[1024];
  va_list arg;
  va_start(arg, szFormat);
  _vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
  va_end(arg);

  OutputDebugStringA(szBuff);
}
#else
#define DebugOutput(...) printf(__VA_ARGS__)
#endif
#else
#define DebugOutput(...)
#endif