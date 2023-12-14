#include "stdafx.h"
#include <GLFW/glfw3.h>

#include <XRFrameworkBase/ApplicationGLFW.h>
#include <XRFrameworkBase/ThreadWin32.h>
#include <XRFrameworkBase/WindowGLFW.h>

#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/Application.h>

namespace xr
{
	std::unique_ptr<Thread> ApplicationGLFW::createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		auto thread = Thread::createThread(getApplication(), threadName, launchImmediatly, threadExecution);
		return thread;
	}

	std::unique_ptr<EventFetcher> ApplicationGLFW::createEventFetcher(Thread* ownerThread)
	{
		auto eventFetcher = std::unique_ptr<EventFetcher>(new EventFetcherGLFW(getApplication(), ownerThread));
		return eventFetcher;
	}

	std::unique_ptr<Window> ApplicationGLFW::createWindow(EventFetcher* eventFetcher, WindowDescription& windowDescription)
	{
		auto window = std::unique_ptr<Window>(new WindowGLFW(getApplication(), eventFetcher, windowDescription));
		return window;
	}

} // namespace xr