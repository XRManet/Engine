#include "stdafx.h"
#include "XRLoopManager.h"

#include <XRFrameworkBase/XRScene.h>
#include "XRSceneManager.h"

#define NEXT_ALIGN_2(offset, size_2) ((offset + size_2 - 1) & ~(size_2 - 1))

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

const char* GetGlMeaningDebugType(GLenum debugType)
{
#if defined(GL_KHR_debug)
	switch (debugType)
	{
	case GL_DEBUG_TYPE_ERROR:				return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	return "Deprecated behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	return "Undefined behavior";
	case GL_DEBUG_TYPE_PORTABILITY:			return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE:			return "Performance";
	case GL_DEBUG_TYPE_OTHER:				return "Other";
	case GL_DEBUG_TYPE_MARKER:				return "Marker";
	case GL_DEBUG_TYPE_PUSH_GROUP:			return "Push group";
	case GL_DEBUG_TYPE_POP_GROUP:			return "Pop group";
	}
#endif
	return "<ERROR>";
}

const char* GetGlMeaningSeverity(GLenum severity)
{
#if defined(GL_KHR_debug)
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:	return "NOTIFICATION";
	case GL_DEBUG_SEVERITY_HIGH:			return "HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM:			return "MEDIUM";
	case GL_DEBUG_SEVERITY_LOW:				return "LOW";
	}
#endif
	return "<ERROR>";
}

void GLAPIENTRY MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = %s, severity = %s, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		GetGlMeaningDebugType(type), GetGlMeaningSeverity(severity), message);
}

XRRenderingStratagyForward::XRRenderingStratagyForward()
{
  // TODO) Initialize()의 내용을 glew 초기화 특정되는 순간 할 수 있는게 좋음.
  // 1. Observer를 둬서 라이브러리 준비가 끝나면 Initialize()가 호출되도록,
  // 2. 아니면 람다를 등록해서 라이브러리 준비가 끝나면 그 즉시 호출되도록
}

XRRenderingStratagyForward::~XRRenderingStratagyForward()
{
}

static GLint MAX_UNIFORM_BUFFER_BINDINGS = 0;
static GLint MAX_UNIFORM_LOCATIONS = 0;
static GLint UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;

static GLint MAX_VARYING_COMPONENTS = 0;

static GLint MAX_TEXTURE_IMAGE_UNITS = 0;
static GLint MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0;
static GLint MAX_COMPUTE_TEXTURE_IMAGE_UNITS = 0;

GLuint UNIFORM_BINDING[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
};

void XRRenderingStratagyForward::Initialize()
{
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

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

  GL_CALL(glDeleteShader(_glVertexShader));
  GL_CALL(glDeleteShader(_glFragmentShader));
  
  glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &MAX_UNIFORM_BUFFER_BINDINGS);
  printf("Max uniform block bindings: %d\n", MAX_UNIFORM_BUFFER_BINDINGS);
  glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &MAX_UNIFORM_LOCATIONS);
  printf("Max uniform locations: %d\n", MAX_UNIFORM_LOCATIONS);
  glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UNIFORM_BUFFER_OFFSET_ALIGNMENT);
  printf("Uniform buffer offset alignment: %d\n", UNIFORM_BUFFER_OFFSET_ALIGNMENT);

  printf("\n");
  glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &MAX_VARYING_COMPONENTS);
  printf("Max varying components: %d\n", MAX_VARYING_COMPONENTS);

  printf("\n");
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_IMAGE_UNITS);
  printf("Max texture image units: %d\n", MAX_TEXTURE_IMAGE_UNITS);
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_COMBINED_TEXTURE_IMAGE_UNITS);
  printf("Max combined texture image units: %d\n", MAX_COMBINED_TEXTURE_IMAGE_UNITS);
  glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &MAX_COMPUTE_TEXTURE_IMAGE_UNITS);
  printf("Max compute texture image units: %d\n", MAX_COMPUTE_TEXTURE_IMAGE_UNITS);

  printf("=============================================\n\n");

  glGenBuffers(UNIFORM_BUFFER_NAME::Count, _uniformBuffers);
}

