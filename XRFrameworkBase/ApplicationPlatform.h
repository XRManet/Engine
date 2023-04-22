#pragma once

namespace xr
{
	class Thread;
	using ThreadExecution = std::function<uint32_t()>;

	class EventFetcher;
	class Window;
	struct WindowDescription;

	class ApplicationPlatform
	{
	public:
		virtual ~ApplicationPlatform() {}

	public:
		virtual void							waitForAllThreads() = 0;

		virtual std::unique_ptr<Thread>			createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution) = 0;

		virtual std::unique_ptr<EventFetcher>	createEventFetcher(Thread* ownerThread) = 0;
		virtual std::unique_ptr<Window>			createWindow(EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription) = 0;
	};

} // namespace xr