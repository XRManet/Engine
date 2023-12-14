#pragma once

#include <XRFrameworkBase/XRRenderEngine.h>

class XRBaseExport XRRenderEngineBase : public XRRenderEngine
{
public:
	XRRenderEngineBase();

public:
};

class XRBaseExport XRRenderDeviceBase : public XRRenderDevice
{
public:
	XRRenderDeviceBase(XRRenderEngine* ownerRenderEngine, XRRenderDeviceCreateInfo&& renderDeviceCreateInfo);

public:
	XRRenderDeviceCreateInfo const& getRenderDeviceCreateInfo() const override final;

private:
	XRRenderDeviceCreateInfo _renderDeviceCreateInfo;
};