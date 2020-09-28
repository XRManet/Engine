#include "stdafx.h"

#include "XRModel.h"
#include "XRHash.h"

uint32_t XRFormat::getCommonSize() const
{
	switch(_value)
	{
		case XRFormat::UNKNOWN:
			return 0;
		case XRFormat::R4G4_UNORM_PACK8:
			return 1;
		case XRFormat::R8G8_UNORM:
			return 2;
		case XRFormat::R8G8B8_UNORM:
			return 3;
		case XRFormat::R8G8B8A8_TYPELESS:
		case XRFormat::R8G8B8A8_UNORM:
		case XRFormat::R8G8B8A8_SNORM:
		case XRFormat::R8G8B8A8_USCALED:
		case XRFormat::R8G8B8A8_SSCALED:
		case XRFormat::R8G8B8A8_UINT:
		case XRFormat::R8G8B8A8_SINT:
		case XRFormat::R8G8B8A8_SRGB:
		case XRFormat::B8G8R8A8_UNORM:
		case XRFormat::B8G8R8A8_SNORM:
		case XRFormat::B8G8R8A8_USCALED:
		case XRFormat::B8G8R8A8_SSCALED:
		case XRFormat::B8G8R8A8_UINT:
		case XRFormat::B8G8R8A8_SINT:
		case XRFormat::B8G8R8A8_SRGB:
		case XRFormat::A8B8G8R8_UNORM_PACK32:
		case XRFormat::A8B8G8R8_SNORM_PACK32:
		case XRFormat::A8B8G8R8_USCALED_PACK32:
		case XRFormat::A8B8G8R8_SSCALED_PACK32:
		case XRFormat::A8B8G8R8_UINT_PACK32:
		case XRFormat::A8B8G8R8_SINT_PACK32:
		case XRFormat::A8B8G8R8_SRGB_PACK32:
		case XRFormat::A2R10G10B10_UNORM_PACK32:
		case XRFormat::A2R10G10B10_SNORM_PACK32:
		case XRFormat::A2R10G10B10_USCALED_PACK32:
		case XRFormat::A2R10G10B10_SSCALED_PACK32:
		case XRFormat::A2R10G10B10_UINT_PACK32:
		case XRFormat::A2R10G10B10_SINT_PACK32:
		case XRFormat::A2B10G10R10_UNORM_PACK32:
		case XRFormat::A2B10G10R10_SNORM_PACK32:
		case XRFormat::A2B10G10R10_USCALED_PACK32:
		case XRFormat::A2B10G10R10_SSCALED_PACK32:
		case XRFormat::A2B10G10R10_UINT_PACK32:
		case XRFormat::A2B10G10R10_SINT_PACK32:
			return 4;
		case XRFormat::R16_UNORM:
		case XRFormat::R16_SNORM:
		case XRFormat::R16_USCALED:
		case XRFormat::R16_SSCALED:
		case XRFormat::R16_UINT:
		case XRFormat::R16_SINT:
		case XRFormat::R16_SFLOAT:
			return 2;
		case XRFormat::R16G16_UNORM:
		case XRFormat::R16G16_SNORM:
		case XRFormat::R16G16_USCALED:
		case XRFormat::R16G16_SSCALED:
		case XRFormat::R16G16_UINT:
		case XRFormat::R16G16_SINT:
		case XRFormat::R16G16_SFLOAT:
			return 4;
		case XRFormat::R16G16B16_UNORM:
		case XRFormat::R16G16B16_SNORM:
		case XRFormat::R16G16B16_USCALED:
		case XRFormat::R16G16B16_SSCALED:
		case XRFormat::R16G16B16_UINT:
		case XRFormat::R16G16B16_SINT:
		case XRFormat::R16G16B16_SFLOAT:
			return 6;
			
		case XRFormat::R32G32_SFLOAT:
			return 8;
			
		case XRFormat::R32G32B32_SFLOAT:
			return 12;
	}
	assert(false);
	return -1;
}

XRModel::XRModel(XRModelData const* loadableData) : _data(loadableData)
{
}

XRModel::~XRModel()
{
}

XRInputLayoutDesc::XRInputLayoutDesc(std::vector<XRVertexBufferDesc>&& vertexBuffers)
	: _vertexBuffers(std::move(vertexBuffers))
{
	uint32_t numVertexBuffers = static_cast<uint32_t>(_vertexBuffers.size());
	uint32_t vertexAttributeIndex = 0;
	for (uint32_t i = 0; i < numVertexBuffers; ++i)
	{
		uint32_t numAttributes = static_cast<uint32_t>(_vertexBuffers[i].attributes.size());
		uint32_t size = 0;
		for(uint32_t j = 0; j < numAttributes; ++j)
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
	_vertexBuffers.reserve(_vertexBuffers.size() + inputLayoutDesc._vertexBuffers.size());

	// XRInputLayoutDesc 생성자 참고해서 BindingPoint, AttibuteIndex 조정
	_vertexBuffers.insert(_vertexBuffers.end(), inputLayoutDesc._vertexBuffers.begin(), inputLayoutDesc._vertexBuffers.end());
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
