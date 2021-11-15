#pragma once

#include "XRSceneNode.h"

class XRModel;
class XRTexture;

class XRBaseExport XRActorNode : public XRSceneNode
{
public:
	XRActorNode();
	virtual ~XRActorNode();

	void bindModel(XRModel* model) { _model = model; }
	void bindTexture(XRTexture* texture) { _texture = texture; }

	XRModel* getModel() const { return _model; }

private:
	XRModel* _model;
	XRTexture* _texture;
};