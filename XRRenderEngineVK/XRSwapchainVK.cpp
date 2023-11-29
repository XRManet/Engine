#include "stdafx.h"

#include <XRFrameworkBase/XRPlatform.h>
#include <XRFrameworkBase/Window.h>

#include "XRSwapchainVK.h"


VkSurfaceKHR createSurfaceFromWindow(xr::Window* window);

void XRSwapchainVK::initialize(XRSwapchain* swapchainHandle)
{
	_handle = swapchainHandle;
	XRSwapchainCreateInfo const* swapchainCreateInfo = swapchainHandle->getSwapchainCreateInfo();

	VkSurfaceKHR surface = createSurfaceFromWindow(swapchainCreateInfo->_window);

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = 3;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	vkGetPhysicalDeviceSurfaceSupportKHR()

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
}

#if XR_PLATFORM == XR_PLATFORM_WINDOWS
VkSurfaceKHR createSurfaceFromWindow(xr::Window* window)
{
	VkInstance vkInstance;
	HWND platformNativeHandle = static_cast<HWND>(window->getPlatformNativeHandle());

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = platformNativeHandle;
	createInfo.hinstance = GetModuleHandle(nullptr);

	VkSurfaceKHR vkSurface;
	vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &vkSurface);

	return vkSurface;
}
#else
//#elif XR_PLATFORM == XR_PLATFORM_OSX
#error "Not implemented yet"
VkSurfaceKHR createSurfaceFromWindow(xr::Window* window)
{
}
#endif