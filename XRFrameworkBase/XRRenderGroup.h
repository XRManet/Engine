#pragma once

#include "stdafx.h"

// Declared from here
class XRRenderGroup;
class XRRenderGroupManager;

// Referenced by here
class XRInputLayout;
class XRObjectGroup;
class XRGMORule;

class XRBaseExport XRRenderGroup
{
	friend XRRenderGroupManager;

public:
	enum class UpdateFrequency {
		Static,
		Volatile,
		Default = Static,
	};

	enum class GeometricalSector {
		Global,
		Broad,
		Tiled,
		Default = Global,
	};

	struct Properties
	{
		uint32_t						_inputLayoutDescKey = 0;
		UpdateFrequency					_updateFrequency = UpdateFrequency::Default;
		GeometricalSector				_geometricalSector = GeometricalSector::Default;
	};


private:
	Properties							_properties;
	uint32_t							_propertyHash = 0;
	XRInputLayout*						_inputLayout = 0;


protected:
	std::vector<XRObjectGroup const*>	_objectGroups;


protected:
	Properties const& getPropery() const { return _properties; }
	XRInputLayout const* getInputLayout() const { return _inputLayout; }

public:
	XRRenderGroup() = default;
	virtual ~XRRenderGroup() = default;

private:
	bool isAdoptableObjectGroup(XRObjectGroup const* newGroup);
	uint32_t calcPropertyHash();

public:
	bool addObjectGroup(XRObjectGroup const* newGroup);
	void updateObjectGroup(XRObjectGroup const* objectGroup);

public:
	uint32_t getRenderGroupPropertyHash()
	{
		if (_propertyHash != 0)
			return _propertyHash;

		return _propertyHash = calcPropertyHash();
	}

private:
	virtual void update() {}

public:
	void bind() const;
	void draw() const;
};

// Note(jiman): 가칭. singleton access 가 필요해서 대충 만듬.
class XRBaseExport XRRenderGroupManager
{
private:
	std::unordered_map<uint32_t, XRRenderGroup*> _renderGroups;

private:
	XRRenderGroupManager() = default;
	~XRRenderGroupManager() {}

public:
	static XRRenderGroupManager* get()
	{
		static XRRenderGroupManager _default;
		return &_default;
	}

public:
	XRRenderGroup* getRenderGroupByKey(uint32_t keyRenderGroup)
	{
		auto result = _renderGroups.find(keyRenderGroup);
		if (result != _renderGroups.end())
			return result->second;
		return nullptr;
	}

	bool createRenderGroup(XRInputLayoutDesc& inputLayoutDesc, XRRenderGroup::Properties& properties);

	bool insertRenderGroup(XRRenderGroup* renderGroup)
	{
		return insertRenderGroup(renderGroup->getRenderGroupPropertyHash(), renderGroup);
	}

	bool insertRenderGroup(uint32_t keyRenderGroup, XRRenderGroup* renderGroup)
	{
		auto result = _renderGroups.insert({keyRenderGroup, renderGroup});
		return result.second;
	}
};
