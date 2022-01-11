//
//  XRPlatformOSX.hpp
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#include "XRPlatform.h"
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
        
        XRDSOImpl(const char* dso_name)
        {
            sprintf(_openName, "lib%s.dylib", dso_name);
			
			//_dyld_register_func_for_add_image(callback);
            _handle = dlopen(_openName, RTLD_LOCAL);
            if(_handle == nullptr)
            {
                const char* fallback_name = "libXRRenderEngineGL.dylib";
				sprintf(_openName, "%s", fallback_name);
                _handle = dlopen(_openName, RTLD_LOCAL);
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
			findXrSymbols(header, vmaddr_slide, listOfFunctionNames);
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
		static void findXrSymbols(mach_header const* header, intptr_t vmaddr_slide, std::vector<std::string>& listOfFunctionNames)
		{
			load_command const* command = reinterpret_cast<load_command const*>(header + 1);
			
			intptr_t baseAddress = (intptr_t)header;
			
			mach_header_64 const* header64 = nullptr;
			if (header->magic == MH_MAGIC_64)
			{
				header64 = reinterpret_cast<mach_header_64 const*>(header);
				command = reinterpret_cast<load_command const*>(header64 + 1);
			}
			
			Dl_info dlinfo;
			dladdr(header, &dlinfo);

			const char* strtabAddress = 0;
			struct nlist_64 const* symtabAddress = 0;
			struct nlist_32 const* symtabAddress32 = 0;
			for(uint32_t i = 0; i < header->ncmds; ++i)
			{
				printf("CMD: %x ", command->cmd);
				switch(command->cmd)
				{
//				case LC_SEGMENT: {
//					segment_command const* seg = reinterpret_cast<segment_command const*>(command);
//					break;
//				}
//				case LC_SEGMENT_64: {
//					segment_command_64 const* seg = reinterpret_cast<segment_command_64 const*>(command);
//					break;
//				}
				case LC_IDENT: {
					// Note: Enter here if header is for executable.
					ident_command const* id = reinterpret_cast<ident_command const*>(command);
					char const* name = reinterpret_cast<char const*>(id + 1);
					printf("[%d]: LC_IDENT, name: %s\n", i, name);
					break;
				}
				case LC_ID_DYLIB: {
					// Note: Enter here if header is for dylib.
					dylib_command const* dylibId = reinterpret_cast<dylib_command const*>(command);
					char const* name = reinterpret_cast<char const*>(command) + dylibId->dylib.name.offset;
					printf("[%d]: LC_ID_DYLIB, name: %s\n", i, name);
					
					baseAddress = vmaddr_slide;
					break;
				}
				case LC_LOAD_DYLIB: {
					dylib_command const* dylibId = reinterpret_cast<dylib_command const*>(command);
					char const* name = reinterpret_cast<char const*>(command) + dylibId->dylib.name.offset;
					printf("[%d]: LC_LOAD_DYLIB, name: %s\n", i, name);
					break;
				}
				case LC_DYLD_INFO:
				case LC_DYLD_INFO_ONLY: {
					dyld_info_command const* dyldInfo = reinterpret_cast<dyld_info_command const*>(command);
					printf("[%d]: LC_DYLD_INFO%s\n", i, (command->cmd & LC_REQ_DYLD) ? "_ONLY" : "");
					printf("\tRebase %x, %x\n", dyldInfo->rebase_off, dyldInfo->rebase_size);
					printf("\tBind   %x, %x\n", dyldInfo->bind_off, dyldInfo->bind_size);
					printf("\tWeak   %x, %x\n", dyldInfo->weak_bind_off, dyldInfo->weak_bind_size);
					printf("\tLazy   %x, %x\n", dyldInfo->lazy_bind_off, dyldInfo->lazy_bind_size);
					printf("\tExport %x, %x\n", dyldInfo->export_off, dyldInfo->export_size);
					printf("\t - header: %llx, vmaddr_slide: %llx base: %llx dlinfo.sname: %llx\n", uint64_t(header), uint64_t(vmaddr_slide), uint64_t(baseAddress), uint64_t(dlinfo.dli_sname));
					
					if (dyldInfo->rebase_off == 0)
					{
						//baseAddress = intptr_t(header);
					}
					break;
				}
				case LC_SYMTAB: {
					symtab_command const* symtab = reinterpret_cast<symtab_command const*>(command);
					strtabAddress = reinterpret_cast<char const*>((intptr_t)baseAddress + symtab->stroff);
					symtabAddress = reinterpret_cast<struct nlist_64 const*>((intptr_t)baseAddress + symtab->symoff);
					//symtabAddress32 = reinterpret_cast<struct nlist_32 const*>((intptr_t)baseAddress + symtab->symoff);
					
					//intptr_t address = baseAddress + ((vmaddr_slide == (intptr_t)baseAddress) ? 0 : vmaddr_slide);
					
					printf("[%d]: LC_SYMTAB\n", i);
					printf("\tString %x, %x\n", symtab->stroff, symtab->strsize);
					printf("\tSymbol %x, %x\n", symtab->symoff, symtab->nsyms);
					printf("\t - header: %llx, vmaddr_slide: %llx base: %llx dlinfo.sname: %llx\n", uint64_t(header), uint64_t(vmaddr_slide), uint64_t(baseAddress), uint64_t(dlinfo.dli_sname));
					
					uint32_t max10 = symtab->nsyms > 10 ? 10 : symtab->nsyms;
					for(uint32_t s = 0; s < max10; ++s)
					{
						printf("\t[%d]: %s\n", s, strtabAddress + symtabAddress[s].n_un.n_strx);
					}
					
					break;
				}
				case LC_DYSYMTAB: {
					dysymtab_command const* dysymtab = reinterpret_cast<dysymtab_command const*>(command);
					
					printf("[%d]: LC_DYSYMTAB\n", i);
					printf("\t - header: %llx, vmaddr_slide: %llx base: %llx dlinfo.sname: %llx\n", uint64_t(header), uint64_t(vmaddr_slide), uint64_t(baseAddress), uint64_t(dlinfo.dli_sname));
					
					const char* indirectAddress = ((const char*)header) + dysymtab->indirectsymoff;
					
					for(uint32_t i = dysymtab->iextdefsym; i < dysymtab->iextdefsym + dysymtab->nextdefsym; ++i)
					{
						//printf("\t[%d]: %s\n", i, strtabAddress + symtabAddress[i].n_un.n_strx);
						
						const char* foundSymbolName = strtabAddress + symtabAddress[i].n_un.n_strx;
						if (strncmp(foundSymbolName, "_xr", sizeof("_xr") - 1) == 0)
						{
							listOfFunctionNames.push_back(std::string(foundSymbolName + 1));
						}
					}
				}
				default: {
					break;
				}
				}
				
				command = reinterpret_cast<load_command const*>(reinterpret_cast<uint8_t const*>(command) + command->cmdsize);
			}
			
//			int j;
//			printf("name=%s (%d segments)\n", info->,
//				   info->dlpi_phnum);
//
//			for (j = 0; j < info->dlpi_phnum; j++)
//				printf("\t\t header %2d: address=%10p\n", j,
//					   (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr));
		}
    };
}
