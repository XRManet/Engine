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
  virtual ~XRRenderingStratagy() {}

public:
  virtual void Initialize() {}

public:
  virtual void Render() {}
};

class XRRenderingStratagyForward : public XRRenderingStratagy
{
  // Todo) 일단 렌더링 되는지 테스트해보려고 여기다 선언해서 그냥 써봄.
  // 아래 멤버변수들은 그래픽스 파이프라인에서나 선언해다 쓰고
  // 나중엔 Rendering stratagy와 pipeline 간의 관계를 정립한 뒤,
  // 렌더링으로 연결될 수 있게 변경할 것.
  GLuint _glProgram;
  GLuint _glVertexShader;
  GLuint _glFragmentShader;

  struct UNIFORM_BUFFER_NAME
  {
    enum : GLuint
    {
      Transform,
      Light,
      Count
    };
  };
  
  GLuint _uniformBuffers[UNIFORM_BUFFER_NAME::Count];

  struct Light
  {
    float position[4];
    float intensity[3];
    float attenuation;
  };

  struct Material
  {
    float diffuse[4];
  };

public:
  XRRenderingStratagyForward();
  ~XRRenderingStratagyForward();

  virtual void Render();
  virtual void Initialize();
};

#include <memory>
class XRFrameWalker
{
private:
  std::unique_ptr<XRRenderingStratagy> _rendering_stratagy;


public:
  XRFrameWalker();

  void Initialize()
  {
    _rendering_stratagy->Initialize();
  }

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