#include "stdafx.h"

#include <XRFrameworkBase/GLFWBinder.h>

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
		// Todo(jiman): Move swap buffers to swapchain object
		/* Swap front and back buffers */
		//glfwSwapBuffers(_window);

		/* Poll for and process events */
		glfwPollEvents();

		auto& boundWindows = getBoundWindows();
		for (Window* window : boundWindows)
		{
			auto windowGLFW = static_cast<WindowGLFW*>(window);
			auto glfwWindow = windowGLFW->getGlfwWindow();
			if (glfwWindowShouldClose(glfwWindow) == false)
			{
				glfwDestroyWindow(glfwWindow);
			}

			// Todo(jiman): unbind the window from this event fetcher
		}

		return true;
	}

	void EventFetcherGLFW::InputKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto windowGLFW = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
		auto eventFetcherGLFW = static_cast<EventFetcherGLFW*>(windowGLFW->getEventFetcher());

		const bool controlKeyPressed = (mods & GLFW_MOD_CONTROL) != 0;
		const bool altKeyPressed = (mods & GLFW_MOD_ALT) != 0;
		const bool shiftKeyPressed = (mods & GLFW_MOD_SHIFT) != 0;

		const bool prevKeyPressed = (action == GLFW_REPEAT);
		const bool curKeyPressed = (action != GLFW_RELEASE);

		uint32_t pressedCount = 1;
		eventFetcherGLFW->_eventKeyboard(windowGLFW, controlKeyPressed, altKeyPressed, shiftKeyPressed, curKeyPressed, prevKeyPressed, pressedCount, key);
	}

	void EventFetcherGLFW::InputMouse(GLFWwindow* window, int button, int action, int mods)
	{
		auto windowGLFW = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
		auto eventFetcherGLFW = static_cast<EventFetcherGLFW*>(windowGLFW->getEventFetcher());

		const unsigned left = (button == GLFW_MOUSE_BUTTON_LEFT) ? ((action == GLFW_PRESS) ? 0x1 : (action == GLFW_RELEASE) ? 0x4 : 0) : 0;
		const unsigned middle = (button == GLFW_MOUSE_BUTTON_MIDDLE) ? ((action == GLFW_PRESS) ? 0x1 : (action == GLFW_RELEASE) ? 0x4 : 0) : 0;
		const unsigned right = (button == GLFW_MOUSE_BUTTON_RIGHT) ? ((action == GLFW_PRESS) ? 0x1 : (action == GLFW_RELEASE) ? 0x4 : 0) : 0;
		const unsigned x1 = (button == GLFW_MOUSE_BUTTON_4) ? ((action == GLFW_PRESS) ? 0x1 : (action == GLFW_RELEASE) ? 0x4 : 0) : 0;
		const unsigned x2 = (button == GLFW_MOUSE_BUTTON_5) ? ((action == GLFW_PRESS) ? 0x10 : (action == GLFW_RELEASE) ? 0x40 : 0x00) : 0x00;

		eventFetcherGLFW->_eventMouse(windowGLFW, left, middle, right, x1 | x2, 0, 0);
	}

	void EventFetcherGLFW::PositionMouse(GLFWwindow* window, double xpos, double ypos)
	{
		auto windowGLFW = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
		auto eventFetcherGLFW = static_cast<EventFetcherGLFW*>(windowGLFW->getEventFetcher());

		int32_t windowSizeX = 0, windowSizeY = 0;
		glfwGetWindowSize(window, &windowSizeX, &windowSizeY);

		eventFetcherGLFW->_eventMouse(windowGLFW, 0, 0, 0, 0, static_cast<int>(xpos), static_cast<int>(windowSizeY - ypos));
	}

	void EventFetcherGLFW::ScrollMouse(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto windowGLFW = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
		auto eventFetcherGLFW = static_cast<EventFetcherGLFW*>(windowGLFW->getEventFetcher());

		eventFetcherGLFW->_eventMouse(windowGLFW, 0, 0, 0, 0, 0, 0);
	}
} // namespace xr


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
			_window = glfwCreateWindow(int(description._width), int(description._height), description._title.c_str(), NULL, NULL);
			glfwSetWindowUserPointer(_window, this);
		}

		// Default combination of event callbacks
		{
			glfwSetKeyCallback(_window, EventFetcherGLFW::InputKeyboard);
			glfwSetMouseButtonCallback(_window, EventFetcherGLFW::InputMouse);
			glfwSetCursorPosCallback(_window, EventFetcherGLFW::PositionMouse);
			glfwSetScrollCallback(_window, EventFetcherGLFW::ScrollMouse);
		}
	}

	WindowGLFW::~WindowGLFW()
	{
		// Todo(jiman): Check all possible paths for destroying window

		if (_window != nullptr)
		{
			int shouldClose = glfwWindowShouldClose(_window);
			assert(shouldClose && "A _window should be set to close.");

			glfwDestroyWindow(_window);
			_window = nullptr;
		}
	}

	void* WindowGLFW::getPlatformNativeHandle() const
	{
		return glfwGetWin32Window(_window);
	}

} // namespace xr