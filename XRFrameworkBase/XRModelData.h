#pragma once

#include <stdafx.h>

#include "XRLoadable.h"
#include "XRPrimitiveTypes.h"

#define XR_MODEL_DATA_LAYOUT_AOS	1
#define XR_MODEL_DATA_LAYOUT_SOA	2
#define XR_MODEL_DATA_LAYOUT		XR_MODEL_DATA_LAYOUT_SOA

struct XRObjectHeader;

// XRModelData
// A default representation data form of XRModel
class XRModelDataLoader : public XRLoadable
{
public:
  struct VertexPosition { float x, y, z, w; };
  struct VertexNormal { float x, y, z; };
  struct TextureCoordinate { float u, v, w; };
  struct VertexParameter { float u, v, w; };
  struct FaceElement { int vertex, texture, normal; };
  struct FaceIndexTriangle { int v0, v1, v2; };
  struct FaceIndexQuad { int v0, v1, v2, v3; };

  struct VertexAttribute
  {
	  size_t offset = 0;
	  uint32_t num_elements = 0;
  };

public:
  XRModelDataLoader(std::string && path) : XRLoadable(std::move(path)) { Initialize(); }
  XRModelDataLoader(std::string const& path) : XRLoadable(path) { Initialize(); }

private:
  void Initialize();

public:		XRObjectHeader const*	GetHeader() const	{ return reinterpret_cast<XRObjectHeader const*>(_memory.data()); }
protected:	XRObjectHeader *		GetHeader()			{ return reinterpret_cast<XRObjectHeader*>(_memory.data()); }
};

class XRModelData
{
private:
	XRModelDataLoader* _modelDataLoader;
	
public:
	XRModelData(std::string && path);
	XRModelData(std::string const& path);
	
public:
	bool LoadDataFromFile() { return _modelDataLoader->LoadDataFromFile(); }

public:		XRObjectHeader const*	GetHeader() const { return const_cast<XRModelDataLoader const*>(_modelDataLoader)->GetHeader(); }
};
