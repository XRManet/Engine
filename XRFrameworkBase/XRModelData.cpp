#include "stdafx.h"

#include "XRModel.h"
#include "XRModelData.h"

void XRModelDataLoader::Initialize()
{
	_memory.reserve(PAGE_SIZE);

	_memory.resize(sizeof(XRObjectHeader));
	new (GetHeader()) XRObjectHeader;
}

XRModelDataLoader* CreateModelDataLoader(std::string const& path);

XRModelData::XRModelData(std::string&& path)
{
	_modelDataLoader = CreateModelDataLoader(path);
	if (nullptr == _modelDataLoader) throw nullptr;
}

XRModelData::XRModelData(std::string const& path)
{
	_modelDataLoader = CreateModelDataLoader(path);
	if (nullptr == _modelDataLoader) throw nullptr;
}

// Todo(jiman): 컴파일 종속을 피하려면 팩토리가 별도 파일에 있어야 함.
#include "XRWavefrontObject.h"

XRModelDataLoader* CreateModelDataLoader(std::string const& path)
{
	size_t found = path.rfind(".xrm");
	if (found != std::string::npos)
		return new XRModelDataLoader(path);

	found = path.rfind(".obj");
	if (found != std::string::npos)
		return new XRWavefrontObject(path);

	// Other types
	return nullptr;
}