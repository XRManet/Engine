// XRRenderEngineVK.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"

#include "XRRenderEngineVK.h"
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


XRRenderEngineVulkan::XRRenderEngineVulkan()
	: XRRenderEngineBase()
{
}

XRRenderEngineVulkan::~XRRenderEngineVulkan()
{
	_physicalDevices.clear();
	_instance.destroy();
}

void XRRenderEngineVulkan::initialize(xr::Application* application)
{
	bindApplication(application, &xr::Application::addRenderEngine, &xr::Application::removeRenderEngine);

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

		auto instanceInfo = vk::InstanceCreateInfo({}, &appInfo);
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

auto getProperPhysicalDevice = [](auto const& physicalDevices)
{
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

	uint32_t maxScore = getScoreForPhysicalDevice(physicalDevices.front());
	uint32_t maxIndex = 0;
	for (uint32_t i = 1; i < physicalDevices.size(); ++i)
	{
		uint32_t score = getScoreForPhysicalDevice(physicalDevices[i]);
		if (maxScore < score)
		{
			maxScore = score;
			maxIndex = i;
		}
	}

	return std::make_tuple(physicalDevices[maxIndex], maxIndex);
};

auto getQueueFamliyIndex = [](vk::QueueFlags flags, vk::QueueFlags ignoreFlags, auto& queueFamilyProperties)
{
	auto foundProperty = std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
		[flags, ignoreFlags](auto const& queueFamilyProperty) {
			return ((queueFamilyProperty.queueFlags & flags) | (queueFamilyProperty.queueFlags & ignoreFlags)) == flags;
		});

	return std::distance(queueFamilyProperties.begin(), foundProperty);
};

auto pushDeviceQueueCreateInfo = [](
	auto& deviceQueueCreateInfos,
	auto& queueFamilyProperties, auto& queueFamliyIndex,
	std::vector<float>& priorities,
	vk::QueueFlags findQueueFlag,
	size_t numIgnoreFlags, vk::QueueFlags listIgnoreFlags[])
{
	if (priorities.size() != 0)
	{
		for (uint32_t t = 0; queueFamliyIndex >= queueFamilyProperties.size() && t < numIgnoreFlags; ++t)
			queueFamliyIndex = getQueueFamliyIndex(findQueueFlag, listIgnoreFlags[t], queueFamilyProperties);
	}
	
	if (queueFamliyIndex >= queueFamilyProperties.size())
		return;

	if (priorities.size() > queueFamilyProperties[queueFamliyIndex].queueCount)
		priorities.resize(queueFamilyProperties[queueFamliyIndex].queueCount);

	deviceQueueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, queueFamliyIndex, priorities));
};

