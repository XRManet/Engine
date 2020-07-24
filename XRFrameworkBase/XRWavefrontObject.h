#pragma once

#include "stdafx.h"

#include "XRModelData.h"

class XRWavefrontObject : public XRModelDataLoader
{
public:
	union ReadUnit {
		int i[4] { 0, };
		float f[4];
	};


public:
	XRWavefrontObject(std::string && path) : XRModelDataLoader(std::move(path)) {}
	XRWavefrontObject(std::string const & path) : XRModelDataLoader(path) {}
	virtual ~XRWavefrontObject() {}

	
public:
	bool LoadDataFromFile() override;
};
