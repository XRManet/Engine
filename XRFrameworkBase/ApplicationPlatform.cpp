#include "stdafx.h"

#include <XRFrameworkBase/XRPlatform.h>
#include <XRFrameworkBase/Application.h>

#include <XRFrameworkBase/ApplicationGLFW.h>
#if XR_PLATFORM == XR_PLATFORM_WINDOWS
#include <XRFrameworkBase/ApplicationWin32.h>
#endif

namespace xr
{


	std::unique_ptr<ApplicationPlatform> Application::createApplicationPlatform(PlatformType platformType)
	{
		switch (platformType)
		{
		case PlatformType::GLFW:
			return std::unique_ptr<ApplicationPlatform>(new ApplicationGLFW);
#if XR_PLATFORM == XR_PLATFORM_WINDOWS
		case PlatformType::Win32:
			return std::unique_ptr<ApplicationPlatform>(new ApplicationWin32);
#endif
		}
		assert(false);
		return std::make_unique<ApplicationPlatform>(nullptr);
	}
}