#pragma once

#include <XRFrameworkBase/ApplicationPlatform.h>


namespace xr
{
	class Application;

	class ApplicationGLFW : public ApplicationPlatform
	{
	public:
		ApplicationGLFW(Application* application);
		virtual ~ApplicationGLFW();

	private:
		virtual std::unique_ptr<Thread>&&		createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution) override final;

		virtual std::unique_ptr<EventFetcher>&&	createEventFetcher(Thread* ownerThread) override final;
		virtual std::unique_ptr<Window>&&		createWindow(EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription) override final;

	private:
		virtual void							waitForAllThreads() override final;
	};

} // namespace xr