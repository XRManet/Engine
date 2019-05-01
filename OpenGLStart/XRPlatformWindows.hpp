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
        virtual ~XRDSOImple()
        {
            if(_handle) ::FreeLibrary(_handle);
        }
        
    public:
        void* GetProcAddress(const char* proc)
        {
            return ::GetProcAddress(_handle, proc):
        }
    };
}
