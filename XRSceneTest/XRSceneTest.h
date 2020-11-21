#pragma once

#include "stdafx.h"

#include <XRFrameworkBase/XRScene.h>

class /*XRExport */XRSceneTest :
  public XRScene
{
public:
  XRSceneTest();
  virtual ~XRSceneTest();


public:
  virtual void Update(float dt) override;
};

extern "C" XRExport XRScene* CreateSceneTest()
{
  return new XRSceneTest;
}
