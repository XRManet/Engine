#pragma once

#include <XRFrameworkBase/ApplicationPlatform.h>

namespace xr
{

	class Thread;
	using ThreadExecution = std::function<uint32_t()>;

	class EventFetcher;
	class Window;
	struct WindowDescription;

	class ApplicationChild;

	enum class PlatformType
	{
		GLFW,
		Win32,
		NumOfPlatformTypes
	};

	namespace render { class Renderable; }

	class Application
	{
	public:
		Application(PlatformType platformType);
		Application(PlatformType platformType, ThreadExecution threadExecution);
		virtual ~Application();

	public:
		static void								runApplication(Application* app, int argc, char** argv);

	private:
		void									runMainThread();
		inline void								waitForAllThreads() { _applicationPlatform->waitForAllThreads(); }

	public:
		uint32_t								defaultMainThreadRun();

	public:
		Thread*									getMainThread() { return _mainThread.get(); }

	protected:
		const std::vector<Thread*>&				getAllThreads() const { return _allThreads; }


	private:
		static std::unique_ptr<ApplicationPlatform>	createApplicationPlatform(PlatformType platformType);

	private:
		inline std::unique_ptr<Thread>			createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution) { return _applicationPlatform->createThread(threadName, launchImmediatly, threadExecution); }

		inline std::unique_ptr<EventFetcher>	createEventFetcher(Thread* ownerThread) { return _applicationPlatform->createEventFetcher(ownerThread); }
		inline std::unique_ptr<Window>			createWindow(EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription) { return _applicationPlatform->createWindow(eventFetcher, ownerThread, windowDescription); }

	public:
		void									addThread(ApplicationChild* child);
		void									removeThread(ApplicationChild* child);

		void									addEventFetcher(ApplicationChild* child);
		void									removeEventFetcher(ApplicationChild* child);

		void									addWindow(ApplicationChild* child);
		void									removeWindow(ApplicationChild* child);

	private:
		std::unique_ptr<Thread>					_mainThread;
		std::vector<Thread*>					_allThreads;

		std::unique_ptr<ApplicationPlatform>	_applicationPlatform;
		PlatformType							_platformType;
	};

} // namespace xr