// XRRenderEngineGL.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "XRModelGL.h"

#include <assert.h>

struct RenderEngineGLInitializer
{
  RenderEngineGLInitializer()
  {
    // glewInit() must be called after makeCurrent-like functions
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

XRModel* xrCreateModel(XRModelData const* loadable)
{
  RenderEngineGLInitializer::GetInitializer();
  return new XRModelGL(loadable);
}