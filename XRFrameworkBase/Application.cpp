#include "stdafx.h"

#include <XRFrameworkBase/Application.h>
#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/Thread.h>

#include "Profiler.h"

#include "DeviceD3D11.h"
#include "Window.h"

#if defined(USE_IMGUI)
#include "ImguiBinder.h"
#endif // #if defined(USE_IMGUI)
import Render;

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
		_applicationPlatform = std::move(createApplicationPlatform(platformType));
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
#if defined(USE_IMGUI)
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
#endif // #if defined(USE_IMGUI)

		auto fetcher = createEventFetcher(_mainThread.get());
		auto resizeKey = fetcher->eventResize.registerEvent(
			[](Window* window, bool minimized, bool maximized, uint32_t width, uint32_t height) {
				render::Swapchain* swapchain = window->getSwapchain();
				if (swapchain != nullptr)
				{
					swapchain->resize();
				}
			});
		auto resizeKey2 = fetcher->eventResize.registerEvent(&render::Renderer::Resize);

		InputManager* inputManager = new InputManager();
		auto keyboardKey = fetcher->eventKeyboard.registerEvent(inputManager->GetKeyCallback());
		auto mouseKey = fetcher->eventMouse.registerEvent(inputManager->GetMouseCallback());

		auto imguiBinder = ImguiBinder::getImguiBinder(render::SupportingAPIs::D3D11);

		auto window = [this, &fetcher, &imguiBinder]()
		{
			WindowDescription windowDescription = {};
			windowDescription._title = "RealityProjector";
			windowDescription._width = 1920;
			windowDescription._height = 1080;
			auto window = createWindow(fetcher.get(), _mainThread.get(), windowDescription);

			return window;
		} ();

		Window* projectorWindow = window.get();

		fetcher->processEventQueue([&, this]() {

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

				// Projector Window
				{
					// Scene rendering
					{
						profiler::scopeTracer("main", "Renderer::Render");

						render::Renderer::SetGlobalRenderTarget(projectorWindow->getSwapchain()->getBackBufferView());

						glm::ivec2 renderTargetSize = {
							projectorWindow->getWindowDescription()._width,
							projectorWindow->getWindowDescription()._height
						};

						glm::vec4 clearColor = getClearColor();
						render::Renderer::ClearRenderTarget(clearColor);
						render::Renderer::ClearDepthStencil(1.0f, 0);

						if (_stage->GetEffectType() == 0)
						{
							render::Renderer::Render(projector->GetCamera(), renderTargetSize, renderablesForProjector);
						}

						auto actors = _stage->GetActors();

						if (_stage->GetEffectType() == 1)
						{
							render::Renderer::RenderCenterLight(projector->GetCamera(), renderTargetSize, actors);
						}

						if (_stage->GetEffectType() == 2)
						{
							render::Renderer::RenderPointCloud(projector->GetCamera(), renderTargetSize, _stage->GetPointCloudSize(), sensor->getLastPointCloudVertexBuffer(), sensor->getPointCount(), actors);
						}
					}


					// Start the Dear ImGui frame
					{
						profiler::scopeTracer("main", "ImGui prepare");
						imguiBinder->prepare(projectorWindow);
						ImGui::NewFrame();
					}

					{
						profiler::scopeTracer("main", "ImGui render");
						ImGui::Render();
						imguiBinder->render(render::Renderer::GetRenderTarget());
					}

					// After-UI rendering test
					{
						// render::Renderer::RenderLeftImage(glm::vec2(0, 0), glm::vec2(400, 300), sensor->getLastLeftImageAsTexture());
					}

					// Present
					{
						profiler::scopeTracer("main", "present");
						render::Renderer::Present();
					}

				}
			}
			});

		return 0;
	}

} // namespace xr