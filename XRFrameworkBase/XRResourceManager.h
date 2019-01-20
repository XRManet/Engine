#pragma once

#include "stdafx.h"

class XRModel;

class XRBaseExport XRResourceManager
{
private:
  std::unordered_map<std::string, XRModel*> _models;


public:
  XRResourceManager();


public:
  std::vector<std::string>&& QueryListOfModels();
  XRModel* GetModelByKey(std::string&& key) noexcept;
};