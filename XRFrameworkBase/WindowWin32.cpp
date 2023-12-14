#include "stdafx.h"
#include <XRFrameworkBase/WindowWin32.h>
#include <XRFrameworkBase/ApplicationWin32.h>

#if defined(USE_IMGUI)
#include "ImguiBinder.h"
#endif // #if defined(USE_IMGUI)

#include <Windowsx.h>

namespace xr
{

	class WindowEventExecutor : public WindowBindingInfo
	{
	public:
		WindowEventExecutor(HWND hwnd);

	public:
		void		bind(HWND hwnd);
		bool		isManaged() const { return _isManagedWindow; }
		bool		isMaximized() const;

	public:
		void		adjustMaximizedClientRect(RECT& rect);
		LRESULT		hitTest(const POINT& cursor) const;

	private:
		bool		_isManagedWindow = false;
	};

	WindowEventExecutor::WindowEventExecutor(HWND hwnd)
	{
		_isManagedWindow = (reinterpret_cast<HWND>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)) == hwnd);

		if (_isManagedWindow)
		{
			_window = (WindowWin32*)(::GetWindowLongPtr(hwnd, offsetof(WindowBindingInfo, _window)));
			_eventFetcher = (EventFetcherWin32*)(::GetWindowLongPtr(hwnd, offsetof(WindowBindingInfo, _eventFetcher)));
		}
	}

	void WindowEventExecutor::bind(HWND hwnd)
	{
		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(hwnd));
		::SetWindowLongPtr(hwnd, offsetof(WindowBindingInfo, _window), LONG_PTR(_window));
		::SetWindowLongPtr(hwnd, offsetof(WindowBindingInfo, _eventFetcher), LONG_PTR(_eventFetcher));
	}

	bool WindowEventExecutor::isMaximized() const
	{
		WINDOWPLACEMENT windowPlacement = {};
		windowPlacement.length = sizeof(WINDOWPLACEMENT);
		if (::GetWindowPlacement(_window->getHwnd(), &windowPlacement) == false)
			return false;

		return windowPlacement.showCmd == SW_MAXIMIZE;
	}


	void WindowEventExecutor::adjustMaximizedClientRect(RECT& rect)
	{
		if (isMaximized() == false)
			return;

		HMONITOR monitor = ::MonitorFromWindow(_window->getHwnd(), MONITOR_DEFAULTTONULL);
		if (monitor == NULL)
			return;

		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(monitorInfo);
		if (::GetMonitorInfoW(monitor, &monitorInfo) == FALSE)
			return;

		rect = monitorInfo.rcMonitor;
	}


	LRESULT WindowEventExecutor::hitTest(const POINT& cursor) const
	{
		const POINT border{
			::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
			::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
		};
		RECT windowRect;
		if (!::GetWindowRect(_window->getHwnd(), &windowRect)) {
			return HTNOWHERE;
		}

		const auto drag = HTCAPTION;

		enum region_mask {
			client = 0b0000,
			left = 0b0001,
			right = 0b0010,
			top = 0b0100,
			bottom = 0b1000,
		};

		const auto result =
			left * (cursor.x < (windowRect.left + border.x)) |
			right * (cursor.x >= (windowRect.right - border.x)) |
			top * (cursor.y <= (windowRect.top + border.y)) |
			bottom * (cursor.y >= (windowRect.bottom - border.y));

		switch (result) {
		case left: return HTLEFT;
		case right: return HTRIGHT;
		case top: return HTTOP;
		case bottom: return HTBOTTOM;
		case top | left: return HTTOPLEFT;
		case top | right: return HTTOPRIGHT;
		case bottom | left: return HTBOTTOMLEFT;
		case bottom | right: return HTBOTTOMRIGHT;
		case client: return (cursor.y < (windowRect.top + 40)) ? HTCAPTION : HTCLIENT;
		default: return HTNOWHERE;
		}
	}

	EventFetcherWin32::EventFetcherWin32(Application* application, Thread* ownerThread)
		: EventFetcher(application, ownerThread)
	{

	}

	EventFetcherWin32::~EventFetcherWin32()
	{
	}

	bool EventFetcherWin32::processEventQueue()
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				return false;
		}

		return true;
	}
} // namespace xr

namespace xr
{

	LRESULT WINAPI DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (hWnd == nullptr)
			return ::DefWindowProc(hWnd, msg, wParam, lParam);

		if (msg == WM_NCCREATE) {
			auto createstruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
			auto windowBindingInfo = reinterpret_cast<WindowBindingInfo*>(createstruct->lpCreateParams);
			auto windowEventExecutor = static_cast<WindowEventExecutor*>(windowBindingInfo);
			windowEventExecutor->bind(hWnd);
		}

		WindowEventExecutor executor(hWnd);
		if (executor.isManaged() == false)
			return ::DefWindowProc(hWnd, msg, wParam, lParam);

#if defined(USE_IMGUI)
		if (ImGuiBinderWin32Procedure(executor._window, hWnd, msg, wParam, lParam))
			return true;
#endif // #if defined(USE_IMGUI)

