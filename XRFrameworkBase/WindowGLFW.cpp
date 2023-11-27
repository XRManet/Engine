#include "stdafx.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3dll.lib")

#include <XRFrameworkBase/WindowGLFW.h>
#include <XRFrameworkBase/ApplicationGLFW.h>

namespace xr
{
	EventFetcherGLFW::EventFetcherGLFW(Application* application, Thread* ownerThread)
		: EventFetcher(application, ownerThread)
	{

	}

	EventFetcherGLFW::~EventFetcherGLFW()
	{
	}

	bool EventFetcherGLFW::processEventQueue()
	{
		glfwPollEvents();

		auto boundWindows = getBoundWindows();
		for (Window* window : boundWindows)
		{
			auto windowGLFW = static_cast<WindowGLFW*>(window);
			auto glfwWindow = windowGLFW->getGlfwWindow();
			if (glfwWindowShouldClose(glfwWindow) == false)
				glfwDestroyWindow(glfwWindow);
		}

		return true;
	}
} // namespace xr

namespace xr
{
} // namespace xr


bool CreateDeviceD3D(HWND hWnd); // temp for imgui

namespace xr
{

	WindowGLFW::WindowGLFW(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription)
		: Window(application, eventFetcher, windowDescription)
	{
		commonConstructor();
	}

	void WindowGLFW::commonConstructor()
	{
		ApplicationGLFW* applicationGLFW = static_cast<ApplicationGLFW*>(getApplicationPlatform());
		EventFetcherGLFW* eventFetcherGLFW = static_cast<EventFetcherGLFW*>(getEventFetcher());
		const WindowDescription& description = getWindowDescription();

		wchar_t wcWindowTitle[256];
		{
			size_t newsize = description._title.length() + 1;

			size_t convertedChars = 0;
			errno_t result = mbstowcs_s(&convertedChars, wcWindowTitle, newsize, description._title.c_str(), 256 - 1);
		}
		
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

			_window = glfwCreateWindow(int(description._width), int(description._height), description._title.c_str(), NULL, NULL);
		}
	}

	WindowGLFW::~WindowGLFW()
	{
		if (_window != nullptr)
		{
			int shouldClose = glfwWindowShouldClose(_window);
			assert(shouldClose && "A _window should be set to close.");

			glfwDestroyWindow(_window);
			_window = nullptr;
		}
	}

} // namespace xr