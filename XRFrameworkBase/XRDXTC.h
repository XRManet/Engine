#pragma once

#include "stdafx.h"

#include "XRTextureData.h"

class XRDXTC : public XRTextureData
{
public:
	XRDXTC(std::string && path) : XRTextureData(std::move(path)) {}
	XRDXTC(std::string const & path) : XRTextureData(path) {}
	virtual ~XRDXTC() {}


public:
	bool LoadDataFromFile() override;
};