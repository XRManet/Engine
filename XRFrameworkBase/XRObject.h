#pragma once

#include "stdafx.h"

class XRModel;

class XRBaseExport XRObject
{
private:
  XRModel * _model;

public:
  XRObject(XRModel* model) : _model(model) {}
};

