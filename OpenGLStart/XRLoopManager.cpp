#include "stdafx.h"
#include "XRLoopManager.h"

#include <XRFrameworkBase/XRScene.h>
#include "XRSceneManager.h"

namespace
{
#define DEFAULT_RESOURCE_PATH "../Resources/Shaders/OpenGL/"
  constexpr size_t RESOURCE_PATH_END = sizeof(DEFAULT_RESOURCE_PATH) - 1;

  void CompileShader(GLuint shader, GLsizei fileCount, const char** filename)
  {
    char RESOURCE_PATH[256] = DEFAULT_RESOURCE_PATH;
    char** shaderSources = new char*[fileCount]();
    int* sizes = new int[fileCount]();

    for (GLsizei i = 0; i < fileCount; ++i)
    {
      strcpy(RESOURCE_PATH + RESOURCE_PATH_END, filename[i]);
      FILE* fp = fopen(RESOURCE_PATH, "r");
      
      assert(fp != nullptr);
      if (fp == nullptr) continue;

      bool result = (fseek(fp, 0, SEEK_END) == 0);
      if (result == true)
      {
        long size = ftell(fp);
        rewind(fp);

        char* buffer = new char[size + 1]();

        fread(buffer, sizeof(char), size, fp);
        
        shaderSources[i] = buffer;
        sizes[i] = size;
      }
      
      fclose(fp);
    }

    GL_CALL(glShaderSource(shader, fileCount, shaderSources, sizes));
    GL_CALL(glCompileShader(shader));

    GLint result = GL_FALSE;
    GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
      GLint logLength = 0;
      GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));

      if (logLength > 0)
      {
        GLchar* failedLog = new GLchar[logLength]();
        GL_CALL(glGetShaderInfoLog(shader, logLength, &logLength, failedLog));

        printf("%s", failedLog);
      }
    }

    for (GLsizei i = 0; i < fileCount; ++i)
    {
      delete shaderSources[i];
    }
    delete[] shaderSources;
    delete[] sizes;
  }

  void BuildProgram(GLuint glProgram, GLuint glShaders[], GLuint counts)
  {
    for (GLuint i = 0; i < counts; ++i)
    {
      GL_CALL(glAttachShader(glProgram, glShaders[i]));
    }

    GL_CALL(glLinkProgram(glProgram));

    GLint result = GL_FALSE;
    GL_CALL(glGetProgramiv(glProgram, GL_LINK_STATUS, &result));
    if (result == GL_FALSE)
    {
      GLint logLength = 0;
      GL_CALL(glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &logLength));

      if (logLength > 0)
      {
        GLchar* failedLog = new GLchar[logLength]();
        GL_CALL(glGetProgramInfoLog(glProgram, logLength, &logLength, failedLog));

        printf("%s", failedLog);
      }
    }

    for (GLuint i = 0; i < counts; ++i)
    {
      GL_CALL(glDetachShader(glProgram, glShaders[i]));
    }
  }
}

XRRenderingStratagyForward::XRRenderingStratagyForward()
{
  // TODO) Initialize()의 내용을 glew 초기화 특정되는 순간 할 수 있는게 좋음.
  // 1. Observer를 둬서 라이브러리 준비가 끝나면 Initialize()가 호출되도록,
  // 2. 아니면 람다를 등록해서 라이브러리 준비가 끝나면 그 즉시 호출되도록
}

void XRRenderingStratagyForward::Initialize()
{
  // TODO) separate
  GL_CALL(_glProgram = glCreateProgram());

  GL_CALL(_glVertexShader = glCreateShader(GL_VERTEX_SHADER));
  GL_CALL(_glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER));

  const char* vertexShaders[]{ "SimpleVertex.glsl" };
  CompileShader(_glVertexShader, std::size(vertexShaders), vertexShaders);

  const char* fragmentShaders[]{ "SimpleFragment.glsl" };
  CompileShader(_glFragmentShader, std::size(fragmentShaders), fragmentShaders);

  GLuint shaders[] = {
    _glVertexShader,
    _glFragmentShader
  };
  BuildProgram(_glProgram, shaders, std::size(shaders));
}

void XRRenderingStratagyForward::Render()
{
  glClearColor(1, 1, 1, 1);

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(_glProgram);

  glGetUniformLocation(_glProgram, "");
  

  //GL_CALL(glDrawElementsInstanced(GL_TRIANGLES, )_;
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