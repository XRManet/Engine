#pragma once

#include <XRFrameworkBase/XRGeometry.h>
#include <XRFrameworkBase/XRPlatform.h>

class XRScene;

#include <memory>
class XRRenderer;
class XRFrameProcessor
{
private:
	XRRenderer* _renderer;


public:
	XRFrameProcessor();

	void Initialize();
	void UpdateFrame();
};