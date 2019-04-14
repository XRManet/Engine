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

// Desc) 설정된 경로로부터 얻을 수 있는 모든 모델에 대한 데이터를 획득한다.
// Todo) 경로를 설정하는 방법이 필요
// Todo) 경로 외의 모델 데이터 획득 방법 필요
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
