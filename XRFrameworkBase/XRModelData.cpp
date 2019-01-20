#include "stdafx.h"

#include "XRModelData.h"

void XRModelData::Initialize()
{
  _memory.reserve(PAGE_SIZE);

  _memory.resize(sizeof(ModelHeader));
  new (GetHeader()) ModelHeader;
}
