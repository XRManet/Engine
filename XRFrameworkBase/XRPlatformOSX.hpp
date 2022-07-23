//
//  XRPlatformOSX.hpp
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#include "XRPlatform.h"
#include <unordered_map>
#include <dlfcn.h>
// 64bit system 에서 더 이상 구현 제공 X
// Ref. https://stackoverflow.com/questions/2965273/what-library-is-needed-for-mach-o-nlist-h-mac-os-x
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include <mach-o/dyld.h>

struct nlist_32 {
	union {
		uint32_t n_strx;	/* index into the string table */
	} n_un;
	uint8_t n_type;		/* type flag, see below */
	uint8_t n_sect;		/* section number or NO_SECT */
	int16_t n_desc;		/* see <mach-o/stab.h> */
	uint32_t n_value;	/* value of this symbol (or stab offset) */
};

namespace XRPlatform
{
    template<>
    struct XRDSOImpl<OSX> : public XRDSO
    {
        friend XRDSO* LoadDSO(char const* dso_name);
        friend void UnloadDSO(XRDSO* dso);
        
    private:
        void* _handle = nullptr;
		char _openName[256] = {0, };
		
		std::unordered_map<std::string, std::string> _tableNameToSymbol;
        
        XRDSOImpl(const char* dso_name)
        {
			if ( strncmp(dso_name, "lib", 3) == 0 && strstr(dso_name, ".dylib") != nullptr)
				sprintf(_openName, "%s", dso_name);
			else
				sprintf(_openName, "lib%s.dylib", dso_name);
			
			//_dyld_register_func_for_add_image(callback);
            _handle = dlopen(_openName, RTLD_LOCAL);
			if (_handle == nullptr)
			{
				char* error = dlerror();
				if(error != nullptr)
					printf("dlopen error: %s\n", error);
			}
			assert(_handle != nullptr);
        }
        virtual ~XRDSOImpl()
        {
            if(_handle) dlclose(_handle);
        }
        
    public:
        void* GetProcAddress(const char* proc)
        {
			auto found = _tableNameToSymbol.find(std::string(proc));
			if (found != _tableNameToSymbol.end())
			{
				proc = found->second.c_str();
			}
            return dlsym(_handle, proc);
        }
        
        void ListDllFunctions(std::vector<std::string>& listOfFunctionNames, SymbolOption& symbolOption)
        {
			char const* found = nullptr;
			uint32_t foundImageIndex = 0;
			uint32_t imageCount = _dyld_image_count();
			for(uint32_t& i = foundImageIndex; i < imageCount; ++i)
			{
				char const* imageName = _dyld_get_image_name(i);
				
				found = strstr(imageName, _openName);
				if (found != nullptr)
					break;
			}
			
			assert(found != nullptr);
			
			mach_header const* header = _dyld_get_image_header(foundImageIndex);
			intptr_t vmaddr_slide = _dyld_get_image_vmaddr_slide(foundImageIndex);
			findXrSymbols(header, vmaddr_slide, listOfFunctionNames, _tableNameToSymbol);
        }
		
		/*
		 static bool
		 ptr_is_in_exe(const void *ptr, const struct mach_header *& header, intptr_t& offset, uintptr_t& vmaddr, std::string& image_name)
		 {
			 uint32_t i, count = _dyld_image_count();

			 for (i = 0; i < count; i++) {
				 header = _dyld_get_image_header(i);
				 offset = _dyld_get_image_vmaddr_slide(i);

				 uint32_t j = 0;
				 struct load_command* cmd = (struct load_command*)((char *)header + sizeof(struct mach_header));
				 if(header->magic == MH_MAGIC_64)
					 cmd = (struct load_command*)((char *)header + sizeof(struct mach_header_64));

				 while (j < header->ncmds) {
					 if (cmd->cmd == LC_SEGMENT) {
						 struct segment_command* seg = (struct segment_command*)cmd;
						 if (((intptr_t)ptr >= (seg->vmaddr + offset)) && ((intptr_t)ptr < (seg->vmaddr + offset + seg->vmsize))) {
							 vmaddr = seg->vmaddr;
							 image_name = _dyld_get_image_name(i);
							 return true;
						 }
					 }
					 if (cmd->cmd == LC_SEGMENT_64) {
						 struct segment_command_64* seg = (struct segment_command_64*)cmd;
						 if (((uintptr_t)ptr >= (seg->vmaddr + offset)) && ((uintptr_t)ptr < (seg->vmaddr + offset + seg->vmsize))) {
							 vmaddr = seg->vmaddr;
							 image_name = _dyld_get_image_name(i);
							 return true;
						 }
					 }

					 j++;
					 cmd = (struct load_command*)((char*)cmd + cmd->cmdsize);
				 }
			 }

			 return false;
		 }
		 */
		
	private:
		// Ref. https://www.apriorit.com/dev-blog/225-dynamic-linking-mach-o
		static void findXrSymbols(mach_header const* header, intptr_t vmaddr_slide,
								  std::vector<std::string>& listOfFunctionNames,
								  std::unordered_map<std::string, std::string>& tableNameToSymbol);
    };
}
