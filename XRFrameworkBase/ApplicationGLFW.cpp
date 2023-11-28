#include "stdafx.h"

#include <XRFrameworkBase/GLFWBinder.h>

#include <XRFrameworkBase/ApplicationGLFW.h>
#include <XRFrameworkBase/ThreadWin32.h>

#include "WindowWin32.h"
#include <XRFrameworkBase/Application.h>

namespace xr
{

	struct {
		bool _useMultiThreadEventProcessing = false;
	} g_win32Options;

	ApplicationGLFW::ApplicationGLFW(Application* application)
		: ApplicationPlatform(application)
	{
		static const uint32_t versions[][2] = {
			{4, 6}, {4, 5}, {4, 3}, {4, 1}, {4, 0},
			{3, 3}, {3, 0},
			{2, 1}, {2, 0},
			{1, 1}, {1, 0}
		};
		uint32_t version_try = 0;

		if (glfwInit() == 0)
			throw;

		if (glfwVulkanSupported())
		{
			printf("Vulkan available after getInstanceProcAddress(vkCreateInstance)\n");
		}

		const bool useOpenGL = false;
		if (useOpenGL)
		{
			GLFWwindow* contextWindowForInit = nullptr;
			while (1)
			{
				static const uint32_t max_try = static_cast<uint32_t>(sizeof(versions) / sizeof(versions[0]));
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

				contextWindowForInit = glfwCreateWindow(256, 256, "init", NULL, NULL);

				if (contextWindowForInit != nullptr)
				{
					printf("OpenGL version: ");
					if (version_try <= max_try)
						printf("%d.%d\n", versions[version_try][0], versions[version_try][1]);
					else
						printf("system-default\n");
					
					glfwDestroyWindow(contextWindowForInit);
					break;
				}
				else
				{
					++version_try;
				}
			}
		}

	}

	ApplicationGLFW::~ApplicationGLFW()
	{
		glfwTerminate();
	}

	void ApplicationGLFW::waitForAllThreads()
	{
		const auto& allThreads = getApplication()->getAllThreads();
		std::vector<HANDLE> threadHandles;
		threadHandles.reserve(allThreads.size());

		for (auto& thread : allThreads)
		{
			threadHandles.push_back(static_cast<ThreadWin32*>(thread)->getThreadHandle());
		}

		::WaitForMultipleObjects(static_cast<DWORD>(threadHandles.size()), threadHandles.data(), TRUE, INFINITE);
	}

} // namespace xr