#pragma once

#include "stdafx.h"

class XRMaterial {};

class XRModelData;
// #include "ModelDataRepresentation.h"

class XRModel;

class XRBaseExport XRInputLayout
{
public:
  XRInputLayout() {}
  virtual ~XRInputLayout() {}

public:
	virtual void bind() const {}
};

class XRBaseExport XRModel
{
protected:
  union {
    XRModelData const* _data;
    // Todo) Other types of data for its own rendering engine would be given.
    // To support them, the XRModel has to bind data dynamically.
    // The XRModel introduces common interfaces of model data to a XRObject.
  };
  XRInputLayout* _inputLayout;

public:
  XRModel(XRModelData const* data);
  virtual ~XRModel();

public:
	virtual void bind() const {}
	void bindWithInputLayout() const
	{
		if(_inputLayout != nullptr)
			_inputLayout->bind();

		bind();
	}

	virtual uint32_t getNumVertices() const { return 0; }
	virtual uint32_t getNumIndices() const { return 0; }
};

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRModel* xrCreateModel(XRModelData const* loadable);
#else
extern XRModel* (*xrCreateModel)(XRModelData const* loadable);
#endif