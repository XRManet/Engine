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
	XRSceneNode* parent = this;
	child->setParent(parent);
	_children.push_back(child);

	while (parent != nullptr && parent->_isModified == false)
	{
		parent->_isModified = true;
		parent = parent->_parent;
	}
}

void XRSceneNode::setParent(XRSceneNode* parent)
{
	_parent = parent;
}

void XRSceneNode::Render(XRRenderer* renderer)
{
	renderer->VisitNode(this);

	if (_isModified == true)
	{
		for (auto child : _children)
		{
			child->Render(renderer);
		}

		_isModified = false;
	}
	
	renderer->LeaveNode(this);
}