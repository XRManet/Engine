#pragma once

namespace xr
{
	class Thread;

	class ThreadChild
	{
	public:
		ThreadChild(Thread* ownerTread);
		virtual ~ThreadChild();

	public:
		Thread* getOwnerThread() { return _ownerThread; }

	private:
		Thread* _ownerThread;
	};

} // namespace xr