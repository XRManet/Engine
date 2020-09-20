#pragma once

#include "stdafx.h"

// Declared from here
class XRObject;
class XRObjectGroup;
class XRObjectManager;

// Referenced by here
class XRModel;
class XRCommandBuffer;

/******************************************************************************
 * XRObjectGroup
 * Author: Jiman
 * Context locality: Free.
 * Note:
 *	특정 모델을 사용하는 동일한 object들의 집합.
 */
class XRBaseExport XRObjectGroup
{
public:
	XRModel const* _model = nullptr;
	std::vector<XRObject const*> _objects;
	XRCommandBuffer* _commandBuffer;

	XRObjectGroup() = default;
	XRObjectGroup(XRCommandBuffer* commandBuffer, XRModel const* model, std::vector<XRObject const*>&& objects) : _commandBuffer(commandBuffer), _model(model), _objects(std::move(objects)) {}

	XRObjectGroup& operator = (const XRObjectGroup& rhs) = default;

public:
	void draw() const;
};

/******************************************************************************
 * XRObjectManager
 * Author: Jiman
 * Context locality: Global
 * Note:
 *	현재 사용 중인 모든 object의 life-ownership을 소유.
 */
class XRBaseExport XRObjectManager
{
private:
  std::vector<XRObject*> _instanced_objects;


public:
  XRObjectManager() { _instanced_objects.reserve(PAGE_SIZE / sizeof(XRObject*)); }


public:
  std::vector<XRObject*> GenerateObjects(int count);
};