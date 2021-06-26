#pragma once

#include <XRFrameworkBase/XRDefault.h>

#include "XRModelData.h"

union ReadUnit {
	int i[4]{ 0, };
	float f[4];
};

struct XRWavefrontObjectMeshes;

class XRWavefrontObject : public XRModelDataLoader
{
public:
	XRWavefrontObject(std::string && path) : XRModelDataLoader(std::move(path)) {}
	XRWavefrontObject(std::string const & path) : XRModelDataLoader(path) {}
	virtual ~XRWavefrontObject() {}

	
public:
	bool LoadDataFromFile() override;

private:
	bool ProduceXRModelData(std::vector<XRWavefrontObjectMeshes>&);
};
