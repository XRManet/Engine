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
            char open_name[256] = {0, };
            sprintf(open_name, "lib%s.dylib", dso_name);
            _handle = dlopen(open_name, RTLD_LOCAL);
            if(_handle == nullptr)
            {
                const char* fallback_name = "libXRRenderingEngineGL.dylib";
                _handle = dlopen(fallback_name, RTLD_LOCAL);
            }
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
        
        void ListDllFunctions(std::vector<std::string>& listOfFunctionNames)
        {
            listOfFunctionNames.push_back("xrCreateModel");
            listOfFunctionNames.push_back("xrCreateTexture");
			listOfFunctionNames.push_back("xrCreatePipeline");
			listOfFunctionNames.push_back("xrCreateCommandBuffer");
        }
    };
}
