#pragma once

#include <stdafx.h>

class XRMaterial {};

class XRModelData;
// #include "ModelDataRepresentation.h"

class XRModel;

class XRBaseExport XRInputLayout
{
public:
  XRInputLayout() {}
  virtual ~XRInputLayout() {}
};

class XRBaseExport XRModel
{
protected:
  union {
    XRModelData * _data;
    // Todo) Other types of data for its own rendering engine would be given.
    // To support them, the XRModel has to bind data dynamically.
    // The XRModel introduces common interfaces of model data to a XRObject.
  };
  XRInputLayout* _inputLayout;

public:
  XRModel(XRModelData* data);
  virtual ~XRModel();
};

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRModel* xrCreateModel(XRModelData * loadable);
#else
extern XRModel* (*xrCreateModel)(XRModelData* loadable);
#endif