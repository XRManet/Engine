#include "stdafx.h"
#include <GLFW/glfw3.h>

#include <XRFrameworkBase/WindowGLFW.h>
#include <XRFrameworkBase/ApplicationGLFW.h>

namespace xr
{
	EventFetcherGLFW::EventFetcherGLFW(Thread* ownerThread)
		: EventFetcher(ownerThread)
	{

	}

	EventFetcherGLFW::~EventFetcherGLFW()
	{
	}

	bool EventFetcherGLFW::processEventQueue()
	{
		glfwPollEvents();

		if (glfwWindowShouldClose(window) == false)
		{
			glfwDestroyWindow(window);
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
		case WM_NCHITTEST:
			return executor.hitTest(POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
			break;
		case WM_NCACTIVATE: {
			// Note(jiman): DWM이 처리하는 과정에 대한 파악 이후 추가 처리 필요.
			break;
		}
		case WM_SIZE: {
			const uint32_t width = (uint32_t)LOWORD(lParam);
			const uint32_t height = (uint32_t)HIWORD(lParam);
			executor._window->setWindowSize((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			executor._eventFetcher->eventResize(executor._window, wParam == SIZE_MINIMIZED, wParam == SIZE_MAXIMIZED, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			return 0;
		}
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;

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

			executor._eventFetcher->eventKeyboard(executor._window, controlKeyPressed, altKeyPressed, shiftKeyPressed, curKeyPressed, prevKeyPressed, keyPressedCount, virtualKeyCode);
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

			executor._eventFetcher->eventMouse(executor._window, left, middle, right, xButton, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		}
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

} // namespace xr


bool CreateDeviceD3D(HWND hWnd); // temp for imgui

namespace xr
{

	WindowGLFW::WindowGLFW(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription)
		: Window(application, eventFetcher, windowDescription)
	{
		constructorCommon(eventFetcher);
	}

	WindowGLFW::WindowGLFW(Application* application, EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription)
		: Window(application, eventFetcher, ownerThread, windowDescription)
	{
		constructorCommon(eventFetcher);
	}

	void WindowGLFW::constructorCommon(EventFetcher* eventFetcher)
	{
		ApplicationGLFW* applicationGLFW = static_cast<ApplicationGLFW*>(getApplicationPlatform());
		EventFetcherGLFW* eventFetcherGLFW = static_cast<EventFetcherGLFW*>(eventFetcher);
		const WindowDescription& description = getWindowDescription();
		
		assert(eventFetcherGLFW->getOwnerThread() == getOwnerThread() && "A given event fetcher must be owned by the thread that also owns this window.");

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