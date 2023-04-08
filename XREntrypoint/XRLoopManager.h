#pragma once

#include <XRFrameworkBase/XRGeometry.h>
#include <XRFrameworkBase/XRPlatform.h>

enum XRLoopManagerInfra : unsigned {
	GLFW,
	Default = GLFW,
};

template<XRLoopManagerInfra Infra>
class XRRenderingInfra
{
	XRRenderingInfra(XRSize<float> const& size) {}
};

struct GLFWwindow;

template<>
class XRRenderingInfra<GLFW>
{
public:
	XRRenderingInfra(XRSize<float> const& size);
	~XRRenderingInfra();


private:
	GLFWwindow * _window;

private: // GLFW Events
	static void InputKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void InputMouse(GLFWwindow* window, int button, int action, int mods);
	static void PositionMouse(GLFWwindow* window, double xpos, double ypos);
	static void ScrollMouse(GLFWwindow* window, double xoffset, double yoffset);

public:
	bool WindowShouldClose();
	void PollEvents();
};

class XRScene;

#include <memory>
class XRRenderer;
class XRFrameWalker
{
private:
	XRRenderer* _renderer;


public:
	XRFrameWalker();

	void Initialize();
	void UpdateFrame();
};

template<XRLoopManagerInfra Infra>
class XRLoopManager
{
private:
	XRSize<float> _out_size;
	XRRenderingInfra<Infra> _infra;
	XRFrameWalker _frame_walker;

public:
	XRLoopManager(XRSize<float>&& size) : _out_size(size), _infra(size)
	{
		
	}
	~XRLoopManager() {}


public:
	XRSize<float> const& GetSize() { return _out_size; }


public:
	void Initialize()
	{
		_frame_walker.Initialize();
	}

	void Run()
	{
		while (_infra.WindowShouldClose() == false)
		{
			_frame_walker.UpdateFrame();
			_infra.PollEvents();
		}
	}
};
