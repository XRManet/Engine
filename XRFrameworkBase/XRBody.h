#pragma once

#include "stdafx.h"

class XRBodyData;

class XRBaseExport XRBody
{
private:
  union {
    XRBodyData * _data;
  };
};