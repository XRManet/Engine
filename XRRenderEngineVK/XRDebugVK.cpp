#include "stdafx.h"

vk::DebugUtilsMessengerCreateInfoEXT xrGetDefaultDebugUtilsMessengerCreateInfo();

VKAPI_ATTR VkBool32 VKAPI_CALL xrDefaultDebugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT				messageTypes,
	VkDebugUtilsMessengerCallbackDataEXT const*	pCallbackData,
	void*										/*pUserData*/)
{
#if !defined( NDEBUG )
	if (static_cast<uint32_t>(pCallbackData->messageIdNumber) == 0x822806fa)
	{
		// Validation Warning: vkCreateInstance(): to enable extension VK_EXT_debug_utils, but this extension is intended to support use by applications when
		// debugging and it is strongly recommended that it be otherwise avoided.
		return VK_FALSE;
	}
	else if (static_cast<uint32_t>(pCallbackData->messageIdNumber) == 0xe8d1a9fe)
	{
		// Validation Performance Warning: Using debug builds of the validation layers *will* adversely affect performance.
		return VK_FALSE;
	}
#endif

	std::cerr << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
		<< vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)) << ":\n";
	std::cerr << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
	std::cerr << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
	std::cerr << std::string("\t") << "message         = <" << pCallbackData->pMessage << ">\n";
	if (0 < pCallbackData->queueLabelCount)
	{
		std::cerr << std::string("\t") << "Queue Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
		{
			std::cerr << std::string("\t\t") << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
		}
	}
	if (0 < pCallbackData->cmdBufLabelCount)
	{
		std::cerr << std::string("\t") << "CommandBuffer Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
		{
			std::cerr << std::string("\t\t") << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
		}
	}
	if (0 < pCallbackData->objectCount)
	{
		std::cerr << std::string("\t") << "Objects:\n";
		for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
		{
			std::cerr << std::string("\t\t") << "Object " << i << "\n";
			std::cerr << std::string("\t\t\t") << "objectType   = " << vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType))
				<< "\n";
			std::cerr << std::string("\t\t\t") << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
			if (pCallbackData->pObjects[i].pObjectName)
			{
				std::cerr << std::string("\t\t\t") << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
			}
		}
	}
	return VK_FALSE;
}

vk::DebugUtilsMessengerCreateInfoEXT xrGetDefaultDebugUtilsMessengerCreateInfo()
{
	return vk::DebugUtilsMessengerCreateInfoEXT(
		{},
		vk::DebugUtilsMessageSeverityFlagsEXT(),
		vk::DebugUtilsMessageTypeFlagsEXT(),
		xrDefaultDebugUtilsMessengerCallback, nullptr);
}