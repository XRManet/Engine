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
		auto fetcher = createEventFetcher(getMainThread());

		auto window = [this, &fetcher]()
		{
			WindowDescription windowDescription = {};
			windowDescription._title = "Main";
			windowDescription._width = 1920;
			windowDescription._height = 1080;
			auto window = createWindow(fetcher.get(), getMainThread(), windowDescription);

			return window;
		} ();

		fetcher->processLoop([&, this]() {
			});

		return 0;
	}
}