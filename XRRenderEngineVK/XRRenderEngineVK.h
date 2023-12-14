#pragma once

#include <XRFrameworkBase/XRRenderEngineBase.h>
#include <XRFrameworkBase/ApplicationChild.h>

class XRRenderEngineVulkan : public XRRenderEngineBase
{
public:
	XRRenderEngineVulkan();
	~XRRenderEngineVulkan() override;

public:
	XRRenderDevice* createRenderDevice(XRRenderDeviceCreateInfo* createInfo) override final;
	XRRenderDevice* createDefaultRenderDevice() override final;

public:
	const vk::Instance& getInstance() const { return _instance; }

public:
	void initialize(xr::Application* application);

private:
	vk::Instance					_instance;
	std::vector<vk::PhysicalDevice>	_physicalDevices;
};

struct XRRenderDeviceVulkanInfo
{
	uint32_t _graphicsQueueFamilyIndex = ~0u;
	uint32_t _computeQueueFamilyIndex = ~0u;
	uint32_t _transferQueueFamilyIndex = ~0u;

	uint32_t _presentQueueFamilyIndex = ~0u;

	std::vector<uint32_t>	_availableQueueCounts;
};

class XRRenderDeviceVulkan : public XRRenderDeviceBase
{
public:
	XRRenderDeviceVulkan(XRRenderEngine* ownerRenderEngine, XRRenderDeviceCreateInfo&& renderDeviceCreateInfo, XRRenderDeviceVulkanInfo&& renderDeviceVulkanInfo, vk::PhysicalDevice physicalDevice, vk::Device device)
		: XRRenderDeviceBase(ownerRenderEngine, std::move(renderDeviceCreateInfo)), _physicalDevice(physicalDevice), _device(device) {}


public:
	XRBuffer* createBuffer(XRBufferCreateInfo const* createInfo) override final;
	XRTexture* createTexture(XRTextureCreateInfo const* createInfo) override final;
	XRTexture* createTextureFromData(XRTextureData const* loadable) override final;
	XRPipeline* createPipeline(XRPipelineStateDescription const* description) override final;
	XRCommandBuffer* createCommandBuffer() override final;
	XRSwapchain* createSwapchain(XRSwapchainCreateInfo const* createInfo) override final;

public:
	const vk::PhysicalDevice& getPhysicalDevice() const { return _physicalDevice; }
	const vk::Device& getDevice() const { return _device; }

private:
	vk::PhysicalDevice			_physicalDevice;
	vk::Device					_device;

	XRRenderDeviceVulkanInfo	_info;
};