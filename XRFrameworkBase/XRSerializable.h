#pragma once

#include "stdafx.h"

class XRSerializable
{
public:
  virtual bool SerializeDataIntoFile() { return false; }
};
