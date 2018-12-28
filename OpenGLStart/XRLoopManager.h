#pragma once

#include <XRFrameworkBase/XRGeometry.h>

enum XRLoopManagerInfra : unsigned {
  GLFW,
  Default = GLFW,
};

template<XRLoopManagerInfra Infra>
class XRRenderingInfra
{
  XRRenderingInfra(XRSize const& size) {}
};

struct GLFWwindow;

template<>
class XRRenderingInfra<GLFW>
{
public:
  XRRenderingInfra(XRSize const& size)
  {
    if (glfwInit() != 0) {
      // Todo: Retrieve proper version for OpenGL.
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    }
    else throw;


    _window = glfwCreateWindow(size._width, size._height,
      "Hello world!", NULL, NULL);

    if (_window != nullptr) {
      glfwMakeContextCurrent(_window);
    }

    else throw;
  }

  ~XRRenderingInfra()
  {
    if (!_window) {
      glfwTerminate();
    }
  }


private:
  GLFWwindow * _window;


public:
  bool WindowShouldClose() { return glfwWindowShouldClose(_window); }
  void PollEvents()
  {
    /* Swap front and back buffers */
    glfwSwapBuffers(_window);

    /* Poll for and process events */
    glfwPollEvents();
  }
};

class XRRenderingStratagy
{
public:
  virtual void Render() {}
};

class XRRenderingStratagyForward : public XRRenderingStratagy
{
public:
  virtual void Render();
};

#include <memory>
class XRFrameWalker
{
private:
  std::unique_ptr<XRRenderingStratagy> _rendering_stratagy;


public:
  XRFrameWalker();


public:
  void UpdateFrame();
};

template<XRLoopManagerInfra Infra>
class XRLoopManager
{
private:
  XRSize _out_size;
  XRRenderingInfra<Infra> _infra;
  XRFrameWalker _frame_walker;

public:
  XRLoopManager(XRSize&& size) : _out_size(size), _infra(size)
  {
  }
  ~XRLoopManager() {}


public:
  XRSize const& GetSize() { return _out_size; }


public:
  void Run()
  {
    while (_infra.WindowShouldClose() == false)
    {
      _frame_walker.UpdateFrame();
      _infra.PollEvents();
    }
  }
};