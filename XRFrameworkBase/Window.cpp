#include "stdafx.h"
#include <XRFrameworkBase/Window.h>
#include <XRFrameworkBase/WindowUserContext.h>

#include <XRFrameworkBase/Application.h>
#include <XRFrameworkBase/Thread.h>

#include <XRFrameworkBase/XRSwapchain.h>

namespace xr
{

	EventFetcher::EventFetcher(Application* application, Thread* ownerThread)
		: ApplicationChild(application, &Application::addEventFetcher, &Application::removeEventFetcher)
		, ThreadChild(ownerThread)
	{
		assert(ownerThread->getApplication() == application && "A given ownerThread must be owned by the application that also owns this event fetcher.");
	}

	EventFetcher::~EventFetcher()
	{
	}

	void EventFetcher::processLoop(std::function<void()> loopWorks)
	{
		while (processEventQueue())
		{
			loopWorks();
		}
	}

	void EventFetcher::bindWindow(Window* window)
	{
		_boundWindows.emplace_back(window);
	}

	void EventFetcher::unbindWindow(Window* window)
	{
		std::erase_if(_boundWindows, [window](Window* elem) { return (window == elem); });
	}

	Window::Window(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription)
		: ApplicationChild(application, &Application::addWindow, &Application::removeWindow)
		, _boundEventFetcher(eventFetcher)
		, _windowDescription(windowDescription)
		, _boundSwapchain(nullptr)
	{
		assert(eventFetcher->getApplication() == application && "A given eventFetcher must be owned by the application that also owns this window.");
		assert(eventFetcher->getOwnerThread() == Thread::getCurrentThread() && "Creating a window must be called in a thread that owns a eventFetcher.");
		_boundEventFetcher->bindWindow(this);
	}

	Window::~Window()
	{
	}

	EventFetcher* Window::getEventFetcher() const { return _boundEventFetcher; }
	const WindowDescription& Window::getWindowDescription() const { return _windowDescription; }
	XRSwapchain* Window::getSwapchain() const { return _boundSwapchain.get(); }

	void Window::attachUserContext(const std::string& key, std::unique_ptr<WindowUserContext>&& context)
	{
		_userContexts.emplace(key, std::move(context));
	}

	const std::unordered_map<
		std::string,
		std::unique_ptr<WindowUserContext>
	>& Window::getUserContexts() const { return _userContexts; }

	void Window::setSwapchain(XRSwapchain* swapchain)
	{
		_boundSwapchain = std::move(std::unique_ptr<XRSwapchain>(swapchain));
	}

	void Window::setWindowSize(uint32_t width, uint32_t height)
	{
		_windowDescription._width = width;
		_windowDescription._height = height;
	}


} // namespace xr
