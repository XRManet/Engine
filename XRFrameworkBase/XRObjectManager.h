#pragma once

#include "stdafx.h"

class XRObject;
class XRModel;

class XRBaseExport XRObjectManager
{
private:
  std::vector<XRObject*> _instanced_objects;


public:
  XRObjectManager() { _instanced_objects.reserve(PAGE_SIZE / sizeof(XRObject*)); }


public:
  void GenerateObjects(XRModel * model, int count);
};
