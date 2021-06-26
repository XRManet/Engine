#ifndef __XR_COMMON_GLSL__
#define __XR_COMMON_GLSL__

// XR Macro Expansion Utility
#define __XR_MEU_CONCAT_INTERNAL(token1, token2)		token1##token2
#define __XR_MEU_CONCAT(token1, token2)					__XR_MEU_CONCAT_INTERNAL(token1, token2)

#if defined(VULKAN)
#define XR_RESOURCE_LAYOUT(SET, ...)		layout(set=SET, __VA_ARGS__)
#define XR_INPUT_LAYOUT(...)				layout(__VA_ARGS__)
#else // GL
#define XR_RESOURCE_LAYOUT(SET, ...)		layout(__VA_ARGS__)
#define XR_INPUT_LAYOUT(...)				layout(__VA_ARGS__)
#endif

#define xrSetFrameCommon		1

#endif // __XR_COMMON_GLSL__