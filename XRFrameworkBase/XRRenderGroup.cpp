#include "stdafx.h"

#include "XRRenderGroup.h"

#include "XRObject.h"
#include "XRObjectManager.h"

#include "XRModel.h"
#include "XRCommandBuffer.h"

bool XRRenderGroup::isAdoptableObjectGroup(XRObjectGroup const * newGroup)
{
	if (newGroup->_model->getInputLayout() != _property._inputLayout)
		return false;
	
	return true;
}

bool XRRenderGroup::addObjectGroup(XRObjectGroup const * newGroup)
{
	if (true == isAdoptableObjectGroup(newGroup))
	{
		_objectGroups.push_back(newGroup);
		return true;
	}

	return false;
}

void XRRenderGroup::draw() const
{
	// Bind InputLayout (VAO)
	// Bind Per Model Info (Primitive VBOs)
	// Bind Per Instance Info (Instance VBOs)

	// Draw Command
}
