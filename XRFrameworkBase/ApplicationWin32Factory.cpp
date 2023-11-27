#include "stdafx.h"
#include <XRFrameworkBase/ApplicationWin32.h>
#include <XRFrameworkBase/ThreadWin32.h>
#include <XRFrameworkBase/WindowWin32.h>

#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/Application.h>

namespace xr
{
	std::unique_ptr<Thread>&& ApplicationWin32::createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		auto thread = Thread::createThread(getApplication(), threadName, launchImmediatly, threadExecution);
		return std::move(thread);
	}

	std::unique_ptr<EventFetcher>&& ApplicationWin32::createEventFetcher(Thread* ownerThread)
	{
		auto eventFetcher = std::unique_ptr<EventFetcher>(new EventFetcherWin32(getApplication(), ownerThread));
		return std::move(eventFetcher);
	}

	std::unique_ptr<Window>&& ApplicationWin32::createWindow(EventFetcher* eventFetcher, WindowDescription& windowDescription)
	{
		auto window = std::unique_ptr<Window>(new WindowWin32(getApplication(), eventFetcher, windowDescription));
		return std::move(window);
	}

} // namespace xr