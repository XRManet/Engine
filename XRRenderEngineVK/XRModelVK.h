#pragma once

#include <XRFrameworkBase/XRModel.h>

class XRInputLayoutVK : public XRInputLayout
{
private:
	unsigned _vao;

public:
	XRInputLayoutVK(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize);
	XRInputLayoutVK(const XRObjectHeader* header);

	virtual ~XRInputLayoutVK();

public:
	void bind() const override;
	void bindAttributes(uint32_t bindingIndex) const;
	void* generateVertexBuffers() const;
};

class XRModelVK : public XRModel
{
public:
	XRModelVK(XRModelData const* data);
	virtual ~XRModelVK();
};