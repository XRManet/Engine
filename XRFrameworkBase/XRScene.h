#pragma once

#include "stdafx.h"

// TODO) File Manager
class XRLoadable
{
private:
  std::string _path;


protected:
  std::vector<unsigned char> _memory;


public:
  XRLoadable(std::string && path);
  XRLoadable(std::string const & path);


protected:
  std::string const & GetPath() const { return _path; }


public:
  virtual bool LoadDataFromFile() { return false; }

public:
  
};

class XRModelData : public XRLoadable
{
public:
  struct VertexPosition { float x, y, z, w; };
  struct VertexNormal { float x, y, z; };
  struct TextureCoordinate { float u, v, w; };
  struct VertexParameter { float u, v, w; };
  struct FaceElement { int vertex, texture, normal; };
  struct FaceIndexTriangle { int v0, v1, v2; };
  struct FaceIndexQuad { int v0, v1, v2, v3; };
  struct ModelHeader
  {
    size_t vertex_offset = 0;
    size_t normal_offset = 0;
    size_t texture_offset = 0;
    size_t index_offset = 0;

    int vertex_count = 0;
    int normal_count = 0;
    int texture_count = 0;
    int index_count = 0;

    int primitive_type = 0;
    int vertex_number_in_face = 0;
  };


public:
  XRModelData(std::string && path) : XRLoadable(std::move(path)) { Initialize(); }
  XRModelData(std::string const& path) : XRLoadable(path) { Initialize(); }

private:
  void Initialize();


protected:
  ModelHeader * GetHeader() { return reinterpret_cast<ModelHeader*>(_memory.data()); }

public:
  ModelHeader const* GetHeader() const { return reinterpret_cast<ModelHeader const*>(_memory.data()); }
};

class XRWavefrontObject : public XRModelData
{
public:
  union ReadUnit {
    float f[4];
    int i[4];
  };


public:
  XRWavefrontObject(std::string && path) : XRModelData(std::move(path)) {}
  XRWavefrontObject(std::string const & path) : XRModelData(path) {}
  virtual ~XRWavefrontObject() {}


public:
  bool LoadDataFromFile() override;
};

class XRWavefrontMaterial : public XRLoadable
{
public:
public:
  bool LoadDataFromFile() override {}
};

class XRSavable
{
public:
  virtual bool SaveDataIntoFile() { return false; }
};

class XRMaterial {};
class XRTexture {};
class XRTextureAtlas : public XRTexture {};

class XRModel
{
private:
  XRModelData * _data;

public:
  static XRModel* GenerateModelFromData(XRModelData* loadableData);


private:
  XRModel(XRModelData* data);
};

class XRObject
{
private:
  XRModel * _model;

public:
  XRObject() {}
};

class XRObjectManager
{
private:
  std::vector<XRObject*> _instanced_objects;


public:
  XRObjectManager() {}


public:
};

class XRResourceManager
{
private:
  std::unordered_map<std::string, XRModel*> _models;


public:
  XRResourceManager();
};

class XRBaseExport XRScene
{
protected:
  XRObjectManager _object_manager;
  XRResourceManager _resource_manager;


public:
  XRScene();
  virtual ~XRScene();


public:
  virtual void Update(float dt) {}


public:
  virtual void PrepareToWakeUp() {}
  virtual void ReadyToBePaused() {}
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