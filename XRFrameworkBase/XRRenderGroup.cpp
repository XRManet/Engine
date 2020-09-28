#include "stdafx.h"
#include "XRHash.h"

#include "XRRenderGroup.h"

#include "XRObject.h"
#include "XRObjectManager.h"

#include "XRModel.h"
#include "XRCommandBuffer.h"

bool XRRenderGroup::isAdoptableObjectGroup(XRObjectGroup const * newGroup)
{
	if (newGroup->_model->getInputLayout() != _inputLayout)
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

uint32_t XRRenderGroup::calcPropertyHash()
{
	assert(_properties._inputLayoutHash != 0);
	return GetHash(&_properties, sizeof(_properties));
}

void XRRenderGroup::draw() const
{
	// Bind InputLayout (VAO)
	// Bind Per Model Info (Primitive VBOs)
	// Bind Per Instance Info (Instance VBOs)

	// Draw Command
}

bool XRRenderGroupManager::createRenderGroup(XRInputLayoutDesc& inputLayoutDesc, XRRenderGroup::Properties& properties)
{
	// Merge Input Layout Based on input layout got above

	XRInputLayout* inputLayout = nullptr;
	if (properties._inputLayoutHash != 0)
	{
		//	- extract or derive desc. <- Merge desc.
		class XRInputLayoutAccessor : public XRInputLayout
		{
		public:
			XRInputLayoutDesc const& getInputLayoutDesc() const { return XRInputLayout::getInputLayoutDesc(); }
		};
		auto inputLayoutAccessor = static_cast<XRInputLayoutAccessor*>(XRInputLayout::GetInputLayoutByKey(properties._inputLayoutHash));
		assert(inputLayoutAccessor == nullptr);
		if (inputLayoutAccessor == nullptr)
			return false;

		XRInputLayoutDesc mergedInputLayoutDesc = inputLayoutAccessor->getInputLayoutDesc();
	}
	else
	{
	}

	//	- create new input layout by merged desc.
	return true;
}