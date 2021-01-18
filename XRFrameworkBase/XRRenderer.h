
#pragma once

#include "stdafx.h"

class XRSceneNode;
class XRTransformNode;
class XRLightNode;
class XRActorNode;

class XRResourceManager;
class XRRenderPassManager;
class XRPipelineManager;

namespace XRPlatform
{
struct XRDSO;
}

class XRBaseExport XRRenderer
{
protected:
	XRRenderPassManager* const _renderPassManager;
	XRPipelineManager* const _pipelineManager;

public:
	XRRenderer();
	~XRRenderer();

public:
	bool Bind(XRPlatform::XRDSO* dso);

public:
	virtual void Initialize(XRResourceManager* resourceManager) {}

	virtual void Update();
	virtual void Render();
	void RegisterNode(XRSceneNode* node);
	
private:
	void RegisterTransformNode(XRTransformNode* node);
	void RegisterLightNode(XRLightNode* node);
	void RegisterActorNode(XRActorNode* node);
};


using XRPFN_CREATE_RENDERER = XRRenderer * (*)();

struct XRRendererInfo
{
	char const* dso_name = nullptr;

	XRPFN_CREATE_RENDERER CreateRenderer = nullptr;

	XRRenderer* renderer = nullptr;

	bool isBound = false;

public:
	inline bool IsAvailable() { return dso_name != nullptr && CreateRenderer != nullptr; }

	XRRenderer* GetRenderer()
	{
		if (renderer == nullptr)
			renderer = CreateRenderer();
		return renderer;
	}


public:
	inline bool operator == (XRRendererInfo const& rendererInfo) {
		return dso_name == rendererInfo.dso_name
			&& CreateRenderer == rendererInfo.CreateRenderer;
	}

	inline bool operator != (XRRendererInfo const& rendererInfo) {
		return !(*this == rendererInfo);
	}
};