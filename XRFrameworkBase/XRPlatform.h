//
//  XRPlatform.h
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#ifndef XRPlatform_h
#define XRPlatform_h

#include <vector>
#include <string>

namespace XRPlatform
{
    enum Names
    {
        NotImplemented,
        Windows,
        OSX,
        Count
    };
    
    struct XRDSO
    {
        XRDSO()             {}
        virtual ~XRDSO()    {}
        
        bool isValid()      { return false; }
    };
    template<unsigned PlatformName> struct XRDSOImpl;
    
    XRDSO* LoadDSO(char const* dso_name);
    void UnloadDSO(XRDSO* dso);
    
    void* GetProcAddress(XRDSO* dso, char const* proc);
    void ListDLLFunctions(XRDSO* dso, std::vector<std::string>& listOfFunctionNames);
};


#endif /* XRPlatform_h */
