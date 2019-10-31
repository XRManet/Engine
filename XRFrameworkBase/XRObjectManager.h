#pragma once

#include "stdafx.h"

class XRObject;

class XRBaseExport XRObjectManager
{
private:
  std::vector<XRObject*> _instanced_objects;


public:
  XRObjectManager() { _instanced_objects.reserve(PAGE_SIZE / sizeof(XRObject*)); }


public:
  std::vector<XRObject*> GenerateObjects(int count);
};