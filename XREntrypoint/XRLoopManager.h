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
	XRRenderingInfra(XRSize<float> const& size)
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

		while (1)
		{
			static const GLuint max_try = static_cast<GLuint>(sizeof(versions) / sizeof(versions[0]));
			if (version_try < max_try)
			{
				// Todo: Query for each platform
				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versions[version_try][0]);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versions[version_try][1]);
				// Note: Use compatible profile because there is a bug in GLEW for Windows
#if XR_PLATFORM == XR_PLATFORM_WINDOWS
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#elif XR_PLATFORM == XR_PLATFORM_OSX
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
				glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
				// Note: To control with EGL may not be available.
				//glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
				glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
				glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#if defined(_DEBUG) || defined(DEBUG)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
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

				glfwSetWindowUserPointer(_window, this);
				glfwMakeContextCurrent(_window);
				break;
			}
			else
			{
				++version_try;
			}
		}

		glfwSetKeyCallback(_window, InputKeyboard);
		glfwSetMouseButtonCallback(_window, InputMouse);
		glfwSetCursorPosCallback(_window, PositionMouse);
		glfwSetScrollCallback(_window, ScrollMouse);
	}

	~XRRenderingInfra()
	{
		if (!_window) {
			glfwTerminate();
		}
	}


private:
	GLFWwindow * _window;

private: // GLFW Events
	static void InputKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void InputMouse(GLFWwindow* window, int button, int action, int mods);
	static void PositionMouse(GLFWwindow* window, double xpos, double ypos);
	static void ScrollMouse(GLFWwindow* window, double xoffset, double yoffset);

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
//
//class XRRendererTest : public XRRenderer
//{
//	// Todo) 일단 렌더링 되는지 테스트해보려고 여기다 선언해서 그냥 써봄.
//	// 아래 멤버변수들은 그래픽스 파이프라인에서나 선언해다 쓰고
//	// 나중엔 Rendering stratagy와 pipeline 간의 관계를 정립한 뒤,
//	// 렌더링으로 연결될 수 있게 변경할 것.
//	GLuint _glProgram;
//	GLuint _glVertexShader;
//	GLuint _glFragmentShader;
//
//	struct UNIFORM_BINDING_NAME
//	{
//		enum : GLuint
//		{
//			Matrix,
//			Light,
//			Material,
//			Count
//		};
//	};
//
//	std::vector<GLuint> _uniformBuffers;
//
//	struct Light
//	{
//		float position[4];
//		float intensity[3];
//		float attenuation;
//	};
//
//	struct Material
//	{
//		float diffuse[4];
//	};
//
//public:
//	XRRendererTest();
//	~XRRendererTest();
//
//	void Initialize() override;
//
//	void Update() override;
//	void Render() override;
//};

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
