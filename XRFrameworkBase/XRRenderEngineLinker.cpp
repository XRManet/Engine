#include "stdafx.h"
#include "XRPlatform.h"

#include "XRModel.h"
XRModel* (*xrCreateModel)(XRModelData const* loadable) = nullptr;

#include "XRTexture.h"
XRTexture* (*xrCreateTexture)(XRTextureData const* loadable) = nullptr;

#ifndef XRRENDER_ENGINE
#define XRRENDER_ENGINE_DEFAULT     "XRRenderEngineingGL"
#define XRRENDER_ENGINE             XRRENDER_ENGINE_DEFAULT
#endif

static struct XRRenderEngineLinker
{
  XRPlatform::XRDSO* _dso;

  XRRenderEngineLinker()
  {
    static const char* renderEngineName = XRRENDER_ENGINE;
    _dso = XRPlatform::LoadDSO(renderEngineName);
    assert(_dso != nullptr);

    // Todo) List up and check availability
    std::vector<std::string> dllLists;
    XRPlatform::ListDLLFunctions(_dso, dllLists);

    GetProcAddress(xrCreateModel, "xrCreateModel");
	GetProcAddress(xrCreateTexture, "xrCreateTexture");
  }

  ~XRRenderEngineLinker()
  {
    XRPlatform::UnloadDSO(_dso);
  }

private:
  template<typename func>
  void GetProcAddress(func& outFunction, const char* name)
  {
    outFunction = reinterpret_cast<func>(XRPlatform::GetProcAddress(_dso, name));
    assert(outFunction != nullptr);
  }

} EngineLinkerStarter;