XRRenderDevice* XRRenderEngineVulkan::createRenderDevice(XRRenderDeviceCreateInfo* renderDeviceCreateInfo)
{
	XRRenderDeviceCreateInfo createInfo = *renderDeviceCreateInfo;

	auto [physicalDevice, physicalDeviceIndex]
		= (createInfo._physicalDeviceSelectInfo._physicalDeviceIndex == ~0u)
		? getProperPhysicalDevice(_physicalDevices)
		: std::make_tuple(_physicalDevices[createInfo._physicalDeviceSelectInfo._physicalDeviceIndex], createInfo._physicalDeviceSelectInfo._physicalDeviceIndex);
	
	createInfo._physicalDeviceSelectInfo._physicalDeviceIndex = physicalDeviceIndex;

	XRRenderDeviceVulkanInfo vulkanInfo = {};
	std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;

	auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	auto queueFamilyProperties2 = physicalDevice.getQueueFamilyProperties2();

	vk::QueueFlags graphicsIgnoreFlags[] = { vk::QueueFlags() };
	vk::QueueFlags computeIgnoreFlags[] = { vk::QueueFlagBits::eGraphics, vk::QueueFlags() };
	vk::QueueFlags transferIgnoreFlags[] = { vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute, vk::QueueFlagBits::eCompute, vk::QueueFlagBits::eGraphics, vk::QueueFlags() };

	pushDeviceQueueCreateInfo(deviceQueueCreateInfos, queueFamilyProperties, vulkanInfo._graphicsQueueFamilyIndex, createInfo._queueCreateInfo._prioritiesForGraphicsQueues, vk::QueueFlagBits::eGraphics, xr::countof(graphicsIgnoreFlags), graphicsIgnoreFlags);
	pushDeviceQueueCreateInfo(deviceQueueCreateInfos, queueFamilyProperties, vulkanInfo._computeQueueFamilyIndex, createInfo._queueCreateInfo._prioritiesForComputeQueues, vk::QueueFlagBits::eCompute, xr::countof(computeIgnoreFlags), computeIgnoreFlags);
	pushDeviceQueueCreateInfo(deviceQueueCreateInfos, queueFamilyProperties, vulkanInfo._transferQueueFamilyIndex, createInfo._queueCreateInfo._prioritiesForTransferQueues, vk::QueueFlagBits::eTransfer, xr::countof(transferIgnoreFlags), transferIgnoreFlags);

	auto device = physicalDevice.createDevice(vk::DeviceCreateInfo({}, deviceQueueCreateInfos));
	
	vulkanInfo._availableQueueCounts.resize(queueFamilyProperties.size());
	if (queueFamilyProperties.size() > vulkanInfo._graphicsQueueFamilyIndex)
		vulkanInfo._availableQueueCounts[vulkanInfo._graphicsQueueFamilyIndex] = createInfo._queueCreateInfo._prioritiesForGraphicsQueues.size();
	if (queueFamilyProperties.size() > vulkanInfo._computeQueueFamilyIndex)
		vulkanInfo._availableQueueCounts[vulkanInfo._computeQueueFamilyIndex] = createInfo._queueCreateInfo._prioritiesForComputeQueues.size();
	if (queueFamilyProperties.size() > vulkanInfo._transferQueueFamilyIndex)
		vulkanInfo._availableQueueCounts[vulkanInfo._transferQueueFamilyIndex] = createInfo._queueCreateInfo._prioritiesForTransferQueues.size();

	if (createInfo._physicalDeviceSelectInfo._enablePresent)
	{
		if (createInfo._queueCreateInfo._presentGraphics)
			createInfo._queueCreateInfo._presentGraphics = (vulkanInfo._graphicsQueueFamilyIndex != ~0u) && (physicalDevice.getWin32PresentationSupportKHR(vulkanInfo._graphicsQueueFamilyIndex) == VK_TRUE);
		if (createInfo._queueCreateInfo._presentCompute)
			createInfo._queueCreateInfo._presentCompute = (vulkanInfo._computeQueueFamilyIndex != ~0u) && (physicalDevice.getWin32PresentationSupportKHR(vulkanInfo._computeQueueFamilyIndex) == VK_TRUE);
		if (createInfo._queueCreateInfo._presentTransfer)
			createInfo._queueCreateInfo._presentTransfer = (vulkanInfo._transferQueueFamilyIndex != ~0u) && (physicalDevice.getWin32PresentationSupportKHR(vulkanInfo._transferQueueFamilyIndex) == VK_TRUE);
	}

	return new XRRenderDeviceVulkan(this, std::move(createInfo), std::move(vulkanInfo), physicalDevice, device);
}

