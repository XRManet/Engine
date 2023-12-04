// XRRenderEngineVK.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include <XRFrameworkBase/XRRenderEngineBase.h>
#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/Application.h>

#include "XRSwapchainVK.h"

#include "XRModelVK.h"
#include "XRTextureVK.h"
#include "XRBufferVK.h"
#include "XRPipelineVK.h"
#include "XRCommandBufferVK.h"
#include "XRRenderGroupVK.h"

#include <assert.h>

vk::DebugUtilsMessengerCreateInfoEXT xrGetDefaultDebugUtilsMessengerCreateInfo();

class XRRenderEngineVulkan : public XRRenderEngineBase
{
public:
	XRRenderEngineVulkan(xr::Application* application);
	~XRRenderEngineVulkan() override;

public:
	XRRenderDevice*		createRenderDevice() override final { return nullptr; }
	XRRenderDevice*		createDefaultRenderDevice() override final;

public:
	vk::Instance					_instance;
	std::vector<vk::PhysicalDevice>	_physicalDevices;
};

class XRRenderDeviceVulkan : public XRRenderDeviceBase
{
public:
	XRRenderDeviceVulkan(XRRenderEngine* ownerRenderEngine, vk::PhysicalDevice physicalDevice, vk::Device device)
		: XRRenderDeviceBase(ownerRenderEngine), _physicalDevice(physicalDevice), _device(device) {}


public:
	XRBuffer*			createBuffer(XRBufferCreateInfo const* createInfo) override final;
	XRTexture*			createTexture(XRTextureCreateInfo const* createInfo) override final;
	XRTexture*			createTextureFromData(XRTextureData const* loadable) override final;
	XRPipeline*			createPipeline(XRPipelineStateDescription const* description) override final;
	XRCommandBuffer*	createCommandBuffer() override final;
	XRSwapchain*		createSwapchain(XRSwapchainCreateInfo const* createInfo) override final;

private:
	vk::PhysicalDevice	_physicalDevice;
	vk::Device			_device;
};

XRRenderEngineVulkan::XRRenderEngineVulkan(xr::Application* application)
	: XRRenderEngineBase(application)
{
	auto instanceInfo = [](xr::Application* application)
	{
		const char* enableLayerNames[] = { "" };
		const char* enableExtensionNames[] = {
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		};

		auto appInfo = vk::ApplicationInfo(
			application->getName().c_str(), VK_MAKE_VERSION(1, 0, 0),
			"XRManetEngine", VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_3);

		auto instanceInfo = vk::InstanceCreateInfo({}, &appInfo, enableLayerNames, enableExtensionNames);
#if XR_PLATFORM == XR_PLATFORM_OSX
		instanceInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

		auto debugUtilsMessengerCreateInfo = xrGetDefaultDebugUtilsMessengerCreateInfo();

		return vk::StructureChain<
			vk::InstanceCreateInfo,
			vk::DebugUtilsMessengerCreateInfoEXT>
		{	instanceInfo,
			debugUtilsMessengerCreateInfo,
		};
	}(application);

	_instance = vk::createInstance(instanceInfo.get());
	_physicalDevices = _instance.enumeratePhysicalDevices();
}

XRRenderEngineVulkan::~XRRenderEngineVulkan()
{
	_physicalDevices.clear();
	_instance.destroy();
}

XRRenderDevice* XRRenderEngineVulkan::createDefaultRenderDevice()
{
	auto [physicalDevice, device] = [this]() {
		auto getProperPhysicalDevice = [](auto const& physicalDevices) {
			auto getScoreForPhysicalDevice = [](vk::PhysicalDevice physicalDevice) {
				auto physicalDeviceProperty = physicalDevice.getProperties();

				auto getScoreByPhysicalDeviceType = [](auto physicalDeviceType) {
					switch (physicalDeviceType)
					{
					case vk::PhysicalDeviceType::eOther:			return 0;
					case vk::PhysicalDeviceType::eIntegratedGpu:	return 2;
					case vk::PhysicalDeviceType::eDiscreteGpu:		return 4;
					case vk::PhysicalDeviceType::eVirtualGpu:		return 3;
					case vk::PhysicalDeviceType::eCpu:				return 1;
					default:										return 0;
					}
				};

				return getScoreByPhysicalDeviceType(physicalDeviceProperty.deviceType);
			};

			int32_t maxScore = getScoreForPhysicalDevice(physicalDevices.front());
			int32_t maxIndex = 0;
			for (int32_t i = 1; i < physicalDevices.size(); ++i)
			{
				int32_t score = getScoreForPhysicalDevice(physicalDevices[i]);
				if (maxScore < score)
				{
					maxScore = score;
					maxIndex = i;
				}
			}

			return physicalDevices[maxIndex];
		};

		auto physicalDevice = getProperPhysicalDevice(_physicalDevices);
		auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

		auto foundProperty = std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
			[](auto const& queueFamilyProperty) {
				return queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics;
			});

		size_t graphicsQueueFamilyIndex = std::distance(queueFamilyProperties.begin(), foundProperty);
		assert(graphicsQueueFamilyIndex < queueFamilyProperties.size());

		float queuePriority = 0.0f;
		auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo({}, static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &queuePriority);
		auto device = physicalDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo));

		return std::make_tuple(physicalDevice, device);
	} ();
	
	return new XRRenderDeviceVulkan(this, physicalDevice, device);
}

