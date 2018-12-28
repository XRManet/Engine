#pragma once

#include "stdafx.h"

#if defined(_M_X64) || defined(_M_AMD64)
struct XRBaseExport XRSize {
  float _width;
  float _height;
};

struct XRBaseExport XRPosition {
  float _x;
  float _y;
};

struct XRBaseExport XRBound {
  union {
    struct {
      XRPosition _position;
      XRSize _size;
    };
    struct {
      float _x;
      float _y;
      float _width;
      float _height;
    };
  };
};
#else
#error "TODO: XRGeometry is not defined for all type of architectures but only x64 or amd64"
#endif
