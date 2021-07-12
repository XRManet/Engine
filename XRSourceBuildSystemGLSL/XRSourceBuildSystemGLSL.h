// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the XRSOURCEBUILDSYSTEMGLSL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// XRSOURCEBUILDSYSTEMGLSL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XRSOURCEBUILDSYSTEMGLSL_EXPORTS
#define XRSourceBuildSystemAPI __declspec(dllexport)
#else
#define XRSourceBuildSystemAPI __declspec(dllimport)
#endif

#include <stdint.h>

#include <XRFrameworkBase/XRSourceBuildSystem.h>

// This class is exported from the dll
class XRSourceBuildSystemAPI XRCompilerGLSL : public XRCompiler
{
public:
	XRCompilerGLSL(XRBuildSystemAvailability availability);

public:
	void LoadSourceFiles();

public:
	XRCompiledObject* Compile(uint32_t numSourceFiles, const char** sourceFiles) override final;
	XRCompiledObject* Load(const char* objectFile) override final;

public:
	XRExecutable* BuildExecutable() override final;
	XRLinkable* BuildLibrary() override final;
};

extern XRSourceBuildSystemAPI int nXRSourceBuildSystemGLSL;

XRSourceBuildSystemAPI int fnXRSourceBuildSystemGLSL(void);
