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

	class XRBaseExport Application
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
		const std::vector<Thread*>&				getAllThreads() const { return _allThreads; }

		ApplicationPlatform*					getApplicationPlatform() const { return _applicationPlatform.get(); }


	private:
		static std::unique_ptr<ApplicationPlatform>	createApplicationPlatform(Application* application, PlatformType platformType);

	protected:
		inline std::unique_ptr<Thread>&&		createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution) { return std::move(_applicationPlatform->createThread(threadName, launchImmediatly, threadExecution)); }

		inline std::unique_ptr<EventFetcher>&&	createEventFetcher(Thread* ownerThread) { return std::move(_applicationPlatform->createEventFetcher(ownerThread)); }
		inline std::unique_ptr<Window>&&		createWindow(EventFetcher* eventFetcher, WindowDescription& windowDescription) { return std::move(_applicationPlatform->createWindow(eventFetcher, windowDescription)); }

	public:
		virtual void							addThread(ApplicationChild* child);
		virtual void							removeThread(ApplicationChild* child);

		virtual void							addEventFetcher(ApplicationChild* child);
		virtual void							removeEventFetcher(ApplicationChild* child);

		virtual void							addWindow(ApplicationChild* child);
		virtual void							removeWindow(ApplicationChild* child);

	private:
		std::unique_ptr<Thread>					_mainThread;
		std::vector<Thread*>					_allThreads;

		std::unique_ptr<ApplicationPlatform>	_applicationPlatform;
		PlatformType							_platformType;
	};

} // namespace xr