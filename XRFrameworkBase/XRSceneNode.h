#pragma once

#include "XRObject.h"

class XRRenderer;

enum class XRSceneNodeType
{
	NONE,
	TRANSFORMATION,
	LIGHT,
	ACTOR,
	MATERIAL,
};

struct MemoryRange
{
	uint32_t _offset;
	uint32_t _size;
};

class XRBaseExport XRSceneNode : public XRObject
{
public:
	XRSceneNode(XRSceneNode* parent = nullptr);
	virtual ~XRSceneNode();

	XRSceneNodeType GetType() { return _type; }
	void AddChild(XRSceneNode* child);
	void Render(XRRenderer *renderer);

public:
	const MemoryRange& getBoundMemoryRange() const { return _boundMemoryRange; }

private:
	void setParent(XRSceneNode* parent);

protected:
	XRSceneNodeType _type = XRSceneNodeType::NONE;
	bool _isModified = false;

private:
	XRSceneNode* _parent;
	std::vector<XRSceneNode*> _children;

	MemoryRange _boundMemoryRange;
};