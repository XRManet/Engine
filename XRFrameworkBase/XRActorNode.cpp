#include "stdafx.h"

#include "XRActorNode.h"

XRActorNode::XRActorNode() :
	_model(nullptr),
	_texture(nullptr)
{
	_type = XRSceneNodeType::ACTOR;
}

XRActorNode::~XRActorNode()
{
	if (_model != nullptr)
		delete _model;
	if (_texture != nullptr)
		delete _texture;
}

XRMaterialNode::XRMaterialNode()
{

}

XRMaterialNode::~XRMaterialNode()
{

}