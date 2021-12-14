#include "stdafx.h"

#include "XRSourceBuildSystem.h"

XRSourceBuildSystem* xrGetShaderBuildSystem()
{
	static XRSourceBuildSystem* defaultShaderBuildSystem = xrLoadShaderBuildSystem();
	return defaultShaderBuildSystem;
}

XRSourceBuildSystem::XRSourceBuildSystem(XRCompiler* compiler)
	: _compiler(compiler)
{
}