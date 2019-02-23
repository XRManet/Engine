#pragma once

#include "stdafx.h"

// Todo) ����� �α� �и�
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