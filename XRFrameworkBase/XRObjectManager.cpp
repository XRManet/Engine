#include "stdafx.h"

#include "XRObjectManager.h"

#include "XRObject.h"

std::vector<XRObject*> XRObjectManager::GenerateObjects(int count)
{
  std::vector<XRObject*> objects;
  objects.reserve(count);

  for (int i = 0; i < count; ++i)
  {
    _instanced_objects.emplace_back(new XRObject);
    objects.push_back(_instanced_objects.back());
  }

  return objects;
}
