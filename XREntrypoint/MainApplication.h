#pragma once

#include <XRFrameworkBase/Application.h>

namespace xr
{
	class MainApplication : public Application
	{
	public:
		MainApplication(PlatformType platformType);

	public:
		uint32_t main();
	};
}