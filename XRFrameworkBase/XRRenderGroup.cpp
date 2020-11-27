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
		_isInvalid = true;
		return true;
	}

	return false;
}

uint32_t XRRenderGroup::calcPropertyHash()
{
	assert(_properties._inputLayoutDescKey != 0);
	return GetHash(&_properties, sizeof(_properties));
}

void XRRenderGroup::bind() const
{
}

void XRRenderGroup::draw() const
{
	// Bind InputLayout (VAO)
	// Bind Per Model Info (Primitive VBOs)
	// Bind Per Instance Info (Instance VBOs)

	_inputLayout->bind();

	// Draw Command
}

/**
 * @fn	bool XRRenderGroupManager::createRenderGroup(XRInputLayoutDesc& inputLayoutDesc, XRRenderGroup::Properties& in_properties)
 *
 * @brief	per-instance input layout을 기존에 생성된 per-vertex input layout에 덧붙여 생성하는 경우,
 * 			in_properties::_inputLayoutDescKey는 per-vertex input이 포함된 input layout이다. per-vertex와
 * 			per-instance 모두를 새로 생성하는 경우 in_properties::_inputLayoutDescKey는 반드시 0이어야 한다.
 * 			그 외의 경우는 정의하지 않음.
 *
 * @author	Jiman Jeong
 * @date	2020-11-27
 *
 * @param [in,out]	inputLayoutDesc	Information describing the  Vertex input layout.
 * @param [in,out]	in_properties  	Properties for render group.
 *
 * @returns	True if it succeeds, false if it fails.
 */

bool XRRenderGroupManager::createRenderGroup(XRInputLayoutDesc& inputLayoutDesc, XRRenderGroup::Properties& in_properties)
{
	// Merge Input Layout Based on input layout got above
	//	- extract or derive desc.
	//	- create new input layout by merged desc.

	XRInputLayout* inputLayout = nullptr;
	uint32_t inputLayoutDescKey = 0;
	if (in_properties._inputLayoutDescKey != 0)
	{
		// XRInputLayout::_inputLayoutDesc는 외부 비공개 인터페이스
		class XRInputLayoutAccessor : public XRInputLayout
		{
		public:
			XRInputLayoutDesc const& getInputLayoutDesc() const { return XRInputLayout::getInputLayoutDesc(); }
		};

		auto inputLayoutAccessor = static_cast<XRInputLayoutAccessor*>(XRInputLayout::GetInputLayoutByKey(in_properties._inputLayoutDescKey));
		assert(inputLayoutAccessor == nullptr);
		if (inputLayoutAccessor == nullptr)
			return false;

		XRInputLayoutDesc mergedInputLayoutDesc = inputLayoutAccessor->getInputLayoutDesc();
		mergedInputLayoutDesc.append(inputLayoutDesc);

		uint32_t inputLayoutDescKey = mergedInputLayoutDesc.getHash();
		inputLayout = XRInputLayout::GetInputLayoutByKey(inputLayoutDescKey);
		if (inputLayout == nullptr)
		{
			inputLayout = xrCreateInputLayout(std::move(mergedInputLayoutDesc), 0);
			bool result = XRInputLayout::InsertInputLayout(inputLayoutDescKey, inputLayout);
			assert(result == true);
		}
	}
	else
	{
		assert(false);
	}

	auto renderGroup = xrCreateRenderGroup();
	renderGroup->_properties = in_properties;
	renderGroup->_properties._inputLayoutDescKey = inputLayoutDescKey;
	renderGroup->_inputLayout = inputLayout;

	bool result = insertRenderGroup(renderGroup);
	if (result == false)
	{
		delete renderGroup;
	}

	return result;
}
