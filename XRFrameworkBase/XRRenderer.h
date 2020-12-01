
#pragma once

#include "stdafx.h"

class XRSceneNode;
class XRTransformNode;
class XRLightNode;
class XRActorNode;

class XRBaseExport XRRenderer
{
public:
	XRRenderer();
	~XRRenderer();

public:
	void Render();
	void RegisterNode(XRSceneNode* node);
	
private:
	void RegisterTransformNode(XRTransformNode* node);
	void RegisterLightNode(XRLightNode* node);
	void RegisterActorNode(XRActorNode* node);
};