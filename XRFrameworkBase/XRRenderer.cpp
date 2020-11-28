#include "stdafx.h"
#include "XRRenderer.h"
#include "XRSceneNode.h"
#include "XRTransformNode.h"
#include "XRLightNode.h"
#include "XRActorNode.h"

XRRenderer::XRRenderer()
{

}

XRRenderer::~XRRenderer()
{

}

void XRRenderer::Render()
{

}

void XRRenderer::RegisterNode(XRSceneNode* node)
{
	XRSceneNodeType type = node->GetType();
	switch (type)
	{
	case XRSceneNodeType::TRANSFORMATION:
		RegisterTransformNode(dynamic_cast<XRTransformNode*>(node));
		break;
	case XRSceneNodeType::LIGHT:
		RegisterLightNode(dynamic_cast<XRLightNode*>(node));
		break;
	case XRSceneNodeType::ACTOR:
		RegisterActorNode(dynamic_cast<XRActorNode*>(node));
		break;
	}
}

void XRRenderer::RegisterTransformNode(XRTransformNode* node)
{

}

void XRRenderer::RegisterLightNode(XRLightNode* node)
{

}

void XRRenderer::RegisterActorNode(XRActorNode* node)
{

}