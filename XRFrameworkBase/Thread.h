#pragma once

#include <XRFrameworkBase/ApplicationChild.h>

namespace xr
{
	// Wrap std::function, std::bind
	using ThreadExecution = std::function<uint32_t()>;
	template<typename F, typename... Args>				auto bindThreadExecution(F&& f, Args&&... args) { return std::bind(std::forward<F>(f), std::forward<Args>(args)...); }
	template<typename R, typename F, typename... Args>	auto bindThreadExecution(F&& f, Args&&... args) { return std::bind<R, F>(std::forward<F>(f), std::forward<Args>(args)...); }

	class Thread : public ApplicationChild
	{
	public:
		Thread(ThreadExecution threadExecution);
		virtual ~Thread();

	public:
		static std::unique_ptr<Thread> createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution);
		static std::unique_ptr<Thread> bindThreadFromCurrent(const char* threadName, ThreadExecution threadExecution);

	public:
		static Thread* getCurrentThread();

	public:
		void execute();

	public:
		void setThreadName(const char* threadName) { _threadName = threadName; setThreadName_inner(threadName); };
		const char* getThreadName() const { return _threadName.c_str(); }
		uint32_t getThreadId() const { return _threadId; }

	private:
		virtual void setThreadName_inner(const char* threadName) = 0;

	public:
		virtual void suspend() = 0;
		virtual void resume() = 0;
		virtual bool yield() = 0;

	public:
		virtual uint32_t getExitCode() = 0;

	private:
		static std::unique_ptr<Thread> createThread(bool createOnCurrentThread, ThreadExecution threadExecution, uint32_t& outThreadId);

	private:
		ThreadExecution _threadExecution;
		std::string _threadName;
		uint32_t _threadId;
	};

} // namespace xr