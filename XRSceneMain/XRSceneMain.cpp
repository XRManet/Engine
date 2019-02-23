#include "stdafx.h"

#include "XRSceneMain.h"

#include <XRFrameworkBase/XRObject.h>
#include <XRFrameworkBase/XRLogging.h>

XRCamera camera;

// Todo) Menifest�� ������ �ּ� scene���� ���� �� �ִ� ������ ���� ��
XRSceneMain::XRSceneMain()
{
  auto model = _resource_manager.GetModelByKey("teapot");
  auto objects = _object_manager.GenerateObjects(5);

  for (auto object : objects) {
    object->BindModel(model);
  }

  camera.SetFrustum({16, 9}, 10, 100);
  camera.SetPosition(glm::vec4 { 0, 0, 10, 1 });
  camera.SetQuaternion({});
}


XRSceneMain::~XRSceneMain()
{
}

void XRSceneMain::Update(float dt)
{
  //camera.Move({});
  //camera.Rotate(0, {});

  glm::mat4 viewing = camera.GetInvTransform();
  glm::mat4 projection = camera.GetProjectionTransform();

  glm::mat4 transform_vp = projection * viewing;
}