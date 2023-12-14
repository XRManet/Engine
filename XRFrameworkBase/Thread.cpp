#include "stdafx.h"
#include <XRFrameworkBase/Thread.h>

namespace xr
{
	std::unique_ptr<Thread> Thread::createThread(Application* ownerApplication, const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		uint32_t threadId = 0;
		std::unique_ptr<Thread> thread = createThreadPlatform(ownerApplication, false, threadExecution, threadId);
		thread->_threadId = threadId;

		if (threadName != nullptr)
			thread->setThreadName(threadName);

		return thread;
	}

	std::unique_ptr<Thread> Thread::createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		uint32_t threadId = 0;
		std::unique_ptr<Thread> thread = createThreadPlatform(false, threadExecution, threadId);
		thread->_threadId = threadId;

		if (threadName != nullptr)
			thread->setThreadName(threadName);

		return thread;
	}

	std::unique_ptr<Thread> Thread::bindThreadFromCurrent(const char* threadName, ThreadExecution threadExecution)
	{
		assert(hasThread() == false);

		uint32_t threadId = 0;
		std::unique_ptr<Thread> thread = createThreadPlatform(true, threadExecution, threadId);
		thread->_threadId = threadId;

		if (threadName != nullptr)
			thread->setThreadName(threadName);

		return thread;
	}

	std::unique_ptr<Thread> Thread::bindThreadFromCurrent(Application* ownerApplication, const char* threadName, ThreadExecution threadExecution)
	{
		std::unique_ptr<Thread> thread = bindThreadFromCurrent(threadName, threadExecution);
		
		if (ownerApplication != nullptr)
			thread->bindApplication(ownerApplication);

		return thread;
	}

	struct ThreadContext
	{
		Thread* _thread = nullptr;
	};

	struct ScopedThreadContextBinder
	{
		ScopedThreadContextBinder(ThreadContext& threadContext, Thread* currentThread)
			: _threadContext(threadContext)
		{
			_threadContext._thread = currentThread;
		}

		~ScopedThreadContextBinder()
		{
			_threadContext._thread = nullptr;
		}

	private:
		ThreadContext& _threadContext;
	};

	thread_local ThreadContext g_currentThreadContext;

	Thread* Thread::getCurrentThread()
	{
		assert(g_currentThreadContext._thread != nullptr);
		return g_currentThreadContext._thread;
	}

	bool Thread::hasThread()
	{
		return g_currentThreadContext._thread != nullptr;
	}

	Thread::Thread(ThreadExecution threadExecution)
		: _threadExecution(threadExecution)
	{
	}

	Thread::~Thread()
	{
	}

	void Thread::execute()
	{
		ScopedThreadContextBinder threadContextBinder(g_currentThreadContext, this);
		_threadExecution();
	}

} // namespace xr

#include <XRFrameworkBase/Application.h>

namespace xr
{
	Thread::Thread(Application* ownerApplication, ThreadExecution threadExecution)
		: ApplicationChild(ownerApplication, &Application::addThread, &Application::removeThread)
		, _threadExecution(threadExecution)
	{
	}

	void Thread::bindApplication(Application* ownerApplication)
	{
		ApplicationChild::bindApplication(ownerApplication, &Application::addThread, &Application::removeThread);
	}
} // namespace xr