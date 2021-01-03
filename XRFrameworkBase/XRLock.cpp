#include "stdafx.h"

#include <XRFrameworkBase/XRLock.h>

#include <XRFrameworkBase/XRPlatform.h>
#include <mutex>

namespace xr
{
#if XR_PLATFORM == XR_PLATFORM_OSX
MutexLock::MutexLock()
{
	std::mutex& mutex = reinterpret_cast<std::mutex&>(_mutex);
	new (&mutex) std::mutex;
}

MutexLock::~MutexLock()
{
	std::mutex& mutex = reinterpret_cast<std::mutex&>(_mutex);
	mutex.~mutex();
}

bool MutexLock::lock()
{
	std::mutex& mutex = reinterpret_cast<std::mutex&>(_mutex);
	mutex.lock();
	return true;
}

bool MutexLock::unlock()
{
	std::mutex& mutex = reinterpret_cast<std::mutex&>(_mutex);
	mutex.unlock();
	return true;
}
#endif
}
