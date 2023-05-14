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

	std::unique_ptr<Thread>&& ApplicationGLFW::createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		std::unique_ptr<Thread> temp;
		return std::move(temp);
	}

	std::unique_ptr<EventFetcher>&& ApplicationGLFW::createEventFetcher(Thread* ownerThread)
	{
		std::unique_ptr<EventFetcher> temp;
		return std::move(temp);
	}

	std::unique_ptr<Window>&& ApplicationGLFW::createWindow(EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription)
	{
		std::unique_ptr<Window> temp;
		return std::move(temp);
	}

} // namespace xr