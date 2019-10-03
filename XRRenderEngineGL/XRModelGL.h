#pragma once

#include <XRFrameworkBase/XRModel.h>

class XRInputLayoutGL : public XRInputLayout
{
private:
  unsigned vao;

public:
  XRInputLayoutGL(XRModel* model);
  virtual ~XRInputLayoutGL();
};

class XRModelGL : public XRModel
{
private:
  union {
    unsigned vbo[2];
    struct {
      unsigned vertex;
      unsigned index;
    };
  } GL;

public:
  XRModelGL(XRModelData const* data);
  virtual ~XRModelGL();
};