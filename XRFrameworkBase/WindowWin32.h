#pragma once

#include "Window.h"

namespace xr
{
	class WindowWin32;
	class EventFetcherWin32;
	class InputManager;

	struct WindowBindingInfo
	{
		WindowBindingInfo() = default;

		WindowWin32* _window;
		EventFetcherWin32* _eventFetcher;

	};

	class EventFetcherWin32 : public EventFetcher
	{
	public:
		EventFetcherWin32(Thread* ownerThread);
		virtual ~EventFetcherWin32();

	public:
		virtual void processEventQueue(std::function<void()> loopWorks) override final;
	};

	class WindowWin32 : public Window
	{
	public:
		WindowWin32(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription);
		WindowWin32(Application* application, EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription);
		virtual ~WindowWin32();

		void constructorCommon();

	public:
		using Window::setSwapchain;
		using Window::setWindowSize;

	public:
		HWND getHwnd() const { return _hWnd; }

	private:
		HWND _hWnd;
	};

} // namespace xr