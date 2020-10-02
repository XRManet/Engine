// XRRenderEngineGL.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "XRModelGL.h"
#include "XRTextureGL.h"
#include "XRPipelineGL.h"
#include "XRCommandBufferGL.h"
#include "XRRenderGroupGL.h"

#include <assert.h>

struct RenderEngineGLInitializer
{
  RenderEngineGLInitializer()
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

  static RenderEngineGLInitializer& GetInitializer()
  {
    static RenderEngineGLInitializer __init;
    return __init;
  }
};

XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)
{
	RenderEngineGLInitializer::GetInitializer();
	return new XRInputLayoutGL(std::move(inputLayoutDesc), preferredStrideSize);
}

XRModel* xrCreateModel(XRModelData const* loadable)
{
  RenderEngineGLInitializer::GetInitializer();
  return new XRModelGL(loadable);
}

XRTexture* xrCreateTexture(XRTextureData const* loadable)
{
	RenderEngineGLInitializer::GetInitializer();
	return new XRTextureGL(loadable);
}

XRPipeline* xrCreatePipeline(XRPipelineDescriptor const* descriptor)
{
	RenderEngineGLInitializer::GetInitializer();
	return new XRPipelineGL(descriptor);
}

XRCommandBuffer* xrCreateCommandBuffer()
{
	RenderEngineGLInitializer::GetInitializer();
	return new XRCommandBufferGL;
}

XRRenderGroup* xrCreateRenderGroup()
{
	RenderEngineGLInitializer::GetInitializer();
	return new XRRenderGroupGL;
}
