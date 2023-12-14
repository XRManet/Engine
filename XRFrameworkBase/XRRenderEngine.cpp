#include "stdafx.h"

#include <XRFrameworkBase/Application.h>
#include <XRFrameworkBase/XRRenderEngine.h>

XRRenderEngine::XRRenderEngine()
	: xr::ApplicationChild()
{
}

XRRenderEngine::~XRRenderEngine()
{

}

XRRenderDevice::XRRenderDevice(XRRenderEngine* ownerRenderEngine)
	: xr::RenderEngineChild(ownerRenderEngine)
{

}

XRRenderDevice::~XRRenderDevice()
{

}

namespace xr
{
	RenderEngineChild::RenderEngineChild(XRRenderEngine* ownerRenderEngine)
		: _ownerRenderEngine(ownerRenderEngine)
	{

	}

	RenderEngineChild::~RenderEngineChild()
	{
		_ownerRenderEngine = nullptr;
	}

	RenderDeviceChild::RenderDeviceChild(XRRenderDevice* ownerRenderDevice)
		: _ownerRenderDevice(ownerRenderDevice)
	{

	}

	RenderDeviceChild::~RenderDeviceChild()
	{
		_ownerRenderDevice = nullptr;
	}

}