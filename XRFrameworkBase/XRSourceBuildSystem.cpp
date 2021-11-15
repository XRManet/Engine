#include "stdafx.h"

#include "XRSourceBuildSystem.h"

void XRSourceBuildSystem::registerCompiledObject(uint64_t uniqueKey, XRCompiledObject* compiledObject)
{
	auto result = _compiledObjects.insert({ uniqueKey, nullptr });
	if (true == result.second)
	{
		result.first->second = compiledObject;
	}
}