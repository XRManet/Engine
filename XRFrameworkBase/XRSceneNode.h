#pragma once

#include "stdafx.h"
#include "XRObject.h"

class XRRenderer;

enum class XRSceneNodeType
{
	TRANSFORMATION,
	LIGHT,
	ACTOR
};

class XRBaseExport XRSceneNode : public XRObject
{
public:

	XRSceneNode(XRSceneNode* parent = nullptr);
	virtual ~XRSceneNode();

	XRSceneNodeType GetType() { return _type; }
	void AddChild(XRSceneNode* child);
	void Render(XRRenderer *renderer);

protected:
	XRSceneNodeType _type;

private:
	void setParent(XRSceneNode* parent);

	XRSceneNode* _parent;
	std::vector<XRSceneNode*> _children;
};