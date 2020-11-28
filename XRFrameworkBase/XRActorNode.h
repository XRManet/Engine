#pragma once

#include "XRSceneNode.h"

class XRModel;
class XRTexture;

class XRBaseExport XRActorNode : public XRSceneNode
{
public:
	XRActorNode();
	virtual ~XRActorNode();

	void BindModel(XRModel* model) { _model = model; }
	void BindTexture(XRTexture* texture) { _texture = texture; }

private:
	XRModel* _model;
	XRTexture* _texture;
};