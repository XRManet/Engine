#pragma once

#include <XRFrameworkBase/XRDefault.h>

namespace xr
{
class XRBaseExport MutexLock
{
public:
	MutexLock();
	~MutexLock();
	MutexLock(const MutexLock&) = delete;

public:
	bool lock();
	bool unlock();

private:
	void* _mutex;
};

template<typename Lock>
class ScopedLocker
{
public:
	ScopedLocker(Lock& lockObject)
		: _lockObject(lockObject)
	{
		_lockObject.lock();
	}
	~ScopedLocker()
	{
		_lockObject.unlock();
	}

private:
	Lock& _lockObject;
};
}