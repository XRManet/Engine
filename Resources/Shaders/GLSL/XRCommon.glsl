#ifndef __XR_COMMON_GLSL__
#define __XR_COMMON_GLSL__

// XR Macro Expansion Utility
#define __XR_MEU_CONCAT_INTERNAL(token1, token2)		token1##token2
#define __XR_MEU_CONCAT(token1, token2)					__XR_MEU_CONCAT_INTERNAL(token1, token2)

#if defined(VULKAN)
#define XR_SET(SET)							set=SET,
#else // GL
#define XR_SET(SET)
#endif

#define xrSetFrameCommon		1

#endif // __XR_COMMON_GLSL__