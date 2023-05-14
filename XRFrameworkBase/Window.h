#pragma once

#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/ThreadChild.h>


class XRSwapchain;

namespace xr
{
	class Application;
	class Window;
	struct WindowUserContext;

	struct EventKey
	{
		uint32_t _32bitIdentifier = 0;
	};

	template<typename ReturnType, typename... Arguments>
	struct EventCaller
	{
		using EventExecution = std::function<ReturnType(Arguments...)>;

		EventKey registerEvent(std::function<ReturnType(Arguments...)> function);
		void unregisterEvent(EventKey resizeEventKey);

		void operator()(Arguments... arguments)
		{
			for (auto& eventFunction : _eventFunctions)
			{
				eventFunction((arguments)...);
			}
		}

	private:
		std::vector<std::function<ReturnType(Arguments...)>> _eventFunctions;
		std::vector<EventKey> _eventKeys;
		EventKey _eventKeyGenerator;
	};

	class EventFetcher : public ApplicationChild, public ThreadChild
	{
	public:
		EventFetcher(Thread* ownerThread);
		virtual ~EventFetcher();

	public:
		void processLoop(std::function<void()> loopWorks);

	private:
		virtual bool processEventQueue() = 0;

	public:
		EventCaller<void, Window* /*window*/, bool /*minimized*/, bool /*maximized*/, uint32_t /*width*/, uint32_t /*height*/> eventResize;
		EventCaller<void, Window* /*window*/, bool /*ctrl*/, bool /*alt*/, bool /*shift*/, bool /*currentPressed*/, bool /*previousPressed*/, uint32_t /*keyPressedCount*/, uint32_t /*key*/> eventKeyboard;
		EventCaller<void, Window* /*window*/, const unsigned /*left*/, const unsigned /*middle*/, const unsigned /*right*/, const unsigned /*xButton*/, const int /*windowX*/, const int /*windowY*/> eventMouse;
	};

	struct WindowDescription
	{
		std::string _title;
		uint32_t _width = 0;
		uint32_t _height = 0;
	};

	class Window : public ApplicationChild, public ThreadChild
	{
	public:
		Window(Application* application, EventFetcher* eventFetcher, WindowDescription& windowDescription);
		Window(Application* application, EventFetcher* eventFetcher, Thread* ownerThread, WindowDescription& windowDescription);
		virtual ~Window();

	public:
		EventFetcher* getEventFetcher() const { return _boundEventFetcher; }
		WindowDescription& getWindowDescription() { return _windowDescription; }
		XRSwapchain* getSwapchain() const { return _boundSwapchain.get(); }

	public:
		void attachUserContext(const std::string& key, std::unique_ptr<WindowUserContext>&& context);
		auto& getUserContexts() const { return _userContexts; }


	protected:
		void setSwapchain(XRSwapchain* swapchain);
		void setWindowSize(uint32_t width, uint32_t height) { _windowDescription._width = width; _windowDescription._height = height; }

	private:
		EventFetcher* _boundEventFetcher;
		WindowDescription _windowDescription;

		std::unique_ptr<XRSwapchain> _boundSwapchain;
		std::unordered_map<std::string, std::unique_ptr<WindowUserContext>> _userContexts;
	};

} // namespace xr


namespace xr
{
	template<typename ReturnType, typename... Arguments>
	EventKey EventCaller<ReturnType, Arguments...>::registerEvent(std::function<ReturnType(Arguments...)> function)
	{
		_eventKeyGenerator._32bitIdentifier++;
		assert(_eventKeyGenerator._32bitIdentifier != 0);

		_eventFunctions.push_back(function);
		_eventKeys.push_back(_eventKeyGenerator);

		return _eventKeyGenerator;
	}

	template<typename ReturnType, typename... Arguments>
	void EventCaller<ReturnType, Arguments...>::unregisterEvent(EventKey eventKey)
	{
		for (int i = 0; i < _eventKeys.size(); ++i)
		{
			if (_eventKeys[i]._32bitIdentifier == eventKey._32bitIdentifier)
			{
				_eventFunctions.erase(_eventFunctions.begin() + i);
				_eventKeys.erase(_eventKeys.begin() + i);
				break;
			}
		}
	}
} // namespace xr