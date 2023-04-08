// XRRenderEngineGL.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include <XRFrameworkBase/XRRenderEngine.h>

#include "XRModelGL.h"
#include "XRTextureGL.h"
#include "XRBufferGL.h"
#include "XRPipelineGL.h"
#include "XRCommandBufferGL.h"
#include "XRRenderGroupGL.h"

#include <assert.h>

const char* GetGlMeaningDebugType(GLenum debugType)
{
#if defined(GL_KHR_debug)
	switch (debugType)
	{
	case GL_DEBUG_TYPE_ERROR:               return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined behavior";
	case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
	case GL_DEBUG_TYPE_OTHER:               return "Other";
	case GL_DEBUG_TYPE_MARKER:              return "Marker";
	case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push group";
	case GL_DEBUG_TYPE_POP_GROUP:           return "Pop group";
	}
#endif
	return "<ERROR>";
}

const char* GetGlMeaningSeverity(GLenum severity)
{
#if defined(GL_KHR_debug)
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:    return "NOTIFICATION";
	case GL_DEBUG_SEVERITY_HIGH:            return "HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM:          return "MEDIUM";
	case GL_DEBUG_SEVERITY_LOW:             return "LOW";
	}
#endif
	return "<ERROR>";
}

void GLAPIENTRY MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	constexpr size_t defaultBufferSize = 1024;
	static thread_local GLchar defaultBuffer[defaultBufferSize];
	static thread_local struct {
		GLchar* _memory = defaultBuffer;
		size_t _size = defaultBufferSize;
	} messageBuffer;

	constexpr const char decoratedMessage[] = "GL CALLBACK - %s\nType: %s\nSeverity: %s\nMessage: ";
	constexpr const char decoratedMessage2[] = "** GL ERROR **";
	constexpr size_t decoratedMessageSize = sizeof(decoratedMessage) + sizeof(decoratedMessage2) + 64ull;
	
	size_t requiredBufferSize = length + decoratedMessageSize;
	if (messageBuffer._size < requiredBufferSize)
	{
		if (messageBuffer._memory != defaultBuffer)
			delete[] messageBuffer._memory;

		size_t size = (requiredBufferSize > defaultBufferSize) ? requiredBufferSize : defaultBufferSize;
		messageBuffer._memory = new GLchar[size]();
		messageBuffer._size = size;
	}

	int messageOffset = sprintf_s(messageBuffer._memory, messageBuffer._size, "GL CALLBACK - %s\nType: %s\nSeverity: %s\nMessage: ",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		GetGlMeaningDebugType(type), GetGlMeaningSeverity(severity));
	strncpy_s(messageBuffer._memory + messageOffset, messageBuffer._size - messageOffset, message, length);
	messageBuffer._memory[messageOffset + length + 0] = '\n';
	messageBuffer._memory[messageOffset + length + 1] = '\n';
	messageBuffer._memory[messageOffset + length + 2] = '\0';
	OutputDebugStringA(messageBuffer._memory);
	
	assert(type != GL_DEBUG_TYPE_ERROR);
}

GLint MAX_UNIFORM_BUFFER_BINDINGS = 0;
GLint MAX_UNIFORM_LOCATIONS = 0;
GLint UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
GLint MAX_UNIFORM_BLOCK_SIZE = 0;

GLint MAX_VARYING_COMPONENTS = 0;

GLint MAX_TEXTURE_IMAGE_UNITS = 0;
GLint MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0;
GLint MAX_COMPUTE_TEXTURE_IMAGE_UNITS = 0;

template<>
struct RenderEngineInitializer<OpenGL>
{
	RenderEngineInitializer()
	{
		// glewInit() must be called after makeCurrent-like functions
		glewExperimental = GL_TRUE;
		GLenum result = glewInit();
		if (result != GLEW_OK)
		{
			assert(false && "It must be called after make context current.");
			throw;
		}

		LogGLSystemInfo();
	}

