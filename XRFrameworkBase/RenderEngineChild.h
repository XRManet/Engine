#pragma once

class XRRenderEngine;
class XRRenderDevice;

namespace xr
{
	class XRBaseExport RenderEngineChild
	{
	public:
		RenderEngineChild(XRRenderEngine* ownerRenderEngine);
		virtual ~RenderEngineChild();

	public:
		XRRenderEngine* getOwnerRenderEngine() const { return _ownerRenderEngine; }

	private:
		XRRenderEngine* _ownerRenderEngine;
	};

	class XRBaseExport RenderDeviceChild
	{
	public:
		RenderDeviceChild(XRRenderDevice* ownerRenderDevice);
		virtual ~RenderDeviceChild();

	public:
		XRRenderDevice* getOwnerRenderDevice() const { return _ownerRenderDevice; }

	private:
		XRRenderDevice* _ownerRenderDevice;
	};
}