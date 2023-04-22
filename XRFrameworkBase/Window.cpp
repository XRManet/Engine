#include "stdafx.h"
#include <XRFrameworkBase/Window.h>
#include <XRFrameworkBase/WindowUserContext.h>

#include <XRFrameworkBase/Application.h>
#include <XRFrameworkBase/Thread.h>

import Render;

namespace xr
{

	EventFetcher::EventFetcher(Thread* ownerThread)
		: ThreadChild(ownerThread)
	{
	}

	EventFetcher::~EventFetcher()
	{
	}

	Window::Window(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription)
		: Window(application, eventFetcher, Thread::getCurrentThread(), windowDescription)
	{
	}

	Window::Window(Application* application, EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription)
		: ApplicationChild(application, &Application::addWindow, &Application::removeWindow)
		, ThreadChild(ownerThread)
		, _boundEventFetcher(eventFetcher)
		, _windowDescription(windowDescription)
		, _boundSwapchain(nullptr)
	{
	}

	Window::~Window()
	{
	}

	void Window::attachUserContext(const std::string& key, std::unique_ptr<WindowUserContext>&& context)
	{
		_userContexts.emplace(key, std::move(context));
	}

	void Window::setSwapchain(render::Swapchain* swapchain)
	{
		_boundSwapchain = std::move(std::unique_ptr<render::Swapchain>(swapchain));
	}

} // namespace xr
