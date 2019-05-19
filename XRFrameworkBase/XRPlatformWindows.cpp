#include "stdafx.h"

#include "XRPlatform.h"

namespace XRPlatform
{
  XRDSO LoadDSO(char const* dso_name)
  {
    return ::LoadLibraryA(dso_name);
    //return ::LoadLibraryExA(dso_name, NULL, DONT_RESOLVE_DLL_REFERENCES);
  }

  void UnloadDSO(XRDSO dso)
  {
    ::FreeLibrary(dso);
  }

  void* GetProcAddress(XRDSO dso, char const* proc)
  {
    return ::GetProcAddress(dso, proc);
  }

  void ListDLLFunctions(XRDSO dso, std::vector<std::string>& listOfFunctionNames)
  {
    // Referenced from)
    // https://stackoverflow.com/questions/1128150/win32-api-to-enumerate-dll-export-functions
    assert(((PIMAGE_DOS_HEADER)dso)->e_magic == IMAGE_DOS_SIGNATURE);

    PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)((BYTE *)dso + ((PIMAGE_DOS_HEADER)dso)->e_lfanew);
    assert(header->Signature == IMAGE_NT_SIGNATURE);
    assert(header->OptionalHeader.NumberOfRvaAndSizes > 0);

    PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)dso + header->
      OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    
    //assert (exports->AddressOfNames != 0) // assertion is not preferred.
    if (exports->AddressOfNames != 0)
    {
      BYTE** names = (BYTE**)(reinterpret_cast<const char*>(dso) + exports->AddressOfNames);
      for (int i = 0; i < exports->NumberOfNames; i++)
      {
        const char* name = (const char*)dso + (int)names[i];
        listOfFunctionNames.push_back(name);
      }
    }
  }

}
