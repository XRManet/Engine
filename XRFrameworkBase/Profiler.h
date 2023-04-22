#pragma once

#define MakeLocalName_Concat(name, line)    name##line
#define MakeLocalName_Eval(name, line)      MakeLocalName_Concat(name, line)
#define MakeLocalName(name)                 MakeLocalName_Eval(name, __LINE__)

namespace xr::profiler
{

	void beginTrace(const char* outFilename);
	void endTrace();

	extern bool g_isTracing;

	extern bool g_traceEnabled;
	extern bool g_traceCurrent;

	inline void flowFrom(const char* category, const char* name, void* id);
	inline void flowStep(const char* category, const char* name, void* id, const char* step);
	inline void flowTo(const char* category, const char* name, void* id);

#define scopeTracer(category, name) ScopeTrace MakeLocalName(tracer)(category, name)
	class ScopeTrace
	{
	public:
		inline ScopeTrace(const char* category, const char* name)
			: _category(category), _name(nullptr)
		{
			if (!g_isTracing) return;

			_name = name;
			begin();
		}

		inline ~ScopeTrace()
		{
			if (!g_isTracing || !_name) return;

			end();
		}

	private:
		void begin();
		void end();

	private:
		const char* _category;
		const char* _name;
	};

	void flowFrom_inner(const char* category, const char* name, void* id);
	inline void flowFrom(const char* category, const char* name, void* id)
	{
		if (!g_isTracing) return;
		flowFrom_inner(category, name, id);
	}

	void flowStep_inner(const char* category, const char* name, void* id, const char* step);
	inline void flowStep(const char* category, const char* name, void* id, const char* step)
	{
		if (!g_isTracing) return;
		flowStep_inner(category, name, id, step);
	}

	void flowTo_inner(const char* category, const char* name, void* id);
	inline void flowTo(const char* category, const char* name, void* id)
	{
		if (!g_isTracing) return;
		flowTo_inner(category, name, id);
	}


} // namespace xr::profiler