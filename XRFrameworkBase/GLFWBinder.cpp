#include "stdafx.h"
#include <XRFrameworkBase/XRPlatform.h>

#if XR_PLATFORM == XR_PLATFORM_WINDOWS
#pragma comment(lib, "glfw3dll.lib")
#else // if XR_PLATFORM == XR_PLATFORM_OSX
#error "Not defined..."
#endif