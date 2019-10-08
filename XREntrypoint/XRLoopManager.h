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
      static const GLuint versions[][2] = {
          {4, 6}, {4, 5}, {4, 3}, {4, 1}, {4, 0},
          {3, 3}, {3, 0},
          {2, 1}, {2, 0},
          {1, 1}, {1, 0}
      };
      GLuint version_try = 0;
      
      if (glfwInit() == 0)
          throw;
      
      while(1)
      {
          static const GLuint max_try = static_cast<GLuint>(sizeof(versions) / sizeof(versions[0]));
          if(version_try < max_try)
          {
              glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
              glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versions[version_try][0]);
              glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versions[version_try][1]);
              glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
              glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
              glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
              //glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
              glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
              glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
          }
          else
          {
              glfwDefaultWindowHints();
          }
          
          _window = glfwCreateWindow(size._width, size._height,
                                     "Hello world!", NULL, NULL);
          
          if (_window != nullptr)
          {
              printf("OpenGL version: ");
              if (version_try <= max_try)
                  printf("%d.%d\n", versions[version_try][0], versions[version_try][1]);
              else
                  printf("system-default\n");
              
              glfwMakeContextCurrent(_window);
              break;
          }
          else
          {
              ++version_try;
          }
      }
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

class XRScene;
class XRRenderingStratagy
{
public:
	virtual ~XRRenderingStratagy() {}

public:
	virtual void Initialize() {}

public:
	virtual void Update(XRScene* scene) {}
	virtual void Render() {}
};

class XRRenderingStratagyTest : public XRRenderingStratagy
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
  XRRenderingStratagyTest();
  ~XRRenderingStratagyTest();

  virtual void Initialize();

  virtual void Update(XRScene* scene);
  virtual void Render();
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
