#include "stdafx.h"

#include "XRObjectManager.h"

#include "XRObject.h"

#include "XRModel.h"
#include "XRCommandBuffer.h"

void XRObjectGroup::draw(XRCommandBuffer* commandBuffer) const
{
	_model->bindWithInputLayout();

	//_commandBuffer->drawIndexed(XRPrimitiveTopology::TriangleList, XRIndexType::Index32, 0, _model->getNumIndices(0, 0));

	for (auto i : _objects)
	{
		commandBuffer->drawModel(XRPrimitiveTopology::TriangleList, _model);
		break;
	}
}

std::vector<XRObject*> XRObjectManager::GenerateObjects(int count)
{
	std::vector<XRObject*> objects;
	objects.reserve(count);

	for (int i = 0; i < count; ++i)
	{
		_instanced_objects.emplace_back(new XRObject);
		objects.push_back(_instanced_objects.back());
	}

	return objects;
}
