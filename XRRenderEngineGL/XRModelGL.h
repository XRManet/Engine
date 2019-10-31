#pragma once

#include <XRFrameworkBase/XRModel.h>

class XRInputLayoutGL : public XRInputLayout
{
private:
  unsigned _vao;

public:
  XRInputLayoutGL(XRModelData const* model);
  virtual ~XRInputLayoutGL();

public:
	void bind() const override;
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


public:
	void bind() const override;
};