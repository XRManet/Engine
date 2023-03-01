﻿#pragma once

#include <XRFrameworkBase/XRScene.h>

class /*XRExport */XRSceneTest :
	public XRScene
{
public:
	XRSceneTest();
	virtual ~XRSceneTest();


public:
	virtual void Initialize() override;
	virtual void Update(float dt) override;
};

extern "C" XRExport XRScene * CreateSceneTest()
{
	return new XRSceneTest;
}
