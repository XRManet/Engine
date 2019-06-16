#include "stdafx.h"
#include "XRPlatform.h"

#include "XRModel.h"
XRModel* (*xrCreateModel)(XRModelData* loadable) = nullptr;

static struct XRRenderEngineLinker
{
  XRPlatform::XRDSO _dso = nullptr;

  XRRenderEngineLinker()
  {
    static const char* renderEngineName = XRRENDER_ENGINE;
    _dso = XRPlatform::LoadDSO(renderEngineName);
    assert(_dso != nullptr);

    // Todo) List up and check availability
    std::vector<std::string> dllLists;
    XRPlatform::ListDLLFunctions(_dso, dllLists);

    GetProcAddress(xrCreateModel, "xrCreateModel");
  }

  ~XRRenderEngineLinker()
  {
    XRPlatform::UnloadDSO(_dso);
  }

private:
  template<typename func>
  void GetProcAddress(func& outFunction, const char* name)
  {
    outFunction = static_cast<func>(XRPlatform::GetProcAddress(_dso, name));
    assert(outFunction != nullptr);
  }

} EngineLinkerStarter;