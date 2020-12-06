#include "stdafx.h"

#include "XRResourceManager.h"

#include "XRModel.h"
#include "XRModelData.h"

XRResourceManager::XRResourceManager()
{
	// TODO) Parse manifest for resources and enroll those.
	XRModelData* loadable = nullptr;
	XRModel* model = nullptr;

	loadable = new XRModelData("Resources/teapot.obj");
	loadable->LoadDataFromFile();
	model = xrCreateModel(loadable);
	_modelsForKey["teapot"] = model;
	_inputLayoutsForCategory["Static"] = model->getInputLayout();

	loadable = new XRModelData("Resources/Wolf_obj.obj");
	loadable->LoadDataFromFile();
	model = xrCreateModel(loadable);
	_modelsForKey["wolf"] = model;
	_inputLayoutsForCategory["Skeletal"] = model->getInputLayout();
}

// Desc) 설정된 경로로부터 얻을 수 있는 모든 모델에 대한 데이터를 획득한다.
// Todo) 경로를 설정하는 방법이 필요
// Todo) 경로 외의 모델 데이터 획득 방법 필요
std::vector<std::string>&& XRResourceManager::QueryListOfModels()
{
	std::vector<std::string> keys;

	keys.push_back("teapot");
	keys.push_back("wolf");

	return std::move(keys);
}

XRModel* XRResourceManager::GetModelByKey(std::string&& key) noexcept
{
	if (_modelsForKey.find(key) != _modelsForKey.end()) return _modelsForKey[key];
	else return nullptr;
}

XRInputLayout const* XRResourceManager::GetInputLayoutByCategory(std::string&& category) noexcept
{
	if (_inputLayoutsForCategory.find(category) != _inputLayoutsForCategory.end()) return _inputLayoutsForCategory[category];
	else return nullptr;
}