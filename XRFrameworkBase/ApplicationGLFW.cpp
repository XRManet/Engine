#include "stdafx.h"

#include <XRFrameworkBase/ApplicationGLFW.h>
#include <XRFrameworkBase/ThreadWin32.h>

#include "WindowWin32.h"
#include <XRFrameworkBase/Application.h>

namespace xr
{

	struct {
		bool _useMultiThreadEventProcessing = false;
	} g_win32Options;


	struct {
		WNDCLASSEX windowClass;
	} g_win32Default;

	LRESULT WINAPI DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	ApplicationGLFW::ApplicationGLFW(Application* application)
		: ApplicationPlatform(application)
	{
		// Create application window
		//ImGui_ImplWin32_EnableDpiAwareness();
	}

	ApplicationGLFW::~ApplicationGLFW()
	{
	}

	void ApplicationGLFW::waitForAllThreads()
	{
		const auto& allThreads = getApplication()->getAllThreads();
		std::vector<HANDLE> threadHandles;
		threadHandles.reserve(allThreads.size());

		for (auto& thread : allThreads)
		{
			threadHandles.push_back(static_cast<ThreadWin32*>(thread)->getThreadHandle());
		}

		::WaitForMultipleObjects(static_cast<DWORD>(threadHandles.size()), threadHandles.data(), TRUE, INFINITE);
	}

} // namespace xr