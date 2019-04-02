#include "stdafx.h"

#include "XRSceneMain.h"

#define COMPILE_CPP
#include "Shader/DeferredSample.h"

#include <XRFrameworkBase/XRObject.h>
#include <XRFrameworkBase/XRLogging.h>
#include <XRFrameworkBase/XRUtility.h>

#include <GL/glew.h>

XRCamera camera;

#include <stdio.h>

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

  constexpr GLuint var_test1 = XR::util::GetIndexOfLiteralStringList(g_deferredVar, "var_test1");

  auto data = std::make_tuple(1.5f);
  constexpr int value = XR::util::GetIndexOfLiteralStringList(g_programNameList, "program_name1");
  printf("value: %d\n", value);
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



#if 0
/**********************************
 * CommandSetter Test
 */
template<typename name_tuple, typename data_tuple>
struct BaseCommandSetter
{
  BaseCommandSetter(GLuint program, name_tuple& names, const char* name, data_tuple& data)
  {
    constexpr int index = XR::util::GetIndexOfLiteralStringList<name_tuple>(names, name);
    static int bindingLocation = glGetUniformLocation(program, name);
    glUniform1f(bindingLocation, std::get<index>(data));
  }
};

template<typename name_tuple, typename data_tuple, typename var_type>
void SetVariable(GLuint program, const name_tuple names, data_tuple data, const char* name, var_type value)
{
  int index = XR::util::GetIndexOfLiteralStringList<name_tuple>(names, name);
  static int bindingLocation = glGetUniformLocation(program, name);
  
  std::get<index>(data) = value;
  glUniform1f(bindingLocation, value);
}

void Test()
{
  GLuint program = 9;
  glm::mat4 mvp = glm::identity<glm::mat4>();

  auto data = std::make_tuple(0.1f, 5, mvp);

  SetVariable(program, g_deferredVar, data, "var_test1", 0.2f);

  //constexpr int index = XR::util::GetIndexOfLiteralStringList(g_deferredVar, "var_test1");
  //static int bindingLocation = glGetUniformLocation(program, "var_test1");
  //glUniform1f(bindingLocation, std::get<index>(data));
}
#endif