#include "XRTexture.h"

#include "XRHash.h"
class TexturePropertiesLookup
{
public:
	inline XRTextureProperties* insert(uint32_t key, XRTextureProperties const* textureProperties)
	{
		auto insertResult = _lookup.insert({ key, *textureProperties });

		const bool isInserted = insertResult.second;
		XRTextureProperties* propertiesForKey = &insertResult.first->second;
		assert(true == isInserted || (*propertiesForKey == *textureProperties));

		return propertiesForKey;
	}

private:
	std::unordered_map<uint32_t, XRTextureProperties> _lookup;
};

static TexturePropertiesLookup g_texturePropertiesLookup;

XRTexture::XRTexture(XRTextureCreateInfo const* textureCreateInfo)
{
	_propertiesHash = GetHash(&textureCreateInfo->_properties, sizeof(textureCreateInfo->_properties));
	_properties = g_texturePropertiesLookup.insert(_propertiesHash, &textureCreateInfo->_properties);
}