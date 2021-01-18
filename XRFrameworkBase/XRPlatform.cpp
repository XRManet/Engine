//
//  XRPlatform.cpp
//  Engine
//
//  Created by Jiman Jeong on 21/04/2019.
//  Copyright © 2019 XRManet. All rights reserved.
//

#include "stdafx.h"

#include "XRPlatform.h"

#if XR_PLATFORM == XR_PLATFORM_WINDOWS
#include "XRPlatformWindows.hpp"
static const XRPlatform::Names XRCurrentPlatformName = XRPlatform::Windows;
#elif XR_PLATFORM == XR_PLATFORM_OSX
#include "XRPlatformOSX.hpp"
static const XRPlatform::Names XRCurrentPlatformName = XRPlatform::OSX;
#endif

namespace XRPlatform
{
	template<typename Type>
	struct SimpleReferenceCounter
	{
		Type _var;
		uint32_t _refCounter = 1;

		uint32_t countUp()		{ return ++_refCounter; }
		uint32_t countDown()	{ return --_refCounter; }
	};
	static std::unordered_map<std::string, SimpleReferenceCounter<XRDSO*>> g_loadedDsoList;

	XRDSO* LoadDSO(char const* dso_name)
	{
		XRDSO* ret = nullptr;
		ret = new XRDSOImpl<XRCurrentPlatformName>(dso_name);
		return ret;

		//auto found = g_loadedDsoList.find(dso_name);
		//if (found == g_loadedDsoList.end())
		//{
		//	ret = new XRDSOImpl<XRCurrentPlatformName>(dso_name);
		//	g_loadedDsoList.insert({ dso_name, {ret, } });
		//}
		//else
		//{
		//	ret = found->second._var;
		//	found->second.countUp();
		//}
		//return ret;
	}

	void UnloadDSO(XRDSO* dso)
	{
		if (dso)
		{
			delete dso;

			//std::unordered_map<std::string, SimpleReferenceCounter<XRDSO*>>::iterator found = g_loadedDsoList.end();
			//for (auto it = g_loadedDsoList.begin(); it != g_loadedDsoList.end(); ++it)
			//{
			//	if (it->second._var == dso)
			//	{
			//		it->second.countDown();
			//		found = it;
			//		break;
			//	}
			//}

			//if (found != g_loadedDsoList.end() && found->second._refCounter == 0)
			//{
			//	delete dso;
			//}
		}
	}

	void* GetProcAddress(XRDSO* dso, char const* proc)
	{
		auto platform_dso = static_cast<XRDSOImpl<XRCurrentPlatformName>*>(dso);
		return platform_dso->GetProcAddress(proc);
	}

	void ListDLLFunctions(XRDSO* dso, std::vector<std::string>& listOfFunctionNames, SymbolOption& symOption)
	{
		auto platform_dso = static_cast<XRDSOImpl<XRCurrentPlatformName>*>(dso);
		return platform_dso->ListDllFunctions(listOfFunctionNames, symOption);
	}
}
