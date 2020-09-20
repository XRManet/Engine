#pragma once

#include "stdafx.h"

// Declared from here

// Referenced by here
class XRInputLayout;
class XRObjectGroup;
class XRGMORule;

class XRBaseExport XRRenderGroup
{
public:
	enum class UpdateFrequency {
		Static,
		Volatile
	};

	enum class GeometricalSector {
		Global,
	};

	struct RenderGroupProperty
	{
		XRInputLayout*					_inputLayout;
		UpdateFrequency					_updateFrequency;
		GeometricalSector				_geometricalSector;
	};


private:
	RenderGroupProperty					_property;


private:
	std::vector<XRObjectGroup const*>	_objectGroups;

public:
	XRRenderGroup() = default;
	virtual ~XRRenderGroup() = default;

private:
	bool isAdoptableObjectGroup(XRObjectGroup const* newGroup);

public:
	bool addObjectGroup(XRObjectGroup const* newGroup);

public:
	void draw() const;
};
