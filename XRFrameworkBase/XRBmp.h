#pragma once

#include "stdafx.h"

#include "XRTextureData.h"

class XRBmp : public XRTextureData
{
public:
	XRBmp(std::string && path) : XRTextureData(std::move(path)) {}
	XRBmp(std::string const & path) : XRTextureData(path) {}
	virtual ~XRBmp() {}


public:
	bool LoadDataFromFile() override;
};