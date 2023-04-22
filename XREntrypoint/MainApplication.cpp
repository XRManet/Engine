#include "stdafx.h"

#include <XREntrypoint/MainApplication.h>
#include <XRFrameworkBase/Thread.h>
#include <XRFrameworkBase/Window.h>

namespace xr
{
	MainApplication::MainApplication(PlatformType platformType)
		: Application(platformType, bindThreadExecution(&MainApplication::main, this))
	{

	}

	uint32_t MainApplication::main()
	{
		return 0;
	}
}