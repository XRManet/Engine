#pragma once

#include "stdafx.h"

#include "XRFrameworkBase/XRPrimitiveTypes.h"
#include "XRFrameworkBase/XRRenderCommon.h"

struct XRTextureCreateInfo;
class XRTexture;
class XRTextureData;
class XRTextureRHI
{
public:
	virtual ~XRTextureRHI() {}
};

#ifdef XRRHIOPENGL_EXPORTS
XRRenderExport XRTexture* xrCreateTexture(XRTextureCreateInfo const* createInfo);
XRRenderExport XRTexture* xrCreateTextureFromData(XRTextureData const* loadable);
#else
extern XRTexture* (*xrCreateTexture)(XRTextureCreateInfo const* createInfo);
extern XRTexture* (*xrCreateTextureFromData)(XRTextureData const* loadable);
#endif

/**
 * @struct	XRTextureProperties
 *
 * @brief	A texture properties.
 * @desc	텍스쳐 고유 속성. 이 속성이 동일한 텍스쳐들은 특정 플랫폼이나 기반 API에 관계없이 메모리 레이아웃이 반드시 일치한다.
 *
 * @author	Jiman Jeong
 * @date	2020-11-17
 */

struct XRTextureProperties
{
	/** @brief	Texture type */
	XRResourceType		_type;
	/** @brief	Texture 사용 방식에 대한 bit flags*/
	XRResourceUsageBits	_usage;
	/** @brief	Texture format */
	XRFormat			_format;
	/** @brief	Texture extent */
	xr::vec3<uint32_t>	_extent;
	/** @brief	Number of mip levels */
	uint32_t			_numMipLevels;
	/** @brief	Number of array layers */
	uint32_t			_numArrayLayers;
	/** @brief	Number of samples */
	uint32_t			_numSamples;

	inline bool const operator == (XRTextureProperties const& rhs) const
	{
#define IS_EQUAL_TO_RHS(member) (member == rhs.member)
		return IS_EQUAL_TO_RHS(_type)
			&& IS_EQUAL_TO_RHS(_usage)
			&& IS_EQUAL_TO_RHS(_format)
			&& IS_EQUAL_TO_RHS(_extent)
			&& IS_EQUAL_TO_RHS(_numMipLevels)
			&& IS_EQUAL_TO_RHS(_numArrayLayers)
			&& IS_EQUAL_TO_RHS(_numSamples);
	}
};

struct XRTextureCreateInfo
{
	XRTextureProperties _properties;
};

/**
 * @class	XRTexture
 *
 * @brief	A handle of texture.
 *
 * @author	Jiman Jeong
 * @date	2020-11-08
 */
class XRBaseExport XRTexture
{
#ifdef XRRHIOPENGL_EXPORTS
	/** @brief	Factory function */
	friend XRTexture* xrCreateTexture(XRTextureCreateInfo const* createInfo);
	friend XRTexture* xrCreateTextureFromData(XRTextureData const* loadable);
#endif

protected:
	/** @brief	텍스쳐 데이터
	 * @details	_data != nullptr -> 로드된 텍스쳐. */
	XRTextureData const* _data = nullptr;
	/** @brief	RHI 인터페이스 */
	XRTextureRHI* _rhi;

	/** @brief	텍스쳐 프로퍼티 */
	XRTextureProperties*	_properties;
	uint32_t				_propertiesHash;

public:
	XRTexture(XRTextureData const* data) : _data(data) {}
	XRTexture(XRTextureCreateInfo const* textureCreateInfo);
	virtual ~XRTexture() {}
};