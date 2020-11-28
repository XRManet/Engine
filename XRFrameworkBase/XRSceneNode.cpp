#include "stdafx.h"

#include "XRSceneNode.h"
#include "XRRenderer.h"


XRSceneNode::XRSceneNode(XRSceneNode* parent) :
	_parent(parent)
{
	if (_parent != nullptr)
	{
		_parent->AddChild(this);
	}
}

XRSceneNode::~XRSceneNode()
{
	for (auto child : _children)
	{
		delete child;
	}
}

void XRSceneNode::AddChild(XRSceneNode* child)
{
	child->setParent(this);
	_children.push_back(child);
}

void XRSceneNode::setParent(XRSceneNode* parent)
{
	_parent = parent;
}

void XRSceneNode::Render(XRRenderer* renderer)
{
	renderer->RegisterNode(this);

	for (auto child : _children)
	{
		child->Render(renderer);
	}
}