#pragma once

#include "stdafx.h"

#include "XRObjectManager.h"
#include "XRResourceManager.h"

class XRCamera;

class XRBaseExport XRScene
{
protected:
  XRObjectManager _object_manager;
  XRResourceManager _resource_manager;
  std::vector<XRCamera> _cameras;

  std::unordered_map<std::string, XRObjectGroup> _object_groups;

public:
	std::vector<XRCamera>& getCameras() { return _cameras; }
	XRObjectGroup const* getObjectGroup(std::string groupName) const
	{
		auto it = _object_groups.find(groupName);
		assert(it != _object_groups.end());
		return &it->second;
	}


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