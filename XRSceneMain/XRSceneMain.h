#pragma once

#include "stdafx.h"

#include <XRFrameworkBase/XRScene.h>

class XRExport XRSceneMain :
  public XRScene
{
public:
  XRSceneMain();
  virtual ~XRSceneMain();


public:
  virtual void Update(float dt) override;
};

extern "C" XRExport XRScene* CreateSceneMain()
{
  return new XRSceneMain;
}