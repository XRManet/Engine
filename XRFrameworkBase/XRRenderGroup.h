﻿#pragma once

#include <XRFrameworkBase/XRDefault.h>

// Declared from here
class XRRenderGroup;
class XRRenderGroupManager;

XRRenderAPI(xrCreateRenderGroup)()->XRRenderGroup*;

// Referenced by here
class XRInputLayout;
class XRInputLayoutDesc;
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
	uint32_t							_propertiesHash = 0;
	XRInputLayout*						_inputLayout = 0;
	bool								_isInvalid = false;


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
		if (_propertiesHash != 0)
			return _propertiesHash;

		return _propertiesHash = calcPropertyHash();
	}

private:
	virtual bool updateImpl() { return false; }

public:
	void update()
	{
		if (_isInvalid == true)
		{
			const bool result = updateImpl();
			if (result == true)
			{
				_isInvalid = false;
			}
			else
			{
				// fallback process
			}
		}
	};
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