	static RenderEngineInitializer& GetInitializer()
	{
		static RenderEngineInitializer __init;
		return __init;
	}

	void LogGLSystemInfo()
	{
		assert(glGetError() == GL_NO_ERROR);

		if (glfwExtensionSupported("GL_KHR_debug") == GLFW_TRUE)
		{
			GL_CALL(glEnable(GL_DEBUG_OUTPUT));

			if (glfwGetProcAddress("glDebugMessageCallback"))
				glDebugMessageCallback(MessageCallback, 0);
		}

		if (glfwExtensionSupported("GL_ARB_vertex_attrib_binding") == GLFW_TRUE)
		{
		}

		printf("\n=============================================\n");
		printf("Query OpenGL System Informations\n\n");

		GL_CALL_WARN(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &MAX_UNIFORM_BUFFER_BINDINGS));
		printf("Max uniform block bindings: %d\n", MAX_UNIFORM_BUFFER_BINDINGS);
		GL_CALL_WARN(glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &MAX_UNIFORM_LOCATIONS));
		printf("Max uniform locations: %d\n", MAX_UNIFORM_LOCATIONS);
		GL_CALL_WARN(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UNIFORM_BUFFER_OFFSET_ALIGNMENT));
		printf("Uniform buffer offset alignment: %d\n", UNIFORM_BUFFER_OFFSET_ALIGNMENT);
		GL_CALL_WARN(glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &MAX_UNIFORM_BLOCK_SIZE));
		printf("Max size of an uniform block: %d\n", MAX_UNIFORM_BLOCK_SIZE);

		printf("\n");
		GL_CALL_WARN(glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &MAX_VARYING_COMPONENTS));
		printf("Max varying components: %d\n", MAX_VARYING_COMPONENTS);

		printf("\n");
		GL_CALL_WARN(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_IMAGE_UNITS));
		printf("Max texture image units: %d\n", MAX_TEXTURE_IMAGE_UNITS);
		GL_CALL_WARN(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_COMBINED_TEXTURE_IMAGE_UNITS));
		printf("Max combined texture image units: %d\n", MAX_COMBINED_TEXTURE_IMAGE_UNITS);
		GL_CALL_WARN(glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &MAX_COMPUTE_TEXTURE_IMAGE_UNITS));
		printf("Max compute texture image units: %d\n", MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
	}
};

XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	return new XRInputLayoutGL(std::move(inputLayoutDesc), preferredStrideSize);
}

XRModel* xrCreateModel(XRModelData const* loadable)
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	return new XRModelGL(loadable);
}

XRTexture* xrCreateTexture(XRTextureCreateInfo const* createInfo)
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	auto textureGL = new XRTextureGL;
	auto textureHandle = new XRTexture(createInfo);
	textureHandle->_rhi = textureGL;
	return textureHandle;
}

XRTexture* xrCreateTextureFromData(XRTextureData const* loadable)
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	auto textureGL = new XRTextureGL;
	if (nullptr != loadable)
		textureGL->upload(loadable);

	auto textureHandle = new XRTexture(loadable);
	textureHandle->_rhi = textureGL;
	return textureHandle;
}

XRBuffer* xrCreateBuffer(XRBufferCreateInfo const* createInfo)
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	auto bufferGL = new XRBufferGL;
	auto bufferHandle = new XRBuffer(createInfo, bufferGL);

	bufferGL->Initialize(bufferHandle);
	return bufferHandle;
}

XRPipeline* xrCreatePipeline(XRPipelineStateDescription const* description)
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	return new XRPipelineGL(description);
}

XRCommandBuffer* xrCreateCommandBuffer()
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	return new XRCommandBufferGL;
}

XRRenderGroup* xrCreateRenderGroup()
{
	RenderEngineInitializer<OpenGL>::GetInitializer();
	return new XRRenderGroupGL;
}
