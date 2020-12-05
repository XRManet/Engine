#include "stdafx.h"

#include "XRScene.h"
#include "XRSceneNode.h"
#include "XRTransformNode.h"
#include "XRLightNode.h"
#include "XRActorNode.h"
#include "XRPipeline.h"

XRScene::XRScene()
	: _root(nullptr)
{
	_root = new XRSceneNode();
}


XRScene::~XRScene()
{
}

XRSceneNode* XRScene::GenerateNode(XRSceneNodeType type)
{
	XRSceneNode* node = nullptr;

	switch (type)
	{
	case XRSceneNodeType::TRANSFORMATION:
		node = new XRTransformNode();
		break;
	case XRSceneNodeType::LIGHT:
		node = new XRLightNode();
		break;
	case XRSceneNodeType::ACTOR:
		node = new XRActorNode();
		break;
	}

	return node;
}

void XRScene::Render(XRRenderer* renderer)
{
	_root->Render(renderer);
}