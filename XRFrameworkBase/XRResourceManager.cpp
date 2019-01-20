#include "stdafx.h"

#include "XRResourceManager.h"

#include "XRModel.h"

// Temporary include
#include "XRWavefrontObject.h"

XRResourceManager::XRResourceManager()
{
  // TODO) Parse manifest for resources and enroll those.
  XRModelData * loadable = new XRWavefrontObject("../Resources/teapot.obj");
  XRModel * model = new XRModel(loadable);

  _models["teapot"] = model;
}

std::vector<std::string>&& XRResourceManager::QueryListOfModels()
{
  std::vector<std::string> keys;

  keys.push_back("teapot");

  return std::move(keys);
}

XRModel* XRResourceManager::GetModelByKey(std::string&& key) noexcept
{
  if (_models.find(key) != _models.end()) return _models[key];
  else return nullptr;
}
