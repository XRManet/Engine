#pragma once

#include <XRFrameworkBase/XRDefault.h>

class XRModel;
class XRInputLayout;

class XRBaseExport XRResourceManager
{
private:
	std::unordered_map<std::string, XRModel*> _modelsForKey;
	std::unordered_map<std::string, XRInputLayout const*> _inputLayoutsForCategory;


public:
	XRResourceManager();


public:
	std::vector<std::string>&& QueryListOfModels();
	XRModel* GetModelByKey(std::string&& key) noexcept;
	XRInputLayout const* GetInputLayoutByCategory(std::string&& category) noexcept;
};