		switch (msg)
		{
		case WM_NCCALCSIZE: {
			const bool shouldIndicateClientArea = (wParam == TRUE);
			if (shouldIndicateClientArea) {
				auto& calcSizeParams = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
				executor.adjustMaximizedClientRect(calcSizeParams.rgrc[0]);
				return 0;
			}
			break;
		}
		case WM_NCHITTEST: {
			return executor.hitTest(POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
		}
		case WM_NCACTIVATE: {
			// Note(jiman): DWM이 처리하는 과정에 대한 파악 이후 추가 처리 필요.
			break;
		}
		case WM_SIZE: {
			const uint32_t width = (uint32_t)LOWORD(lParam);
			const uint32_t height = (uint32_t)HIWORD(lParam);
			executor._window->setWindowSize((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			executor._eventFetcher->_eventResize(executor._window, wParam == SIZE_MINIMIZED, wParam == SIZE_MAXIMIZED, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			return 0;
		}
		case WM_SYSCOMMAND: {
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		}
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP: {
			WORD virtualKeyCode = LOWORD(wParam);
			const bool controlKeyPressed = virtualKeyCode == VK_CONTROL;
			const bool altKeyPressed = virtualKeyCode == VK_MENU;
			const bool shiftKeyPressed = virtualKeyCode == VK_SHIFT;

			WORD keyFlags = HIWORD(lParam);
			const bool prevKeyPressed = (keyFlags & KF_REPEAT) == KF_REPEAT;
			const bool curKeyPressed = (keyFlags & KF_UP) != KF_UP;

			WORD keyPressedCount = LOWORD(lParam);

			executor._eventFetcher->_eventKeyboard(executor._window, controlKeyPressed, altKeyPressed, shiftKeyPressed, curKeyPressed, prevKeyPressed, keyPressedCount, virtualKeyCode);
			break;
		}
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_XBUTTONDBLCLK: {
			const unsigned left = ((msg == WM_LBUTTONDOWN) ? 0x1 : ((msg == WM_LBUTTONDBLCLK) ? 0x2 : ((msg == WM_LBUTTONUP) ? 0x4 : 0)));
			const unsigned middle = ((msg == WM_MBUTTONDOWN) ? 0x1 : ((msg == WM_MBUTTONDBLCLK) ? 0x2 : ((msg == WM_MBUTTONUP) ? 0x4 : 0)));
			const unsigned right = ((msg == WM_RBUTTONDOWN) ? 0x1 : ((msg == WM_RBUTTONDBLCLK) ? 0x2 : ((msg == WM_RBUTTONUP) ? 0x4 : 0)));
			const unsigned xButton = ((msg == WM_XBUTTONDOWN) ? 0x1 : ((msg == WM_XBUTTONDBLCLK) ? 0x2 : ((msg == WM_XBUTTONUP) ? 0x4 : 0)));

			executor._eventFetcher->_eventMouse(executor._window, left, middle, right, xButton, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		}
		case WM_CLOSE: {
			executor._eventFetcher->_eventOnClose(executor._window);
			break;
		}
		case WM_DESTROY: {
			executor._eventFetcher->_eventOnDestroy(executor._window);
			::PostQuitMessage(0);
			return 0;
		}
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

} // namespace xr


bool CreateDeviceD3D(HWND hWnd); // temp for imgui

namespace xr
{

	WindowWin32::WindowWin32(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription)
		: Window(application, eventFetcher, windowDescription)
	{
		//commonConstructor();
	}

	void WindowWin32::commonConstructor()
	{
		ApplicationWin32* applicationWin32 = static_cast<ApplicationWin32*>(getApplicationPlatform());
		const WindowDescription& description = getWindowDescription();
		const WNDCLASSEX& defaultWindowClass = applicationWin32->getDefaultWindowClass();

		wchar_t wcWindowTitle[256];
		{
			size_t newsize = description._title.length() + 1;

			size_t convertedChars = 0;
			errno_t result = mbstowcs_s(&convertedChars, wcWindowTitle, newsize, description._title.c_str(), 256 - 1);
		}
		DWORD borderlessWindowStyle = WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

		WindowBindingInfo windowBindingInfo;
		windowBindingInfo._window = this;
		windowBindingInfo._eventFetcher = static_cast<EventFetcherWin32*>(getEventFetcher());

		_hWnd = ::CreateWindowEx(0, defaultWindowClass.lpszClassName, wcWindowTitle, borderlessWindowStyle, 0, 0, description._width, description._height, NULL, NULL, defaultWindowClass.hInstance, &windowBindingInfo);

		// Show the window
		::ShowWindow(_hWnd, SW_SHOWDEFAULT);
		::UpdateWindow(_hWnd);
	}

	WindowWin32::~WindowWin32()
	{
		if (_hWnd != NULL)
			::DestroyWindow(_hWnd);
	}

	void* WindowWin32::getPlatformNativeHandle() const
	{
		return getHwnd();
	}

} // namespace xr>	XRFrameworkBase.dll!xr::WindowWin32::commonConstructor() Line 274	C++
