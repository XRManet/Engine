#include "stdafx.h"

#include <XREntrypoint/MainApplication.h>
#include <XRFrameworkBase/Thread.h>
#include <XRFrameworkBase/Window.h>

#include <XRFrameworkBase/XRSystemLinker.hpp>
#include <XRFrameworkBase/XRSourceBuildSystem.h>
#include <XRFrameworkBase/XRRenderEngine.h>

#include "XRFrameProcessor.h"
#include "XRSceneManager.h"
#include "XRRenderEnginePlatformBinder.hpp"

namespace xr
{
	MainApplication::MainApplication(PlatformType platformType)
		: Application("XRManet", platformType, bindThreadExecution(&MainApplication::main, this))
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
			
			return createWindow(fetcher.get(), windowDescription);
		} ();

		XRSourceBuildSystem* shaderBuildSystem = xrGetShaderBuildSystem();

		auto renderEngine = std::unique_ptr<XRRenderEngine>(xrCreateRenderEngine(this));

		XRFrameProcessor frameProcessor;
		frameProcessor.Initialize();

		auto sceneManager = XRSceneManager::GetInstance();
		sceneManager->BindPrimaryScene("main");

		fetcher->processLoop([&, this]() {
			frameProcessor.UpdateFrame();
			});

		return 0;
	}
}