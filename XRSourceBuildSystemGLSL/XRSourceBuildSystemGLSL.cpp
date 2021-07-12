// XRSourceBuildSystemGLSL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "XRSourceBuildSystemGLSL.h"


// This is an example of an exported variable
XRSourceBuildSystemAPI int nXRSourceBuildSystemGLSL=0;

// This is an example of an exported function.
XRSourceBuildSystemAPI int fnXRSourceBuildSystemGLSL(void)
{
    return 0;
}

static XRBuildSystemAvailability g_buildSystemAvailability {
	.runtimeCompile = true,
	.offlineCompile = false,
    .compatibleObject = false,
};

// This is the constructor of a class that has been exported.
XRCompilerGLSL::XRCompilerGLSL(XRBuildSystemAvailability availability)
    : XRCompiler(availability)
{

	static int isProgramInterfaceQueriable = glfwExtensionSupported("GL_ARB_program_interface_query");
	static bool doQueryProgramInterface = (isProgramInterfaceQueriable == GLFW_TRUE)
		|| (glfwGetProcAddress("glGetProgramInterfaceiv") != nullptr);

	static int isUniformBufferObjectQueriable = glfwExtensionSupported("GL_ARB_uniform_buffer_object");
	static bool doQueryUniformBuffer = (isUniformBufferObjectQueriable == GLFW_TRUE)
		|| (glfwGetProcAddress("glGetActiveUniformsiv") != nullptr);


    return;
}

void XRCompilerGLSL::LoadSourceFiles()
{

}

XRCompiledObject* XRCompilerGLSL::Compile(uint32_t numSourceFiles, const char** sourceFiles)
{
	return nullptr;
}

XRCompiledObject* XRCompilerGLSL::Load(const char* objectFile)
{
	return nullptr;
}

XRExecutable* XRCompilerGLSL::BuildExecutable()
{
	return nullptr;
}

XRLinkable* XRCompilerGLSL::BuildLibrary()
{
	return nullptr;
}