#include "stdafx.h"

#include "XRModel.h"
#include "XRHash.h"

XRModel::XRModel(XRModelData const* loadableData) : _data(loadableData)
{
}

XRModel::~XRModel()
{
}

XRInputLayoutDesc::XRInputLayoutDesc(std::vector<XRVertexBufferDesc>& vertexBuffers)
{
	append(vertexBuffers);
}

uint32_t XRInputLayoutDesc::calcHash()
{
	size_t tempSize = 0;
	
	uint32_t numVertexBuffers = static_cast<uint32_t>(_vertexBuffers.size());
	for (uint32_t i = 0; i < numVertexBuffers; ++i)
	{
		uint32_t numAttributes = static_cast<uint32_t>(_vertexBuffers[i].attributes.size());
		tempSize += sizeof(XRVertexAttributeDesc) * numAttributes;
	}
	
	std::vector<uint8_t> bytes;
	bytes.resize(tempSize);
	
	tempSize = 0;
	for (uint32_t i = 0; i < numVertexBuffers; ++i)
	{
		uint32_t numAttributes = static_cast<uint32_t>(_vertexBuffers[i].attributes.size());
		memcpy(bytes.data() + tempSize, _vertexBuffers[i].attributes.data(), sizeof(XRVertexAttributeDesc) * numAttributes);
		tempSize += sizeof(XRVertexAttributeDesc) * numAttributes;
	}
	
	return GetHash(bytes.data(), bytes.size());
}

void XRInputLayoutDesc::append(XRInputLayoutDesc& inputLayoutDesc)
{
	append(inputLayoutDesc._vertexBuffers);
}

void XRInputLayoutDesc::append(std::vector<XRVertexBufferDesc>& vertexBufferDescs)
{
	_vertexBuffers.reserve(_vertexBuffers.size() + vertexBufferDescs.size());
	const uint32_t oldSize = _vertexBuffers.size();

	_vertexBuffers.insert(_vertexBuffers.end(), vertexBufferDescs.begin(), vertexBufferDescs.end());
	const uint32_t newSize = _vertexBuffers.size();

	uint32_t vertexAttributeIndex = (oldSize > 0) ? _vertexBuffers[oldSize - 1].attributes.back().shaderLocation + 1 : 0;
	for (uint32_t i = oldSize; i < newSize; ++i)
	{
		uint32_t numAttributes = static_cast<uint32_t>(_vertexBuffers[i].attributes.size());
		uint32_t size = 0;
		for (uint32_t j = 0; j < numAttributes; ++j)
		{
			_vertexBuffers[i].attributes[j].bindingIndex = i;
			_vertexBuffers[i].attributes[j].shaderLocation = vertexAttributeIndex++;
			_vertexBuffers[i].attributes[j].offset = size;
			size += _vertexBuffers[i].attributes[j].format.getCommonSize();
		}

		_vertexBuffers[i].bindingIndex = i;
		_vertexBuffers[i].stride = size;
	}

	_hash = calcHash();
}


static std::unordered_map<uint32_t, XRInputLayout*> g_inputLayoutLibrary;

XRInputLayout* XRInputLayout::GetInputLayoutByKey(uint32_t keyInputLayout)
{
	auto result = g_inputLayoutLibrary.find(keyInputLayout);
	if (result != g_inputLayoutLibrary.end())
		return result->second;
	return nullptr;
}

bool XRInputLayout::InsertInputLayout(uint32_t keyInputLayout, XRInputLayout* inputLayout)
{
	auto result = g_inputLayoutLibrary.insert({ keyInputLayout, inputLayout });
	// Log if not inserted.
	return result.second;
}