XRRenderAPI(xrCreateRenderEngine)(xr::Application* application)->XRRenderEngine*
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRRenderEngineVulkan(application);
}

template<>
struct RenderEngineInitializer<DeviceAPI::Vulkan>
{
	RenderEngineInitializer()
	{
		LogVKSystemInfo();
	}

	static RenderEngineInitializer& GetInitializer()
	{
		static RenderEngineInitializer __init;
		return __init;
	}

	void setupStatics()
	{
		
	}

	void LogVKSystemInfo()
	{
		
	}
};

XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRInputLayoutVK(std::move(inputLayoutDesc), preferredStrideSize);
}

XRModel* xrCreateModel(XRModelData const* loadable)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRModelVK(loadable);
}

XRRenderGroup* xrCreateRenderGroup()
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRRenderGroupVK;
}

XRTexture* XRRenderDeviceVulkan::createTexture(XRTextureCreateInfo const* createInfo)
{
	auto textureVK = new XRTextureVK(this);
	auto textureHandle = new XRTexture(createInfo);
	textureHandle->_rhi = textureVK;
	return textureHandle;
}

XRTexture* xrCreateTexture(XRRenderDevice* ownerRenderDevice, XRTextureCreateInfo const* createInfo) {
	static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createTexture(createInfo);
}

XRTexture* XRRenderDeviceVulkan::createTextureFromData(XRTextureData const* loadable)
{
	auto textureVK = new XRTextureVK(this);
	if (nullptr != loadable)
		textureVK->upload(loadable);

	auto textureHandle = new XRTexture(loadable);
	textureHandle->_rhi = textureVK;
	return textureHandle;
}

XRTexture* xrCreateTextureFromData(XRRenderDevice* ownerRenderDevice, XRTextureData const* loadable) {
	static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createTextureFromData(loadable);
}

XRBuffer* XRRenderDeviceVulkan::createBuffer(XRBufferCreateInfo const* createInfo)
{
	auto bufferVK = new XRBufferVK(this);
	auto bufferHandle = new XRBuffer(createInfo, bufferVK);

	bufferVK->Initialize(bufferHandle);
	return bufferHandle;
}


XRBuffer* xrCreateBuffer(XRRenderDevice* ownerRenderDevice, XRBufferCreateInfo const* createInfo) {
	static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createBuffer(createInfo);
}

XRPipeline* XRRenderDeviceVulkan::createPipeline(XRPipelineStateDescription const* description)
{
	return new XRPipelineVK(description);
}

XRPipeline* xrCreatePipeline(XRRenderDevice* ownerRenderDevice, XRPipelineStateDescription const* description) {
	static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createPipeline(description);
}

XRCommandBuffer* XRRenderDeviceVulkan::createCommandBuffer()
{
	return new XRCommandBufferVK(this);
}

XRCommandBuffer* xrCreateCommandBuffer(XRRenderDevice* ownerRenderDevice) {
	static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createCommandBuffer();
}

XRSwapchain* XRRenderDeviceVulkan::createSwapchain(XRSwapchainCreateInfo const* createInfo)
{
	auto swapchainVK = new XRSwapchainVK(this);
	auto swapchainHandle = new XRSwapchain(createInfo, swapchainVK);

	swapchainVK->initialize(swapchainHandle);
	return swapchainHandle;
}

XRSwapchain* xrCreateSwapchain(XRRenderDevice* ownerRenderDevice, XRSwapchainCreateInfo const* createInfo) {
	static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createSwapchain(createInfo);
}