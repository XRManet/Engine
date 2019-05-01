//
//  XRPlatformOSX.hpp
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#include "XRPlatform.h"
#include <dlfcn.h>

namespace XRPlatform
{
    template<>
    struct XRDSOImpl<OSX> : public XRDSO
    {
        friend XRDSO* LoadDSO(char const* dso_name);
        friend void UnloadDSO(XRDSO* dso);
        
    private:
        void* _handle = nullptr;
        
        XRDSOImpl(const char* dso_name)
        {
            _handle = dlopen(dso_name, RTLD_LOCAL);
        }
        virtual ~XRDSOImpl()
        {
            if(_handle) dlclose(_handle);
        }
        
    public:
        void* GetProcAddress(const char* proc)
        {
            return dlsym(_handle, proc);
        }
    };
}
