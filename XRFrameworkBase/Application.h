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
		Application(std::string&& name, PlatformType platformType);
		Application(std::string&& name, PlatformType platformType, ThreadExecution threadExecution);
		virtual ~Application();

	public:
		static void								runApplication(Application* app, int argc, char** argv);

	private:
		void									runMainThread();
		inline void								waitForAllThreads();

	public:
		uint32_t								defaultMainThreadRun();

	public:
		const std::string&						getName() const;

		Thread*									getMainThread();
		const std::vector<Thread*>&				getAllThreads() const;

		ApplicationPlatform*					getApplicationPlatform() const;


	private:
		static std::unique_ptr<ApplicationPlatform>	createApplicationPlatform(Application* application, PlatformType platformType);

	protected:
		std::unique_ptr<Thread>					createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution);

		std::unique_ptr<EventFetcher>			createEventFetcher(Thread* ownerThread);
		std::unique_ptr<Window>					createWindow(EventFetcher* eventFetcher, WindowDescription& windowDescription);

	public:
		virtual void							addThread(ApplicationChild* child);
		virtual void							removeThread(ApplicationChild* child);

		virtual void							addEventFetcher(ApplicationChild* child);
		virtual void							removeEventFetcher(ApplicationChild* child);

		virtual void							addWindow(ApplicationChild* child);
		virtual void							removeWindow(ApplicationChild* child);

		virtual void							addRenderEngine(ApplicationChild* child);
		virtual void							removeRenderEngine(ApplicationChild* child);

	private:
		std::unique_ptr<Thread>					_mainThread;
		std::vector<Thread*>					_allThreads;

		Window*									_mainWindow = nullptr;

		std::unique_ptr<ApplicationPlatform>	_applicationPlatform;
		PlatformType							_platformType;
		std::string								_name;
	};

} // namespace xr