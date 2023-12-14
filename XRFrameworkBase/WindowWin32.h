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
		EventFetcherWin32(Application* application, Thread* ownerThread);
		virtual ~EventFetcherWin32();

	public:
		virtual bool processEventQueue() override final;
	};

	class WindowWin32 : public Window
	{
	public:
		WindowWin32(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription);
		virtual ~WindowWin32();

		void commonConstructor();

	public:
		using Window::setSwapchain;
		using Window::setWindowSize;

	public:
		void* getPlatformNativeHandle() const override;
		HWND getHwnd() const { return _hWnd; }

	private:
		HWND _hWnd = NULL;
	};

} // namespace xr