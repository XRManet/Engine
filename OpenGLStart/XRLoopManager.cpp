#include "stdafx.h"
#include "XRLoopManager.h"

#include <XRFrameworkBase/XRScene.h>
#include "XRSceneManager.h"

void XRRenderingStratagyForward::Render()
{
  glClearColor(1, 1, 1, 1);

  glClear(GL_COLOR_BUFFER_BIT);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
 
  GLuint vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

XRFrameWalker::XRFrameWalker()
{
  // TODO) select a stratagy by reading from manifest.
  _rendering_stratagy.reset(new XRRenderingStratagyForward);
}

void XRFrameWalker::UpdateFrame()
{
  auto scene = XRSceneManager::GetInstance()->GetPrimaryScene();

  scene->Update(0);
  _rendering_stratagy->Render();
}