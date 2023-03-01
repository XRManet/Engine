#include "stdafx.h"
#include "XRModelVK.h"

#include <XRFrameworkBase/XRModelData.h>

#include <glm/vec4.hpp>

struct XRFormatVK : public XRFormat
{
public:
};

struct XRVertexAttributeDescGL : XRVertexAttributeDesc { friend XRInputLayoutVK; };
struct XRVertexBufferDescGL : XRVertexBufferDesc { friend XRInputLayoutVK; };

XRInputLayoutVK::XRInputLayoutVK(XRInputLayoutDesc&& in_inputLayoutDesc, uint32_t preferredStrideSize)
	: XRInputLayout(std::move(in_inputLayoutDesc), preferredStrideSize)
{
}

XRInputLayoutVK::XRInputLayoutVK(const XRObjectHeader* objectHeader)
: XRInputLayout(objectHeader)
{
}

XRInputLayoutVK::~XRInputLayoutVK()
{
}

void XRInputLayoutVK::bind() const
{
}

void XRInputLayoutVK::bindAttributes(uint32_t bindingIndex) const
{

}

void* XRInputLayoutVK::generateVertexBuffers() const
{
	return nullptr;
}


XRModelVK::XRModelVK(XRModelData const* data)
	: XRModel(data)
{

}

XRModelVK::~XRModelVK()
{

}