#pragma once

#include <XRFrameworkBase/XRDefault.h>

class XRBodyData;

class XRBaseExport XRBody
{
private:
  union {
    XRBodyData * _data;
  };
};