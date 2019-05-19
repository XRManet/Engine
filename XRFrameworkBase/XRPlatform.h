#pragma once

#include "stdafx.h"

namespace XRPlatform
{
#if defined(_WIN32) || defined(_WIN64)
  typedef HMODULE XRDSO;
#endif

  XRBaseExport XRDSO LoadDSO(char const* dso_name);
  XRBaseExport void UnloadDSO(XRDSO dso);

  XRBaseExport void* GetProcAddress(XRDSO dso, char const* proc);
  XRBaseExport void ListDLLFunctions(XRDSO dso, std::vector<std::string>& listOfFunctionNames);
};
