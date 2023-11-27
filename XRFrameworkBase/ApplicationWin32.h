#pragma once

#include <XRFrameworkBase/ApplicationPlatform.h>

namespace xr
{

	class ApplicationWin32 : public ApplicationPlatform
	{
	public:
		ApplicationWin32(Application* application);
		virtual ~ApplicationWin32();

	private:
		virtual std::unique_ptr<Thread>&&		createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution) override final;

		virtual std::unique_ptr<EventFetcher>&&	createEventFetcher(Thread* ownerThread) override final;
		virtual std::unique_ptr<Window>&&		createWindow(EventFetcher* eventFetcher, WindowDescription& windowDescription) override final;

	private:
		virtual void							waitForAllThreads() override final;

	public:
		const WNDCLASSEX&						getDefaultWindowClass() const;
	};

} // namespace xr