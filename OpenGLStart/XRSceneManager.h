#pragma once

#include <vector>
#include <map>
#include <string>

#include "stdafx.h" // XRPlatform

#include <XRFrameworkBase/XRScene.h>

namespace XRPlatform
{
  typedef HMODULE XRDSO;

  XRDSO LoadDSO(char const* dso_name);
  void UnloadDSO(XRDSO dso);

  void* GetProcAddress(XRDSO dso, char const* proc);
};


class XRScene;
struct XRSceneInfo;

class XRSceneManager
{
private:
  std::map<std::string, XRSceneInfo> _list_of_scenes;
  std::map<std::string, XRPlatform::XRDSO> _list_of_dsos;

  XRScene* _primary_scene = nullptr;


private:
  XRSceneManager();
  ~XRSceneManager();


public:
  static XRSceneManager* GetInstance() {
    static XRSceneManager _instance;

    return &_instance;
  }

  static char const* MANIFEST_NAME;
  

public:
  void QueryScenes(std::string&& query);


public:
  void BindPrimaryScene(std::string&& key);

public:
  XRScene * GetPrimaryScene() { return _primary_scene; }
};