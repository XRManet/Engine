#pragma once

#include "Window.h"

namespace xr
{
	class WindowGLFW;
	class EventFetcherGLFW;

	class EventFetcherGLFW : public EventFetcher
	{
	public:
		EventFetcherGLFW(Application* application, Thread* ownerThread);
		virtual ~EventFetcherGLFW();

	public:
		virtual bool processEventQueue() override final;

	public:
		static void InputKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void InputMouse(GLFWwindow* window, int button, int action, int mods);
		static void PositionMouse(GLFWwindow* window, double xpos, double ypos);
		static void ScrollMouse(GLFWwindow* window, double xoffset, double yoffset);

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
		virtual ~WindowGLFW();

		void commonConstructor();

	public:
		using Window::setSwapchain;
		using Window::setWindowSize;

	public:
		GLFWwindow* getGlfwWindow() const { return _window; }

	private:
		GLFWwindow* _window;
	};

} // namespace xr