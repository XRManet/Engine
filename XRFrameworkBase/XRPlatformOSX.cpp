//
//  XRPlatformOSX.cpp
//  XRFrameworkBase
//
//  Created by Jiman Jeong on 2022/07/17.
//  Copyright © 2022 XRManet. All rights reserved.
//

#include <XRFrameworkBase/XRPlatformOSX.hpp>
#include <cxxabi.h>

//#define USE_DEBUGGING_LOG

#if defined(USE_DEBUGGING_LOG)
#define FIND_SYMBOL_LOG(str, ...) printf(str, ##__VA_ARGS__)
#else // #if defined(USE_DEBUGGING_LOG)
#define FIND_SYMBOL_LOG(str, ...)
#endif // #if defined(USE_DEBUGGING_LOG)


namespace XRPlatform
{
void XRDSOImpl<OSX>::findXrSymbols(mach_header const* header, intptr_t vmaddr_slide, std::vector<std::string>& listOfFunctionNames, std::unordered_map<std::string, std::string>& tableNameToSymbol)
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
		FIND_SYMBOL_LOG("CMD: %x ", command->cmd);
		switch(command->cmd)
		{
#if defined(USE_DEBUGGING_LOG)
//		case LC_SEGMENT: {
//			segment_command const* seg = reinterpret_cast<segment_command const*>(command);
//			break;
//		}
//		case LC_SEGMENT_64: {
//			segment_command_64 const* seg = reinterpret_cast<segment_command_64 const*>(command);
//			break;
//		}
		case LC_IDENT: {
			// Note: Enter here if header is for executable.
			ident_command const* id = reinterpret_cast<ident_command const*>(command);
			char const* name = reinterpret_cast<char const*>(id + 1);
			FIND_SYMBOL_LOG("[%d]: LC_IDENT, name: %s\n", i, name);
			break;
		}
		case LC_ID_DYLIB: {
			// Note: Enter here if header is for dylib.
			dylib_command const* dylibId = reinterpret_cast<dylib_command const*>(command);
			char const* name = reinterpret_cast<char const*>(command) + dylibId->dylib.name.offset;
			FIND_SYMBOL_LOG("[%d]: LC_ID_DYLIB, name: %s\n", i, name);
			
			baseAddress = vmaddr_slide;
			break;
		}
		case LC_LOAD_DYLIB: {
			dylib_command const* dylibId = reinterpret_cast<dylib_command const*>(command);
			char const* name = reinterpret_cast<char const*>(command) + dylibId->dylib.name.offset;
			FIND_SYMBOL_LOG("[%d]: LC_LOAD_DYLIB, name: %s\n", i, name);
			break;
		}
		case LC_DYLD_INFO:
		case LC_DYLD_INFO_ONLY: {
			dyld_info_command const* dyldInfo = reinterpret_cast<dyld_info_command const*>(command);
			FIND_SYMBOL_LOG("[%d]: LC_DYLD_INFO%s\n", i, (command->cmd & LC_REQ_DYLD) ? "_ONLY" : "");
			FIND_SYMBOL_LOG("\tRebase %x, %x\n", dyldInfo->rebase_off, dyldInfo->rebase_size);
			FIND_SYMBOL_LOG("\tBind   %x, %x\n", dyldInfo->bind_off, dyldInfo->bind_size);
			FIND_SYMBOL_LOG("\tWeak   %x, %x\n", dyldInfo->weak_bind_off, dyldInfo->weak_bind_size);
			FIND_SYMBOL_LOG("\tLazy   %x, %x\n", dyldInfo->lazy_bind_off, dyldInfo->lazy_bind_size);
			FIND_SYMBOL_LOG("\tExport %x, %x\n", dyldInfo->export_off, dyldInfo->export_size);
			FIND_SYMBOL_LOG("\t - header: %llx, vmaddr_slide: %llx base: %llx dlinfo.sname: %llx\n", uint64_t(header), uint64_t(vmaddr_slide), uint64_t(baseAddress), uint64_t(dlinfo.dli_sname));
			
			if (dyldInfo->rebase_off == 0)
			{
				//baseAddress = intptr_t(header);
			}
			break;
		}
#endif
		case LC_SYMTAB: {
			symtab_command const* symtab = reinterpret_cast<symtab_command const*>(command);
			strtabAddress = reinterpret_cast<char const*>((intptr_t)baseAddress + symtab->stroff);
			symtabAddress = reinterpret_cast<struct nlist_64 const*>((intptr_t)baseAddress + symtab->symoff);
			symtabAddress32 = reinterpret_cast<struct nlist_32 const*>((intptr_t)baseAddress + symtab->symoff);
			
			//intptr_t address = baseAddress + ((vmaddr_slide == (intptr_t)baseAddress) ? 0 : vmaddr_slide);
			
		#if defined(USE_DEBUGGING_LOG)
			FIND_SYMBOL_LOG("[%d]: LC_SYMTAB\n", i);
			FIND_SYMBOL_LOG("\tString %x, %x\n", symtab->stroff, symtab->strsize);
			FIND_SYMBOL_LOG("\tSymbol %x, %x\n", symtab->symoff, symtab->nsyms);
			FIND_SYMBOL_LOG("\t - header: %llx, vmaddr_slide: %llx base: %llx dlinfo.sname: %llx\n", uint64_t(header), uint64_t(vmaddr_slide), uint64_t(baseAddress), uint64_t(dlinfo.dli_sname));
			
			uint32_t max10 = symtab->nsyms > 10 ? 10 : symtab->nsyms;
			uint32_t s = 0, v = 0;
			for(; s < max10 && v < symtab->nsyms;)
			{
				if (symtabAddress[v++].n_un.n_strx >= symtab->strsize)
				{
					FIND_SYMBOL_LOG("\t[Err]\n");
					continue;
				}
				FIND_SYMBOL_LOG("\t[%d]: %s\n", s, strtabAddress + symtabAddress[v-1].n_un.n_strx);
				++s;
			}
		#endif // #if defined(USE_DEBUGGING_LOG)
			
			break;
		}
		case LC_DYSYMTAB: {
			dysymtab_command const* dysymtab = reinterpret_cast<dysymtab_command const*>(command);
			
			FIND_SYMBOL_LOG("[%d]: LC_DYSYMTAB\n", i);
			FIND_SYMBOL_LOG("\t - header: %llx, vmaddr_slide: %llx base: %llx dlinfo.sname: %llx\n", uint64_t(header), uint64_t(vmaddr_slide), uint64_t(baseAddress), uint64_t(dlinfo.dli_sname));
			
			const char* indirectAddress = ((const char*)header) + dysymtab->indirectsymoff;
			
			for(uint32_t i = dysymtab->iextdefsym; i < dysymtab->iextdefsym + dysymtab->nextdefsym; ++i)
			{
				//FIND_SYMBOL_LOG("\t[%d]: %s\n", i, strtabAddress + symtabAddress[i].n_un.n_strx);
				
				const char* foundSymbolName = strtabAddress + symtabAddress[i].n_un.n_strx;
				bool found = ( strncmp(foundSymbolName, "_xr", sizeof("_xr") - 1) == 0 )
							|| (strstr(foundSymbolName, "WorkPassAutoGenerator") != nullptr);
				
				if (found == false)
					continue;
				
				size_t length = 0;
				int status = 0;

				char* demangledName = abi::__cxa_demangle(foundSymbolName, 0, &length, &status);
				if (demangledName != nullptr)
				{
					listOfFunctionNames.push_back(std::string(demangledName));
					free(demangledName);
					
					auto result = tableNameToSymbol.insert( {listOfFunctionNames.back(), std::string(foundSymbolName + 1) });
					assert(result.second == true);
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
//			FIND_SYMBOL_LOG("name=%s (%d segments)\n", info->,
//				   info->dlpi_phnum);
//
//			for (j = 0; j < info->dlpi_phnum; j++)
//				FIND_SYMBOL_LOG("\t\t header %2d: address=%10p\n", j,
//					   (void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr));
}

} // XRPlatform
