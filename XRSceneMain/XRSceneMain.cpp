#include "stdafx.h"

#include "XRSceneMain.h"

#include <XRFrameworkBase/XRObject.h>
#include <XRFrameworkBase/XRLogging.h>
#include <XRFrameworkBase/XRUtility.h>

#include <GL/glew.h>

XRCamera camera;

#include <stdio.h>

template<int I, typename DataTuple>
struct BaseCommandSetter
{
  BasicCommandSetter(GLuint program, const char* name, DataTuple)
  {
    static int bindingLocation = glGetUniformLocation(program, name);
    glUniform1f(bindingLocation, v1);
  }
};

template<int I>
struct DerivedCommandSetter1 : public BaseCommandSetter<I>
{
  DerivedCommandSetter1()
  {
    
  }
};

template<int I>
struct DerivedCommandSetter2 : public BaseCommandSetter<I>
{
  DerivedCommandSetter2()
  {

  }
};

// Todo) Menifest를 별도로 둬서 scene에서 읽을 수 있는 형식을 갖출 것
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

  GLuint program1 = GetProgramByName("program_name1");
  GLuint program2 = GetProgramByName("program_name2");
  GLuint program3 = GetProgramByName("program_name3");
  GLuint program4 = GetProgramByName("program_name4");
  GLuint program5 = GetProgramByName("program_name5");

  
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