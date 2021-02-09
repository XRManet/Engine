// XRRHIOpenGL.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "XRModelGL.h"
#include "XRTextureGL.h"
#include "XRBufferGL.h"
#include "XRPipelineGL.h"
#include "XRCommandBufferGL.h"
#include "XRRenderGroupGL.h"

#include <assert.h>

struct RHIOpenGLInitializer
{
	RHIOpenGLInitializer()
  {
    // glewInit() must be called after makeCurrent-like functions
    glewExperimental = GL_TRUE;
    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
      assert(false && "It must be called after make context current.");
      throw;
    }
  }

  static RHIOpenGLInitializer& GetInitializer()
  {
    static RHIOpenGLInitializer __init;
    return __init;
  }
};

XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)
{
	RHIOpenGLInitializer::GetInitializer();
	return new XRInputLayoutGL(std::move(inputLayoutDesc), preferredStrideSize);
}

XRModel* xrCreateModel(XRModelData const* loadable)
{
	RHIOpenGLInitializer::GetInitializer();
  return new XRModelGL(loadable);
}

XRTexture* xrCreateTexture(XRTextureCreateInfo const* createInfo)
{
	RHIOpenGLInitializer::GetInitializer();
	auto textureGL = new XRTextureGL;
	auto textureHandle = new XRTexture(createInfo);
	textureHandle->_rhi = textureGL;
	return textureHandle;
}

XRTexture* xrCreateTextureFromData(XRTextureData const* loadable)
{
	RHIOpenGLInitializer::GetInitializer();
	auto textureGL = new XRTextureGL;
	if (nullptr != loadable)
		textureGL->upload(loadable);

	auto textureHandle = new XRTexture(loadable);
	textureHandle->_rhi = textureGL;
	return textureHandle;
}

XRBuffer* xrCreateBuffer(XRBufferCreateInfo const* createInfo)
{
	RHIOpenGLInitializer::GetInitializer();
	auto bufferGL = new XRBufferGL;
	auto bufferHandle = new XRBuffer(createInfo);
	bufferHandle->_rhi = bufferGL;
	return bufferHandle;
}

XRPipeline* xrCreatePipeline(XRPipelineStateDescription const* description)
{
	RHIOpenGLInitializer::GetInitializer();
	return new XRPipelineGL(description);
}

XRCommandBuffer* xrCreateCommandBuffer()
{
	RHIOpenGLInitializer::GetInitializer();
	return new XRCommandBufferGL;
}

XRRenderGroup* xrCreateRenderGroup()
{
	RHIOpenGLInitializer::GetInitializer();
	return new XRRenderGroupGL;
}
