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

#define XRRENDER_ENGINE_DEFAULT     XR_DYNAMIC_LIBRARY(XRRenderEngineGL)

#ifndef XRRENDER_ENGINE
#define XRRENDER_ENGINE             XRRENDER_ENGINE_DEFAULT
#endif

#include "XRJson.h"
#define XRRENDER_ENGINE_MANIFEST_NAME "Resources/RenderEngineSettings.json"

static struct XRRenderEngineLinker
{
	XRPlatform::XRDSO* _dso;

	XRRenderEngineLinker()
		: _dso(nullptr)
	{
		bool loadResult = LoadDSOFromManifest();
		if (loadResult == false || _dso == nullptr)
		{
			constexpr static const char* renderEngineName = XRRENDER_ENGINE;
			_dso = XRPlatform::LoadDSO(renderEngineName);
		}

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

private:
	bool LoadDSOFromManifest()
	{
		JsonLoader sceneKeysLoader(XRRENDER_ENGINE_MANIFEST_NAME);
		rapidjson::Document renderEngineSettingsJson;
		sceneKeysLoader.GetParsedDocument(renderEngineSettingsJson);

		auto& dsoName = renderEngineSettingsJson["dso_name"];
		if (dsoName.IsString() == false)
			return false;

		_dso = XRPlatform::LoadDSO(dsoName.GetString());
		return (_dso != nullptr);
	}
} EngineLinkerStarter;
