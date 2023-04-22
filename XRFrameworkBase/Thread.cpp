#include "stdafx.h"
#include <XRFrameworkBase/Thread.h>

namespace xr
{
	std::unique_ptr<Thread> Thread::createThread(const char* threadName, bool launchImmediatly, ThreadExecution threadExecution)
	{
		uint32_t threadId = 0;
		std::unique_ptr<Thread> thread = createThread(false, threadExecution, threadId);
		thread->_threadId = threadId;

		if (threadName != nullptr)
			thread->setThreadName(threadName);

		return thread;
	}

	std::unique_ptr<Thread> Thread::bindThreadFromCurrent(const char* threadName, ThreadExecution threadExecution)
	{
		uint32_t threadId = 0;
		std::unique_ptr<Thread> thread = createThread(true, threadExecution, threadId);
		thread->_threadId = threadId;

		if (threadName != nullptr)
			thread->setThreadName(threadName);

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