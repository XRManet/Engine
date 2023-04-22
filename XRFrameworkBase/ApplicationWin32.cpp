#include "stdafx.h"

#include <XRFrameworkBase/ApplicationWin32.h>
#include <XRFrameworkBase/ThreadWin32.h>

#include "WindowWin32.h"

namespace xr
{

	struct {
		bool _useMultiThreadEventProcessing = false;
	} g_win32Options;


	struct {
		WNDCLASSEX windowClass;
	} g_win32Default;

	LRESULT WINAPI DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	ApplicationWin32::ApplicationWin32()
	{
		// Create application window
		//ImGui_ImplWin32_EnableDpiAwareness();

		g_win32Default.windowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, DefaultWndProc, 0L, sizeof(WindowBindingInfo) + sizeof(LONG_PTR), GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("XRManet Win32 Application"), NULL };
		::RegisterClassEx(&g_win32Default.windowClass);
	}

	ApplicationWin32::~ApplicationWin32()
	{
		::UnregisterClass(g_win32Default.windowClass.lpszClassName, g_win32Default.windowClass.hInstance);
	}

	void ApplicationWin32::waitForAllThreads()
	{
		const auto& allThreads = getAllThreads();
		std::vector<HANDLE> threadHandles;
		threadHandles.reserve(allThreads.size());

		for (auto& thread : allThreads)
		{
			threadHandles.push_back(static_cast<ThreadWin32*>(thread)->getThreadHandle());
		}

		::WaitForMultipleObjects(static_cast<DWORD>(threadHandles.size()), threadHandles.data(), TRUE, INFINITE);
	}

	const WNDCLASSEX& ApplicationWin32::getDefaultWindowClass() const
	{
		return g_win32Default.windowClass;
	}

} // namespace xr