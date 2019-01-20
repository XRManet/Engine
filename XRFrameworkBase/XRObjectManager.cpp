#include "stdafx.h"

#include "XRObjectManager.h"

#include "XRObject.h"

void XRObjectManager::GenerateObjects(XRModel * model, int count)
{
  for (int i = 0; i < count; ++i)
  {
    _instanced_objects.push_back(new XRObject(model));
  }
}
