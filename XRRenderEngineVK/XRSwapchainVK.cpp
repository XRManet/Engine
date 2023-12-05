#include "stdafx.h"

#include <XRFrameworkBase/XRPlatform.h>
#include <XRFrameworkBase/Window.h>

#include "XRRenderEngineVK.h"

#include "XRSwapchainVK.h"


vk::SurfaceKHR createSurfaceFromWindow(vk::Instance const& instance, xr::Window const* window);

void XRSwapchainVK::initialize(XRSwapchain* swapchainHandle)
{
	auto renderDevice = static_cast<XRRenderDeviceVulkan*>(getOwnerRenderDevice());
	auto renderEngine = static_cast<XRRenderEngineVulkan*>(renderDevice->getOwnerRenderEngine());
	
	auto& instance = renderEngine->getInstance();
	auto& physicalDevice = renderDevice->getPhysicalDevice();
	auto& device = renderDevice->getDevice();
	

	_handle = swapchainHandle;
	auto swapchainCreateInfo = swapchainHandle->getSwapchainCreateInfo();
	xr::Window const* window = swapchainCreateInfo->_window;

	vk::SurfaceKHR surface = createSurfaceFromWindow(instance, window);
	auto queueFamliyProperties = physicalDevice.getQueueFamilyProperties();
	vk::Bool32 isSupported = false;
	for (uint32_t i = 0; i < queueFamliyProperties.size(); ++i)
	{
		if (isSupported == false)
			isSupported = physicalDevice.getSurfaceSupportKHR(i, surface);
	}
	assert(isSupported);

	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
	auto presentationModes = physicalDevice.getSurfacePresentModesKHR(surface);

	auto foundSurfaceFormat = std::find_if(surfaceFormats.begin(), surfaceFormats.end(), [](auto const& surfaceFormat) {
		return surfaceFormat.format == vk::Format::eR8G8B8A8Unorm;
		});
	if (foundSurfaceFormat == surfaceFormats.end())
		foundSurfaceFormat = surfaceFormats.begin();

	auto foundPresentationMode = std::find_if(presentationModes.begin(), presentationModes.end(), [](auto const& presentationMode) {
		return presentationMode == vk::PresentModeKHR::eMailbox;
		});
	if (foundPresentationMode == presentationModes.end())
		foundPresentationMode = std::find_if(presentationModes.begin(), presentationModes.end(), [](auto const& presentationMode) {
		return presentationMode == vk::PresentModeKHR::eFifoRelaxed;
			});
	if (foundPresentationMode == presentationModes.end())
		foundPresentationMode = presentationModes.begin();

	vk::CompositeAlphaFlagBitsKHR foundCompositeAlphaBits
		= (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque) ? vk::CompositeAlphaFlagBitsKHR::eOpaque
		: (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) ? vk::CompositeAlphaFlagBitsKHR::eInherit
		: vk::CompositeAlphaFlagBitsKHR::eOpaque;
	;
	vk::SwapchainCreateInfoKHR createInfo({}, surface, 3,
		foundSurfaceFormat->format, foundSurfaceFormat->colorSpace,
		surfaceCapabilities.currentExtent, 1,
		vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive, 0, nullptr,
		surfaceCapabilities.currentTransform,
		foundCompositeAlphaBits,
		*foundPresentationMode);

	vk::SwapchainKHR swapchain = device.createSwapchainKHR(createInfo);

	_surface = surface;
	_swapchain = swapchain;
}

#if XR_PLATFORM == XR_PLATFORM_WINDOWS
vk::SurfaceKHR createSurfaceFromWindow(vk::Instance const& instance, xr::Window const* window)
{
	HWND platformNativeHandle = static_cast<HWND>(window->getPlatformNativeHandle());
	vk::Win32SurfaceCreateInfoKHR createInfo({}, GetModuleHandle(nullptr), platformNativeHandle );
	vk::SurfaceKHR vkSurface = instance.createWin32SurfaceKHR(createInfo);
	return vkSurface;
}
#else
//#elif XR_PLATFORM == XR_PLATFORM_OSX
#error "Not implemented yet"
vk::SurfaceKHR createSurfaceFromWindow(vk::Instance const& instance, xr::Window const* window)
{
}
#endif