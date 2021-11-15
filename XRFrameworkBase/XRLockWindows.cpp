#include "stdafx.h"

#include <XRFrameworkBase/XRPlatform.h>
#include "XRLock.h"

namespace xr
{
MutexLock::MutexLock()
{
	HANDLE& mutexHandle = static_cast<HANDLE&>(_mutex);

	LPSECURITY_ATTRIBUTES	defaultSecurityAttributes = NULL;
	BOOL					initiallyNotOwned = FALSE;
	LPCWSTR					unnamedMutex = NULL;

	mutexHandle = ::CreateMutex(defaultSecurityAttributes,
								initiallyNotOwned,
								unnamedMutex);
	assert(mutexHandle != NULL);
}

MutexLock::~MutexLock()
{
	HANDLE& mutexHandle = static_cast<HANDLE&>(_mutex);
	assert(mutexHandle != NULL);
	::CloseHandle(mutexHandle);
}

bool MutexLock::lock()
{
	HANDLE& mutexHandle = static_cast<HANDLE&>(_mutex);

	DWORD dwWaitResult = ::WaitForSingleObject(
		mutexHandle,    // handle to mutex
		INFINITE);		// no time-out interval

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0: // This thread got ownership of mutex object.
		return true;
	case WAIT_ABANDONED:// Error. The ownership of mutex object is abandoned.
		assert(false);
		break;
	case WAIT_FAILED:
		{
			DWORD lastError = GetLastError();
			assert(false);
	}
		break;
	default:
		assert(false);
	}
	return false;
}

bool MutexLock::unlock()
{
	HANDLE& mutexHandle = static_cast<HANDLE&>(_mutex);

	BOOL result = ::ReleaseMutex(mutexHandle);
	assert(result == TRUE);

	return result == TRUE;
}
}