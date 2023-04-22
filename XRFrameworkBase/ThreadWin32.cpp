#include "stdafx.h"
#include <XRFrameworkBase/ThreadWin32.h>

namespace xr
{
	DWORD WINAPI threadMain(LPVOID threadParam)
	{
		auto threadWin32 = static_cast<ThreadWin32*>(threadParam);
		threadWin32->execute();

		return 0;
	}

	std::unique_ptr<Thread> Thread::createThread(bool createOnCurrentThread, ThreadExecution threadExecution, uint32_t& outThreadId)
	{
		return std::unique_ptr<Thread>(new ThreadWin32(createOnCurrentThread, threadExecution, outThreadId));
	}

	ThreadWin32::ThreadWin32(bool createOnCurrentThrad, ThreadExecution threadExecution, uint32_t& outThreadId)
		: Thread(threadExecution)
	{
		if (createOnCurrentThrad)
		{
			_hThread = GetCurrentThread();
			outThreadId = GetCurrentThreadId();
		}
		else
		{
			LPSECURITY_ATTRIBUTES	defaultSecurityAttributes = NULL;
			DWORD					defaultStackSize = 0;
			DWORD					defaultCreationFlags = 0;
			_hThread = ::CreateThread(defaultSecurityAttributes, defaultStackSize, threadMain, this, defaultCreationFlags, (LPDWORD)&outThreadId);
		}
	}

	void ThreadWin32::setThreadName_inner(const char* threadName)
	{
		wchar_t wcThreadName[256];
		size_t newsize = strlen(threadName) + 1;

		size_t convertedChars = 0;
		errno_t result = mbstowcs_s(&convertedChars, wcThreadName, newsize, threadName, 256 - 1);

		::SetThreadDescription(_hThread, wcThreadName);
	}

	void ThreadWin32::suspend()
	{
		::SuspendThread(_hThread);
	}

	void ThreadWin32::resume()
	{
		::ResumeThread(_hThread);
	}

	bool ThreadWin32::yield()
	{
		return ::SwitchToThread() != FALSE;
	}

	uint32_t ThreadWin32::getExitCode()
	{
		DWORD exitCode = 0;
		BOOL result = ::GetExitCodeThread(_hThread, &exitCode);
		if (result != FALSE)
			return exitCode;

		assert(result != FALSE);
		DWORD errorCode = ::GetLastError();
		return ~0;
	}

} // namespace xr