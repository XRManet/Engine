#include "stdafx.h"

#include "XRTextureData.h"

void XRTextureData::Initialize()
{
	_memory.reserve(PAGE_SIZE);

	_memory.resize(sizeof(TextureHeader));
	new (GetHeader()) TextureHeader;
}
