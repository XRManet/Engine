#include "stdafx.h"
#include <XRFrameworkBase/ThreadChild.h>

#include <XRFrameworkBase/Thread.h>

namespace xr
{
	ThreadChild::ThreadChild(Thread* explicitThread)
		: _ownerThread(explicitThread)
	{
	}

	ThreadChild::~ThreadChild()
	{
	}

} // namespace xr
