#include "XRPlatform.h"

#include "XRModel.h"
XRInputLayout* (*xrCreateInputLayout)(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize) = nullptr;
XRModel* (*xrCreateModel)(XRModelData const* loadable) = nullptr;

#include "XRTexture.h"
XRTexture* (*xrCreateTexture)(XRTextureCreateInfo const* createInfo) = nullptr;
XRTexture* (*xrCreateTextureFromData)(XRTextureData const* loadable) = nullptr;

#include "XRBuffer.h"
XRBuffer* (*xrCreateBuffer)(XRBufferCreateInfo const* createInfo) = nullptr;

#include "XRPipeline.h"
XRPipeline* (*xrCreatePipeline)(XRPipelineStateDescription const* createInfo) = nullptr;

#include "XRCommandBuffer.h"
XRCommandBuffer* (*xrCreateCommandBuffer)() = nullptr;

#include "XRRenderGroup.h"
XRRenderGroup* (*xrCreateRenderGroup)() = nullptr;

#define XRRENDER_ENGINE_GL			XR_DYNAMIC_LIBRARY(XRRenderEngineGL)
#define XRRENDER_ENGINE_VK			XR_DYNAMIC_LIBRARY(XRRenderEngineVK)
#define XRRENDER_ENGINE_DEFAULT     XRRENDER_ENGINE_GL

const char* g_xrRenderEngineNameDso = nullptr;

static struct XRRenderEngineLinker
{
	XRPlatform::XRDSO* _dso;

	XRRenderEngineLinker()
		: _dso(nullptr)
	{
		_dso = XRPlatform::LoadDSO(g_xrRenderEngineNameDso);
		assert(_dso != nullptr);

		// Todo) List up and check availability
		std::vector<std::string> dllLists;
		XRPlatform::ListDLLFunctions(_dso, dllLists);

		GetProcAddress(xrCreateInputLayout,		"xrCreateInputLayout");
		GetProcAddress(xrCreateModel,			"xrCreateModel");
		GetProcAddress(xrCreateTexture,			"xrCreateTexture");
		GetProcAddress(xrCreateTextureFromData,	"xrCreateTextureFromData");
		GetProcAddress(xrCreateBuffer,			"xrCreateBuffer");
		GetProcAddress(xrCreatePipeline,		"xrCreatePipeline");
		GetProcAddress(xrCreateCommandBuffer,	"xrCreateCommandBuffer");
		GetProcAddress(xrCreateRenderGroup,		"xrCreateRenderGroup");
	}

	~XRRenderEngineLinker()
	{
		XRPlatform::UnloadDSO(_dso);
	}

private:
	template<typename func>
	void GetProcAddress(func& outFunction, const char* name)
	{
		outFunction = reinterpret_cast<func>(XRPlatform::GetProcAddress(_dso, name));
		assert(outFunction != nullptr);
	}
} EngineLinkerStarter;
