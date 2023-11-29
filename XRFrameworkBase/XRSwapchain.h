#pragma once

namespace xr
{
	class Window;
} // namespace xr

struct XRSwapchainCreateInfo;
class XRSwapchain;

XRRenderAPI(xrCreateSwapchain)(XRSwapchainCreateInfo const* createInfo)->XRSwapchain*;

struct XRSwapchainCreateInfo
{
	xr::Window* _window;
};

class XRSwapchainRHI
{
protected:
	XRSwapchain* _handle;

public:
	virtual ~XRSwapchainRHI() {}
};

class XRBaseExport XRSwapchain
{
#ifdef XRRENDERENGINE_EXPORTS
	/** @brief	Factory function */
	friend XRSwapchain* xrCreateSwapchain(XRSwapchainCreateInfo const* createInfo);
#endif

protected:
	XRSwapchainRHI*			_rhi;
	XRSwapchainCreateInfo	_swapchainCreateInfo;

public:
	XRSwapchain(XRSwapchainCreateInfo const* swapchainCreateInfo, XRSwapchainRHI* swapchainRHI)
		: _rhi(swapchainRHI), _swapchainCreateInfo(*swapchainCreateInfo)
	{}

	virtual ~XRSwapchain() {}

public:
	inline XRSwapchainCreateInfo const* getSwapchainCreateInfo() const { return &_swapchainCreateInfo; }
};