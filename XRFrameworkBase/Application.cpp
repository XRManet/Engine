#include "stdafx.h"

#include <XRFrameworkBase/Application.h>
#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/Thread.h>
#include <XRFrameworkBase/XRGeometry.h>

#include "Profiler.h"

#include "Window.h"

#include <queue>

namespace xr
{

	Application::Application(PlatformType platformType)
		: Application(platformType, bindThreadExecution(&Application::defaultMainThreadRun, this))
	{
	}

	Application::Application(PlatformType platformType, ThreadExecution threadExecution)
		: _platformType(platformType)
	{
		_applicationPlatform = std::move(createApplicationPlatform(this, platformType));
		_mainThread = Thread::bindThreadFromCurrent("Main Thread", threadExecution);
	}

	Application::~Application()
	{
	}

	void Application::runApplication(Application* app, int argc, char** argv)
	{
		app->runMainThread();
		app->waitForAllThreads();
	}

	void Application::runMainThread()
	{
		_mainThread->execute();
	}

} // namespace xr

namespace xr
{
	void Application::addThread(ApplicationChild* child)
	{
		_allThreads.push_back(static_cast<Thread*>(child));
	}
	void Application::removeThread(ApplicationChild* child)
	{
		Thread* thread = static_cast<Thread*>(child);
		for (int i = 0; i < _allThreads.size(); ++i)
		{
			if (_allThreads[i] == thread)
			{
				_allThreads.erase(_allThreads.begin() + i);
				break;
			}
		}
	}

	void Application::addEventFetcher(ApplicationChild* child)
	{
	}
	void Application::removeEventFetcher(ApplicationChild* child)
	{
	}

	void Application::addWindow(ApplicationChild* child)
	{
	}
	void Application::removeWindow(ApplicationChild* child)
	{
	}
} // namespace xr

namespace xr
{
	uint32_t Application::defaultMainThreadRun()
	{
		auto fetcher = createEventFetcher(getMainThread());
		auto resizeKey = fetcher->eventResize.registerEvent(
			[](Window* window, bool minimized, bool maximized, uint32_t width, uint32_t height) {
				printf("Resize event occurred.");
			});

		auto keyboardKey = fetcher->eventKeyboard.registerEvent(
			[](Window* /*window*/, bool /*ctrl*/, bool /*alt*/, bool /*shift*/, bool /*currentPressed*/, bool /*previousPressed*/, uint32_t /*keyPressedCount*/, uint32_t /*key*/) {
				printf("Keyboard event occurred.");
			});
		auto mouseKey = fetcher->eventMouse.registerEvent(
			[](Window* /*window*/, const unsigned /*left*/, const unsigned /*middle*/, const unsigned /*right*/, const unsigned /*xButton*/, const int /*windowX*/, const int /*windowY*/) {
				printf("Mouse event occurred.");
			});

		auto window = [this, &fetcher]()
		{
			WindowDescription windowDescription = {};
			windowDescription._title = "XRDefault";
			windowDescription._width = 1920;
			windowDescription._height = 1080;
			auto window = createWindow(fetcher.get(), _mainThread.get(), windowDescription);

			return window;
		} ();

		Window* mainWindow = window.get();

		fetcher->processLoop([&, this]() {

			if (profiler::g_traceCurrent != profiler::g_traceEnabled)
			{
				profiler::g_traceEnabled = profiler::g_traceCurrent;
				if (profiler::g_traceEnabled)
					profiler::beginTrace("test.json");
				else
					profiler::endTrace();
			}

			profiler::scopeTracer("app", "app loop");

			// Update
			std::vector<render::Renderable*> renderablesForDirector;
			std::vector<render::Renderable*> renderablesForProjector;
			{
				profiler::scopeTracer("app", "update");
			}

			// Render
			{
				profiler::scopeTracer("app", "render");

				// Scene rendering
				{
					profiler::scopeTracer("main", "Renderer::Render");

					glm::ivec2 renderTargetSize = {
						mainWindow->getWindowDescription()._width,
						mainWindow->getWindowDescription()._height
					};
				}
			}
			});

		return 0;
	}

} // namespace xr