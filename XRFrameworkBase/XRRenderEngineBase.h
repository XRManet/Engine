#pragma once

#include <XRFrameworkBase/XRRenderEngine.h>

class XRBaseExport XRRenderEngineBase : public XRRenderEngine
{
public:
	XRRenderEngineBase(xr::Application* application) : XRRenderEngine(application) {}

public:
};

class XRBaseExport XRRenderDeviceBase : public XRRenderDevice
{
public:
	XRRenderDeviceBase(XRRenderEngine* ownerRenderEngine) : XRRenderDevice(ownerRenderEngine) {}
};