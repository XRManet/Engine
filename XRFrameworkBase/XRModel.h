#pragma once

#include "stdafx.h"

class XRMaterial {};

class XRModelData;

class XRModel
{
private:
  XRModelData * _data;


public:
  XRModel(XRModelData* data);
};
