#pragma once

#include <stdafx.h>

class XRMaterial {};

class XRModelData;
// #include "ModelDataRepresentation.h"

class XRBaseExport XRModel
{
protected:
  union {
    XRModelData * _data;
    // Todo) Other types of data for its own rendering engine would be given.
    // To support them, the XRModel has to bind data dynamically.
    // The XRModel introduces common interfaces of model data to a XRObject.
  };


public:
  XRModel(XRModelData* data);
  virtual ~XRModel();
};

XRRenderExport XRModel* xrCreateModel(XRModelData * loadable);