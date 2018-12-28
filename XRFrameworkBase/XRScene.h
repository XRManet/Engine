#pragma once

#include "stdafx.h"


class XRMaterial {};
class XRObject {};

class XRObjectManager
{
public:
};

class XRBaseExport XRScene
{
public:
  XRScene();
  virtual ~XRScene();


public:
  virtual void Update(float dt) {}
  virtual void Render(float dt) {}


public:
  virtual void Prepare() {}
  virtual void Finalize() {}
};

using XRPFNCREATESCENE = XRScene * (*)();

struct XRBaseExport XRSceneInfo
{
  char const* dso_name = nullptr;

  XRPFNCREATESCENE CreateScene = nullptr;


public:
  inline bool IsAvailable() {
    return dso_name != nullptr
      && CreateScene != nullptr;
  }


public:
  inline bool operator == (XRSceneInfo const& scene) {
    return dso_name == scene.dso_name
      && CreateScene == scene.CreateScene;
  }

  inline bool operator == (XRSceneInfo&& scene) {
    return (*this == scene);
  }

  inline bool operator != (XRSceneInfo const& scene) {
    return !(*this == scene);
  }

  inline bool operator != (XRSceneInfo&& scene) {
    return !(*this == scene);
  }
};