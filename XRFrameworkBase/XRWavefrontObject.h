#pragma once

#include "stdafx.h"

#include "XRModelData.h"

class XRWavefrontObject : public XRModelData
{
public:
  union ReadUnit {
    float f[4];
    int i[4];
  };


public:
  XRWavefrontObject(std::string && path) : XRModelData(std::move(path)) {}
  XRWavefrontObject(std::string const & path) : XRModelData(path) {}
  virtual ~XRWavefrontObject() {}


public:
  bool LoadDataFromFile() override;
};
