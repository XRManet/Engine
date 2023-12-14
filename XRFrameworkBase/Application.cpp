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

	Application::Application(std::string&& name, PlatformType platformType)
		: Application(std::move(name), platformType, bindThreadExecution(&Application::defaultMainThreadRun, this))
	{
	}

	Application::Application(std::string&& name, PlatformType platformType, ThreadExecution threadExecution)
		: _platformType(platformType)
		, _name(move(name))
	{
		_applicationPlatform = std::move(createApplicationPlatform(this, platformType));
		_mainThread = Thread::bindThreadFromCurrent(this, "Main Thread", threadExecution);
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

	void Application::waitForAllThreads()
	{
		_applicationPlatform->waitForAllThreads();
	}

	const std::string&			Application::getName() const { return _name; }
	Thread*						Application::getMainThread() { return _mainThread.get(); }
	const std::vector<Thread*>&	Application::getAllThreads() const { return _allThreads; }
	ApplicationPlatform*		Application::getApplicationPlatform() const { return _applicationPlatform.get(); }

} // namespace xr

namespace xr
{
	std::unique_ptr<Thread> Application::createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		return _applicationPlatform->createThread(threadName, launchImmediatly, threadExecution);
	}

	std::unique_ptr<EventFetcher> Application::createEventFetcher(Thread* ownerThread)
	{
		return _applicationPlatform->createEventFetcher(ownerThread);
	}

	std::unique_ptr<Window> Application::createWindow(EventFetcher* eventFetcher, WindowDescription& windowDescription)
	{
		return _applicationPlatform->createWindow(eventFetcher, windowDescription);
	}

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

	void Application::addRenderEngine(ApplicationChild* child)
	{
	}
	void Application::removeRenderEngine(ApplicationChild* child)
	{
	}
} // namespace xr

namespace xr
{
	uint32_t Application::defaultMainThreadRun()
	{
		auto fetcher = createEventFetcher(getMainThread());
		auto resizeKey = fetcher->_eventResize.registerEvent(
			[](Window* window, bool minimized, bool maximized, uint32_t width, uint32_t height) {
				printf("Resize event occurred.");
			});

		auto keyboardKey = fetcher->_eventKeyboard.registerEvent(
			[](Window* /*window*/, bool /*ctrl*/, bool /*alt*/, bool /*shift*/, bool /*currentPressed*/, bool /*previousPressed*/, uint32_t /*keyPressedCount*/, uint32_t /*key*/) {
				printf("Keyboard event occurred.");
			});
		auto mouseKey = fetcher->_eventMouse.registerEvent(
			[](Window* /*window*/, const unsigned /*left*/, const unsigned /*middle*/, const unsigned /*right*/, const unsigned /*xButton*/, const int /*windowX*/, const int /*windowY*/) {
				printf("Mouse event occurred.");
			});

		auto mainWindow = [this, &fetcher]()
		{
			WindowDescription windowDescription = {};
			windowDescription._title = "XRDefault";
			windowDescription._width = 1920;
			windowDescription._height = 1080;
			auto window = createWindow(fetcher.get(), windowDescription);

			return window;
		} ();

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
			}
		);

		return 0;
	}

} // namespace xr