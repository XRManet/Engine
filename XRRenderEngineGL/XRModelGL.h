#pragma once

#include <XRFrameworkBase/XRModel.h>

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
  XRModelGL(XRModelData* data);
  virtual ~XRModelGL();
};