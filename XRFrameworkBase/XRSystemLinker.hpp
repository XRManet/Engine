#include "XRJson.h"

#ifndef XRRENDER_ENGINE_MANIFEST_NAME
#define XRRENDER_ENGINE_MANIFEST_NAME			"Resources/RenderEngineSettings.json"
#endif // XRRENDER_ENGINE_MANIFEST_NAME

extern const char* g_xrRenderEngineNameDso;
extern const char* g_xrShaderBuildSystemNameDso;

static struct XRSystemLinker
{
public:
	XRSystemLinker()
	{
		bool result = LoadSystemsFromManifest();
		assert(result);
	}

private:
	bool LoadSystemsFromManifest()
	{
		JsonLoader sceneKeysLoader(XRRENDER_ENGINE_MANIFEST_NAME);
		rapidjson::Document renderEngineSettingsJson;
		sceneKeysLoader.GetParsedDocument(renderEngineSettingsJson);

		auto& version = renderEngineSettingsJson["version"];

		auto [major, minor, patch] = parseVersion(version);
		if (major == 0)
			return false;

		struct SubsystemDescription
		{
			const char* _subsystemName;
			const char*& _linkedSubsystemNameDso;
		} subsystemDescriptionsToLoad[] = {
			{"render_engine", g_xrRenderEngineNameDso},
			{"shader_build_system", g_xrShaderBuildSystemNameDso},
		};

		for (uint32_t i = 0; i < sizeof(subsystemDescriptionsToLoad) / sizeof(subsystemDescriptionsToLoad[0]); ++i)
		{
			const auto& description = subsystemDescriptionsToLoad[i];
			auto& subsystem = renderEngineSettingsJson[description._subsystemName];
			bool result = LoadSubsystem(subsystem, description._subsystemName, description._linkedSubsystemNameDso);
			assert(result);
		}

		return true;
	}

	bool LoadSubsystem(rapidjson::Value& valueForSubsystem, const char* subsystemName, const char*& outSubsystemNameDso)
	{
		auto& subsystemVersion = valueForSubsystem["version"];
		auto& dsoName = valueForSubsystem["dso_name"];

		auto [major, minor, patch] = parseVersion(subsystemVersion);
		if (major == 0 || dsoName.IsString() == false)
			return false;

		auto insertResult = _systemNameDso.insert({ subsystemName, dsoName.GetString() });
		assert(insertResult.second == true);
		outSubsystemNameDso = insertResult.first->second.c_str();

		return insertResult.second == true;
	}

	std::tuple<uint32_t, uint32_t, uint32_t> parseVersion(rapidjson::Value& version)
	{
		constexpr uint32_t INTERNAL_BUF_SIZE = 64;
		uint32_t versionStringLength = 0;
		if (version.IsNull() || version.IsString() == false || (versionStringLength = version.GetStringLength()) >= INTERNAL_BUF_SIZE)
		{
			assert(!(version.IsNull() || version.IsString() == false || (versionStringLength = version.GetStringLength()) >= INTERNAL_BUF_SIZE));
			return { 0, 0, 0 };
		}

		char buf[64];
		const char* versionString = version.GetString();
		strncpy_s(buf, versionString, versionStringLength);

		char* context = nullptr;
		char* majorStr = xr::strtok(buf, ".", &context);
		char* minorStr = xr::strtok(nullptr, ".", &context);
		char* patchStr = xr::strtok(nullptr, ".", &context);

		uint32_t major = atoi(majorStr);
		uint32_t minor = atoi(minorStr);
		uint32_t patch = atoi(patchStr);

		return { major, minor, patch };
	}

private:
	std::unordered_map<std::string, std::string> _systemNameDso;
} SystemLinkerStarter;

#include "XRRenderEngineLinker.hpp"
#include "XRSourceBuildSystemLinker.hpp"