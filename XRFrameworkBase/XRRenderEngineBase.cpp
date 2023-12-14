#include "stdafx.h"

#include "XRRenderEngineBase.h"

XRRenderEngineBase::XRRenderEngineBase()
	: XRRenderEngine() {}

XRRenderDeviceBase::XRRenderDeviceBase(XRRenderEngine* ownerRenderEngine, XRRenderDeviceCreateInfo&& renderDeviceCreateInfo)
	: XRRenderDevice(ownerRenderEngine)
	, _renderDeviceCreateInfo()
{}

XRRenderDeviceCreateInfo const& XRRenderDeviceBase::getRenderDeviceCreateInfo() const
{
	return _renderDeviceCreateInfo;
}