//
//  XRPlatform.h
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#ifndef XRPlatform_h
#define XRPlatform_h

namespace XRPlatform
{
    enum Names
    {
        NotImplemented,
        Windows,
        OSX,
        Count
    };
    
    struct XRDSO {};
    template<unsigned PlatformName> struct XRDSOImpl;
    
    XRDSO* LoadDSO(char const* dso_name);
    void UnloadDSO(XRDSO* dso);
    
    void* GetProcAddress(XRDSO* dso, char const* proc);
};


#endif /* XRPlatform_h */