XRRenderDevice* XRRenderEngineVulkan::createDefaultRenderDevice()
{
	XRRenderDeviceCreateInfo createInfo;

	auto [physicalDevice, physicalDeviceIndex] = getProperPhysicalDevice(_physicalDevices);
	createInfo._physicalDeviceSelectInfo._physicalDeviceIndex = physicalDeviceIndex;

	auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	size_t graphicsQueueFamilyIndex = getQueueFamliyIndex(vk::QueueFlagBits::eGraphics, vk::QueueFlags(), queueFamilyProperties);
	assert(graphicsQueueFamilyIndex < queueFamilyProperties.size());

	createInfo._queueCreateInfo._prioritiesForGraphicsQueues.push_back(0.0f);

	float queuePriority = createInfo._queueCreateInfo._prioritiesForGraphicsQueues.back();
	auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo({}, static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &queuePriority);
	auto device = physicalDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo));
	
	XRRenderDeviceVulkanInfo vulkanInfo = {};
	vulkanInfo._availableQueueCounts.resize(queueFamilyProperties.size());
	vulkanInfo._availableQueueCounts[graphicsQueueFamilyIndex] = 1;
	vulkanInfo._graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;

	return new XRRenderDeviceVulkan(this, std::move(createInfo), std::move(vulkanInfo), physicalDevice, device);
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

template<typename T>
class XRRHIBinder : public T
{
public:
	void setRhiObject(decltype(T::_rhi) rhiObject) { T::_rhi = rhiObject; }
};

XRRenderAPI(xrCreateRenderEngine)(xr::Application* application)->XRRenderEngine*
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	auto renderEngine = new XRRenderEngineVulkan();
	renderEngine->initialize(application);
	return renderEngine;
}

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
	static_cast<XRRHIBinder<XRTexture>*>(textureHandle)->setRhiObject(textureVK);
	return textureHandle;
}

XRTexture* xrCreateTexture(XRRenderDevice* ownerRenderDevice, XRTextureCreateInfo const* createInfo) {
	return static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createTexture(createInfo);
}

XRTexture* XRRenderDeviceVulkan::createTextureFromData(XRTextureData const* loadable)
{
	auto textureVK = new XRTextureVK(this);
	if (nullptr != loadable)
		textureVK->upload(loadable);

	auto textureHandle = new XRTexture(loadable);
	static_cast<XRRHIBinder<XRTexture>*>(textureHandle)->setRhiObject(textureVK);
	return textureHandle;
}

XRTexture* xrCreateTextureFromData(XRRenderDevice* ownerRenderDevice, XRTextureData const* loadable) {
	return static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createTextureFromData(loadable);
}

XRBuffer* XRRenderDeviceVulkan::createBuffer(XRBufferCreateInfo const* createInfo)
{
	auto bufferVK = new XRBufferVK(this);
	auto bufferHandle = new XRBuffer(createInfo, bufferVK);

	bufferVK->Initialize(bufferHandle);
	return bufferHandle;
}


XRBuffer* xrCreateBuffer(XRRenderDevice* ownerRenderDevice, XRBufferCreateInfo const* createInfo) {
	return static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createBuffer(createInfo);
}

XRPipeline* XRRenderDeviceVulkan::createPipeline(XRPipelineStateDescription const* description)
{
	return new XRPipelineVK(description);
}

XRPipeline* xrCreatePipeline(XRRenderDevice* ownerRenderDevice, XRPipelineStateDescription const* description) {
	return static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createPipeline(description);
}

XRCommandBuffer* XRRenderDeviceVulkan::createCommandBuffer()
{
	return new XRCommandBufferVK(this);
}

XRCommandBuffer* xrCreateCommandBuffer(XRRenderDevice* ownerRenderDevice) {
	return static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createCommandBuffer();
}

XRSwapchain* XRRenderDeviceVulkan::createSwapchain(XRSwapchainCreateInfo const* createInfo)
{
	auto swapchainVK = new XRSwapchainVK(this);
	auto swapchainHandle = new XRSwapchain(createInfo, swapchainVK);

	swapchainVK->initialize(swapchainHandle);
	return swapchainHandle;
}

XRSwapchain* xrCreateSwapchain(XRRenderDevice* ownerRenderDevice, XRSwapchainCreateInfo const* createInfo) {
	return static_cast<XRRenderDeviceVulkan*>(ownerRenderDevice)->createSwapchain(createInfo);
}