void XRRenderingStratagyForward::Render()
{
  glClearColor(1, 1, 1, 1);

  glClear(GL_COLOR_BUFFER_BIT);

  GL_CALL(glUseProgram(_glProgram));
  
  //GLuint uniformTransformView = -1;
  //GLuint uniformTransformVP = -1;
  GLuint uniformIndexTransform = -1;

  //GL_CALL(uniformTransformView = glGetUniformLocation(_glProgram, "transformView"));
  //GL_CALL(uniformTransformVP = glGetUniformLocation(_glProgram, "transformVP"));

  GLint countGlobalMaxUniformBlockVariables = 0;
  GLint numGlobalMaxUniformBlockVariables = 0;
  GLint numUniformBlocks = 0;
  std::vector<GLint> uniformBlockCount;
  GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numUniformBlocks));
  printf("Number of uniform block resources: %d\n", numUniformBlocks);
  GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM_BLOCK, GL_MAX_NUM_ACTIVE_VARIABLES, &numGlobalMaxUniformBlockVariables));
  printf("Max number of uniform block variables: %d\n", numGlobalMaxUniformBlockVariables);
  GLint numActiveUniform = 0;
  GL_CALL(glGetProgramInterfaceiv(_glProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniform));
  printf("Number of uniform resources:  %d\n", numActiveUniform);

  GLenum props[3] = {
    GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES
  };
  GLsizei length;
  GLint resultParams[64] = { -1, -1 };
  for (GLint i = 0; i < numUniformBlocks; ++i)
  {
    const GLsizei bufSize = 256;

    GLchar blockName[bufSize];
    GL_CALL(glGetProgramResourceName(_glProgram, GL_UNIFORM_BLOCK, i, bufSize, nullptr, blockName));
    printf("Uniform Block Name: %s, (%d/%d)\n", blockName, i, numUniformBlocks);

    glGetProgramResourceiv(_glProgram, GL_UNIFORM_BLOCK, i, 1, props, 1, &length, resultParams);

    glGetProgramResourceiv(_glProgram, GL_UNIFORM_BLOCK, i, 2, props, 64, &length, resultParams);
    glGetProgramResourceiv(_glProgram, GL_UNIFORM_BLOCK, i, 2, props, bufSize, &length, resultParams);

    GLchar uniformName[bufSize];
    GLint numActiveVariables = resultParams[0];
    countGlobalMaxUniformBlockVariables += numActiveVariables;
    printf("Uniform NumActiveVariables: %d\n", numActiveVariables);
    for (GLint activeVariableIndex = resultParams[1]; activeVariableIndex < countGlobalMaxUniformBlockVariables; ++activeVariableIndex)
    {
      GL_CALL(glGetActiveUniformName(_glProgram, activeVariableIndex, bufSize, nullptr, uniformName));
      printf("\tUniform [%d] Name: %s\n", activeVariableIndex, uniformName);
    }
  }

  for (GLint i = 0; i < numActiveUniform; ++i)
  {
    GLchar uniformName[5][256];
    //glGetActiveUniformsiv(_glProgram, numActiveUniform, );
    //glGetActiveUniformName(_glProgram, )
  }

  //glGetProgramResourceIndex(_glProgram, );
  //glGetProgramResourceiv(_glProgram, GL_UNIFORM, );
  GL_CALL(uniformIndexTransform = glGetUniformBlockIndex(_glProgram, "MatrixBlock"));
  assert(uniformIndexTransform == glGetProgramResourceIndex(_glProgram, GL_UNIFORM_BLOCK, "MatrixBlock"));

  GLint uniformTransformNum = 0;
  glGetActiveUniformBlockiv(_glProgram, uniformIndexTransform, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformTransformNum);
  std::vector<GLint> uniformTransformIndices(uniformTransformNum);
  glGetActiveUniformBlockiv(_glProgram, uniformIndexTransform, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformTransformIndices.data());
  GLint uniformTransformSizes = 0;
  glGetActiveUniformBlockiv(_glProgram, uniformIndexTransform, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformTransformSizes);
  const GLsizei blockNameSize = 256;
  GLchar blockName[blockNameSize];
  glGetActiveUniformBlockName(_glProgram, uniformIndexTransform, blockNameSize, nullptr, blockName);
  
  GL_CALL(glUniformBlockBinding(_glProgram, uniformIndexTransform, UNIFORM_BINDING[UNIFORM_BUFFER_NAME::Transform]));

  GLint currentBindingPoint = -1;
  GL_CALL(glGetActiveUniformBlockiv(_glProgram, uniformIndexTransform, GL_UNIFORM_BLOCK_BINDING, &currentBindingPoint));

  GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BINDING[UNIFORM_BUFFER_NAME::Transform], _uniformBuffers[UNIFORM_BUFFER_NAME::Transform]));

  GLuint offset = 0;
  GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, UNIFORM_BINDING[UNIFORM_BUFFER_NAME::Transform], _uniformBuffers[UNIFORM_BUFFER_NAME::Transform], offset, uniformTransformSizes));
  offset = NEXT_ALIGN_2(offset + uniformTransformSizes, UNIFORM_BUFFER_OFFSET_ALIGNMENT);

  GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, UNIFORM_BINDING[UNIFORM_BUFFER_NAME::Light], _uniformBuffers[UNIFORM_BUFFER_NAME::Light], offset, sizeof(Material)));
  offset = NEXT_ALIGN_2(offset + sizeof(Material), UNIFORM_BUFFER_OFFSET_ALIGNMENT);

  //glUniformMatrix4fv(uniformTransformVP, 1, false, );
  //glUniformBufferEXT(_glProgram, uniformLight, );

  //GL_CALL(glDrawElementsInstanced(GL_TRIANGLES, )_;

  printf("\n");
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