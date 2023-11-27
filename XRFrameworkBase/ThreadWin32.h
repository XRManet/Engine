#pragma once

#include "Thread.h"

namespace xr
{

	class ThreadWin32 : public Thread
	{
	public:
		ThreadWin32(bool createOnCurrentThread, ThreadExecution threadExecution, uint32_t& outThreadId);
		ThreadWin32(Application* ownerApplication, bool createOnCurrentThread, ThreadExecution threadExecution, uint32_t& outThreadId);

	private:
		void commonConstructor(bool createOnCurrentThrad, ThreadExecution threadExecution, uint32_t& outThreadId);

	public:
		virtual void setThreadName_inner(const char* threadName) override;

		virtual void suspend() override;
		virtual void resume() override;
		virtual bool yield() override;

		virtual uint32_t getExitCode() override;

	public:
		HANDLE getThreadHandle() const { return _hThread; }

	private:
		HANDLE _hThread;
	};

} // namespace xr

