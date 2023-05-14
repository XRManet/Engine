#pragma once

#include "Window.h"

namespace xr
{
	class WindowGLFW;
	class EventFetcherGLFW;

	class EventFetcherGLFW : public EventFetcher
	{
	public:
		EventFetcherGLFW(Thread* ownerThread);
		virtual ~EventFetcherGLFW();

	public:
		virtual bool processEventQueue() override final;

	private:
		struct {
			std::vector<GLFWwindow*>	_glfw;
			std::vector<WindowGLFW*>	_xr;
		}								_registeredWindows;
	};

	class WindowGLFW : public Window
	{
	public:
		WindowGLFW(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription);
		WindowGLFW(Application* application, EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription);
		virtual ~WindowGLFW();

		void constructorCommon(EventFetcher* eventFetcher);

	public:
		using Window::setSwapchain;
		using Window::setWindowSize;

	public:
		GLFWwindow* getGlfwWindow() const { return _window; }

	private:
		GLFWwindow* _window;
	};

} // namespace xr