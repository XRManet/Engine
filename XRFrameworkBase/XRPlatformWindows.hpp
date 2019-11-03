//
//  XRPlatformWindows.hpp
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#include "XRPlatform.h"
#include <Windows.h>

namespace XRPlatform
{
    template<>
    struct XRDSOImpl<Windows> : public XRDSO
    {
        friend XRDSO* LoadDSO(char const* dso_name);
        friend void UnloadDSO(XRDSO* dso);
        
    private:
        HMODULE _handle = nullptr;
        
        XRDSOImpl(const char* dso_name)
        {
            _handle = ::LoadLibraryA(dso_name);
        }
        virtual ~XRDSOImpl()
        {
            if(_handle) ::FreeLibrary(_handle);
        }
        
    public:
        void* GetProcAddress(const char* proc)
        {
			return ::GetProcAddress(_handle, proc);
        }
        
        void ListDllFunctions(std::vector<std::string>& listOfFunctionNames)
        {
            // Referenced from)
            // https://stackoverflow.com/questions/1128150/win32-api-to-enumerate-dll-export-functions
            assert(((PIMAGE_DOS_HEADER)_handle)->e_magic == IMAGE_DOS_SIGNATURE);
            
            PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)((BYTE *)_handle + ((PIMAGE_DOS_HEADER)_handle)->e_lfanew);
            assert(header->Signature == IMAGE_NT_SIGNATURE);
            assert(header->OptionalHeader.NumberOfRvaAndSizes > 0);
            
            PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE *)_handle + header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
            
            //assert (exports->AddressOfNames != 0) // assertion is not preferred.
            if (exports->AddressOfNames != 0)
            {
				void const* addressOffsetOfNames = (reinterpret_cast<char const*>(_handle) + exports->AddressOfNames);
                int const* offsetOfNames = static_cast<int const*>(addressOffsetOfNames);
                for (int i = 0; i < exports->NumberOfNames; i++)
                {
                    const char* name = reinterpret_cast<char const*>(_handle) + offsetOfNames[i];
                    listOfFunctionNames.push_back(name);
                }
            }
        }
    };
}
