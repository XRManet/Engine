#pragma once

#include <XRFrameworkBase/XRSwapchain.h>

class XRSwapchainVK : public XRSwapchainRHI
{
public:
	XRSwapchainVK(XRRenderDevice* ownerRenderDevice)
		: XRSwapchainRHI(ownerRenderDevice), _swapchain(VK_NULL_HANDLE) {}
	virtual ~XRSwapchainVK() {}

public:
	inline VkSwapchainKHR getVkSwapchain() const { return _swapchain; }

public:
	void initialize(XRSwapchain* swapchainHandle);

private:
	VkSurfaceKHR _surface;
	VkSwapchainKHR _swapchain;
};