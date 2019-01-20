#pragma once

#include "stdafx.h"

class XRSavable
{
public:
  virtual bool SaveDataIntoFile() { return false; }
};
