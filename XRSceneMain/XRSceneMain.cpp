#include "stdafx.h"

#include "XRSceneMain.h"


XRSceneMain::XRSceneMain()
{
  auto model = _resource_manager.GetModelByKey("teapot");
  _object_manager.GenerateObjects(model, 5);


}


XRSceneMain::~XRSceneMain()
{
}

void XRSceneMain::Update(float dt)
{
}