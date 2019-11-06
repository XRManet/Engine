#include "stdafx.h"

#include "XRScene.h"
#include "XRObject.h"

#include "XRModel.h"
#include "XRCommandBuffer.h"

void XRObjectGroup::draw() const
{
	_model->bindWithInputLayout();

	_commandBuffer->drawIndexed(XRPrimitiveTopology::Triangles, XRIndexType::Index16, 0, _model->getNumIndices());
}

XRScene::XRScene()
{
}


XRScene::~XRScene()
{
}