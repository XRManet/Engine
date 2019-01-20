#include "stdafx.h"

#include "XRModel.h"

#include "XRModelData.h"

XRModel::XRModel(XRModelData* loadableData) : _data(loadableData)
{
  if (_data->LoadDataFromFile() == true);
  else throw loadableData;
}
