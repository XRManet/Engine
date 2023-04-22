#include "stdafx.h"

#include <minitrace.h>
#include <minitrace.c>

#include "Profiler.h"
#include "Thread.h"

namespace xr::profiler
{
	static std::atomic<uint32_t> g_tracedThreadCounts = 0;
	static Thread* g_tracedThreads[64] = {};
	static uint32_t* g_tracedThreadIds[64] = {};

	thread_local uint32_t t_tracedThreadId = ~0;

	bool g_isTracing = false;

	bool g_traceEnabled = false;
	bool g_traceCurrent = false;

	static inline void makeCurrent()
	{
		if (t_tracedThreadId == ~0)
		{
			t_tracedThreadId = g_tracedThreadCounts++;
			g_tracedThreadIds[t_tracedThreadId] = &t_tracedThreadId;
			g_tracedThreads[t_tracedThreadId] = Thread::getCurrentThread();
		}
	}

	void beginTrace(const char* outFilename)
	{
		mtr_init(outFilename);
		MTR_META_PROCESS_NAME("Reality Projector");

		g_tracedThreadCounts = 0;
		g_isTracing = true;

		makeCurrent();
	}

	void endTrace()
	{
		uint32_t count = g_tracedThreadCounts;
		for (uint32_t i = 0; i < count; ++i)
		{
			Thread* thread = g_tracedThreads[i];
			internal_mtr_raw_event_tid_arg("", "thread_name", 'M', 0, int(thread->getThreadId()), MTR_ARG_TYPE_STRING_COPY, "name", (void*)thread->getThreadName());

			// deinitialize
			*g_tracedThreadIds[i] = ~0;
			g_tracedThreadIds[i] = nullptr;
			g_tracedThreads[i] = nullptr;
		}

		g_isTracing = false;

		mtr_flush();
		mtr_shutdown();
	}

	void ScopeTrace::begin()
	{
		makeCurrent();
		MTR_BEGIN(_category, _name);
	}

	void ScopeTrace::end()
	{
		makeCurrent();
		MTR_END(_category, _name);
	}

	void flowFrom_inner(const char* category, const char* name, void* id)
	{
		makeCurrent();
		MTR_FLOW_START(category, name, id);
	}

	void flowStep_inner(const char* category, const char* name, void* id, const char* step)
	{
		makeCurrent();
		MTR_FLOW_STEP(category, name, id, step);
	}

	void flowTo_inner(const char* category, const char* name, void* id)
	{
		makeCurrent();
		MTR_FLOW_FINISH(category, name, id);
	}

} // namespace xr::profiler