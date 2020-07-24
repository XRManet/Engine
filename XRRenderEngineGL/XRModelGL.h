#pragma once

#include <XRFrameworkBase/XRModel.h>

class XRInputLayoutGL : public XRInputLayout
{
private:
  unsigned _vao;

public:
    XRInputLayoutGL(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize);
    XRInputLayoutGL(const XRObjectHeader* header);
    
  virtual ~XRInputLayoutGL();

public:
	void bind() const override;
    void* generateVertexBuffers() const;
};

class XRModelGL : public XRModel
{
private:
  union {
    unsigned vbo[0];
    struct {
#if XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_SOA
		unsigned position;
		unsigned normal;
		unsigned textureCoord;
#else
      unsigned vertex;
#endif
      unsigned index;
    };
  } GL;

public:
  XRModelGL(XRModelData const* data);
  virtual ~XRModelGL();


public:
	void bind() const override;

	uint32_t getNumVertices() const override;
	uint32_t getNumIndices() const override;
};
