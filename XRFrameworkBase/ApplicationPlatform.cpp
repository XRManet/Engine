#include "stdafx.h"

#include <XRFrameworkBase/XRPlatform.h>
#include <XRFrameworkBase/Application.h>

#include <XRFrameworkBase/ApplicationGLFW.h>
#if XR_PLATFORM == XR_PLATFORM_WINDOWS
#include <XRFrameworkBase/ApplicationWin32.h>
#endif

namespace xr
{


	std::unique_ptr<ApplicationPlatform> Application::createApplicationPlatform(Application* application, PlatformType platformType)
	{
		switch (platformType)
		{
		case PlatformType::GLFW:
			return std::unique_ptr<ApplicationPlatform>(new ApplicationGLFW(application));
		case PlatformType::Win32:
			#if XR_PLATFORM == XR_PLATFORM_WINDOWS
			return std::unique_ptr<ApplicationPlatform>(new ApplicationWin32(application));
			#else // XR_PLATFORM == XR_PLATFORM_WINDOWS
			break;
			#endif // XR_PLATFORM == XR_PLATFORM_WINDOWS
		}
		assert(false);
		return std::unique_ptr<ApplicationPlatform>();
	}
}