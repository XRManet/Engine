#include "stdafx.h"
#include <GLFW/glfw3.h>

#include <XRFrameworkBase/ApplicationGLFW.h>
#include <XRFrameworkBase/ThreadWin32.h>
#include <XRFrameworkBase/WindowGLFW.h>

#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/Application.h>

namespace xr
{
	std::unique_ptr<Thread>&& ApplicationGLFW::createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		auto thread = Thread::createThread(threadName, launchImmediatly, threadExecution);
		thread->bindApplication(getApplication(), &Application::addThread, &Application::removeThread);
		return std::move(thread);
	}

	std::unique_ptr<EventFetcher>&& ApplicationGLFW::createEventFetcher(Thread* ownerThread)
	{
		auto eventFetcher = std::unique_ptr<EventFetcher>(new EventFetcherGLFW(ownerThread));
		eventFetcher->bindApplication(getApplication(), &Application::addEventFetcher, &Application::removeEventFetcher);
		return std::move(eventFetcher);
	}

	std::unique_ptr<Window>&& ApplicationGLFW::createWindow(EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription)
	{
		auto window = std::unique_ptr<Window>(new WindowGLFW(getApplication(), eventFetcher, ownerThread, windowDescription));
		return std::move(window);
	}

} // namespace xr