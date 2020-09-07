#include "stdafx.h"

#include "XRScene.h"
#include "XRObject.h"

#include "XRModel.h"
#include "XRCommandBuffer.h"

void XRObjectGroup::draw() const
{
	_model->bindWithInputLayout();

	//_commandBuffer->drawIndexed(XRPrimitiveTopology::TriangleList, XRIndexType::Index32, 0, _model->getNumIndices(0, 0));
	_commandBuffer->drawModel(XRPrimitiveTopology::TriangleList, _model);
}

XRScene::XRScene()
{
}


XRScene::~XRScene()
{
}