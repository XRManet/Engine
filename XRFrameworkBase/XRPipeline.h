#pragma once

#include "stdafx.h"
#include <XRFrameworkBase/XRGeometry.h>

/******************************************************************************
  GL State to Vulkan State Mapping
 -----------------------------------------------------------------------
	GL								VK
 	Transformation
		Viewport					ViewportState
		DepthRange					RasterizationState->RasterizationDepthClipState
		DepthClamp					RasterizationState
		ClipDistance[i]				Shader decorating for VS output, PS input
		TransformFeedbackBinding	vkCmdBindTransformFeedbackBuffer, vkBegin/EndTransformFeedback
	Rasterization
		RasterizerDiscard			RasterizationState
		PointSize					RasterizationState
		PointFadeThresholdSize		?
		PointSpriteCoordOrigin		?
		LineWidth					RasterizationState
		LineSmooth					RasterizationState
		CullFaceMode				RasterizationState
		FrontFace					RasterizationState
		PolygonSmooth				?
		PolygonMode					RasterizationState
		PolygonOffset				RasterizationState
	Multisampling
	PixelOperation					ViewportState/DepthStencilState/ColorBlendState
		ScissorTest					ViewportState
		StencilTest					DepthStencilState
		DepthTest					DepthStencilState
		Blend						ColorBlendState
		LogicOp						ColorBlendState
		Dither
		Framebuffer sRGB
	FramebufferControl
		Color/DepthWriteMask		ColorBlendState->ColorBlendAttachmentState
		Color/DepthClearValue
	Framebuffer TargetBinding Point
		Draw/ReadFrameBufferBinding	RenderPass
	Framebuffer per FrameBuffer Object
		DrawBuffer[i]
		ReadBuffer
		FramebufferDefault*			No corresponding Vulkan API/Object exists but could be emulated.
	Framebuffer per Attachment Point
		FramebufferAttachment*
	Renderbuffer per TargetAndBinding Point
	Renderbuffer per RenderBuffer Object
 */

struct XRShaderStageDescription
{
	char const* _vertexFilename;
	char const* _fragmentFilename;
};

class XRScissorRect
{

};

struct XRRasterizationDescription
{
	// RASTERIZER DISCARD
	bool _enableRasterizerDiscard;
	bool _enableDepthClamp;
	// POINT SIZE
	float _pointSize;
	// Line stipple was deprecated from GL 3.1
	// https://stackoverflow.com/questions/6017176/gllinestipple-deprecated-in-opengl-3-1/55088683#55088683
	// POINT SMOOTH		- legacy
	// POINT SPRITE		- legacy
	// POINT SIZE MIN	- legacy
	// POINT SIZE MAX	- legacy
	// POINT FADE THRESHOLD SIZE
	// POINT DISTANCE ATTENUATION	- legacy
	// POINT SPRITE COORD ORIGIN
	// LINE WIDTH
	float _lineWidth;
	// LINE SMOOTH
	enum class LineSmooth
	{
		Default,
	};
	LineSmooth _lineSmooth;
	// LINE STIPPLE PATTERN	- legacy
	// LINE STIPPLE REPEAT	- legacy
	// LINE STIPPLE			- legacy
	// CULL FACE and CULL FACE MODE
	enum class CullMode
	{
		None, // bool _enableCullFace;
		Front,
		Back,
		FrontAndBack,
	};
	CullMode _cullMode;
	// FRONT FACE
	enum class FrontFace
	{
		CCW,
		CW,
	};
	FrontFace _frontFace;
	// POLYGON SMOOTH
	enum class PolygonSmooth
	{
		Default,
	};
	PolygonSmooth _enablePolygonSmooth;
	// POLYGON MODE
	enum class PolygonMode
	{
		Fill,
		Line,
		Point,
		FillBoundNV
	};
	PolygonMode _polygonMode;

	// POLYGON OFFSET POINT/LINE/FILL
	bool _enableDepthBias;
	// POLYGON OFFSET CLAMP
	float _depthBiasClamp;
	// POLYGON OFFSET FACTOR
	float _depthBiasSlopeFactor;
	// POLYGON OFFSET UNITS
	float _depthBiasConstantFactor; // added to each fragment depth
	// - 				- legacy
	// POLYGON STIPPLE	- legacy
};

struct XRMultisampleDescription
{
	// MULTISAMPLE
	bool _enableMultisampling;
	// SAMPLE ALPHA TO COVERAGE
	bool _enableAlphaToCoverage;
	// SAMPLE ALPHA TO ONE
	bool _enableAlphaToOne;
	// SAMPLE COVERAGE
	// SAMPLE COVERAGE VALUE
	// SAMPLE COVERAGE INVERT
	// SAMPLE SHADING
	bool _enableSampleShading;
	// MIN SAMPLE SHADING VALUE
	// SAMPLE MASK
	// SAMPLE MASK VALUE
};

// GL Transformation State

// GL PixelOperation
// Scissor			VK ViewportState
// Depth/Stencil	VK DepthStencilState
// Alpha/Blend		VK ColorBlendState
struct XRPixelOperationDescription
{
	// SCISSOR TEST
	// SCISSOR BOX
	// ALPHA TEST - legacy
	// ALPHA TEST FUNC - legacy
	// ALPHA TEST REF - legacy
	// STENCIL TEST
	// STENCIL FUNC
	// STENCIL VALUE MASK
	// STENCIL REF
	// STENCIL FAIL
	// STENCIL PASS DEPTH FAIL
	// STENCIL PASS DEPTH PASS
	// STENCIL BACK FUNC
	// STENCIL BACK VALUE MASK
	// STENCIL BACK REF
	// STENCIL BACK FAIL
	// STENCIL BACK PASS DEPTH FAIL
	// STENCIL BACK PASS DEPTH PASS
	// DEPTH TEST
	// DEPTH FUNC
	// BLEND
	// BLEND SRC RGB
	// BLEND SRC ALPHA
	// BLEND DST RGB
	// BLEND DST ALPHA
	// BLEND EQUATION RGB
	// BLEND EQUATION ALPHA
	// BLEND COLOR
	// FRAMEBUFFER SRGB
	// DITHER
	// INDEX LOGIC OP
	// COLOR LOGIC OP
	// LOGIC OP MODE
};

struct XRFramebufferControlDescription
{
	// INDEX WRITEMASK
	// COLOR WRITEMASK
	// DEPTH WRITEMASK
	// STENCIL WRITEMASK
	// STENCIL BACK WRITEMASK
	// COLOR CLEAR VALUE
	// INDEX CLEAR VALUE
	// DEPTH CLEAR VALUE
	// STENCIL CLEAR VALUE
	// ACCUM CLEAR VALUE
};

struct XRFramebufferDescription
{
	// Per target binding point
	// DRAW FRAMEBUFFER BINDING
	// READ FRAMEBUFFER BINDING

	// Per framebuffer object
	// DRAW BUFFERi
	// READ BUFFER
	// FRAMEBUFFER DEFAULT WIDTH
	// FRAMEBUFFER DEFAULT HEIGHT
	// FRAMEBUFFER DEFAULT LAYERS
	// FRAMEBUFFER DEFAULT SAMPLES // Multi samples
	// FRAMEBUFFER DEFAULT FIXED SAMPLE LOCATIONS // Programmable sample locations
	//  glGetObjectLabel()

	// Per attachment point
	// FRAMEBUFFER ATTACHMENT OBJECT TYPE
	// FRAMEBUFFER ATTACHMENT OBJECT NAME
	// FRAMEBUFFER ATTACHMENT TEXTURE LEVEL
	// FRAMEBUFFER ATTACHMENT TEXTURE CUBE -
	// MAP FACE
	// FRAMEBUFFER ATTACHMENT TEXTURE LAYER
	// FRAMEBUFFER ATTACHMENT LAYERED
	// FRAMEBUFFER ATTACHMENT COLOR ENCODING
	// FRAMEBUFFER ATTACHMENT COMPONENT TYPE
	// FRAMEBUFFER ATTACHMENT x SIZE
};

struct XRRenderbufferDescription
{
	// Per target binding point
	// RENDERBUFFER BINDING

	// Per renderbuffer object
	// RENDERBUFFER WIDTH
	// RENDERBUFFER HEIGHT
	// RENDERBUFFER INTERNAL FORMAT
	// RENDERBUFFER RED SIZE
	// RENDERBUFFER GREEN SIZE
	// RENDERBUFFER BLUE SIZE
	// RENDERBUFFER ALPHA SIZE
	// RENDERBUFFER DEPTH SIZE
	// RENDERBUFFER STENCIL SIZE
	// RENDERBUFFER SAMPLES
	// -
};

class XRInputLayout;
struct XRVertexInputStateDescription
{
	const XRInputLayout*			_inputlayout;
	XRPrimitiveTopology				_primitiveTopology;
	bool							_isEnabledPrimitivaRestart;
};

class XRViewport;
struct XRViewportStateDescription
{
	uint32_t						_numViewports;
	const XRViewport*				_viewports;

	uint32_t						_numScissors;
	const XRScissorRect*			_scissorRects;
};

struct XRRasterizeationStateDescription
{

};

struct XRResourceLayout
{

};

struct XRPipelineStateDescription
{
	const XRShaderStageDescription*			_shaderStageDescription;
	const XRVertexInputStateDescription*		_vertexInputStateDescription;

//	XRTessellationStateDescription*			_tessellationStateDescription;
//	
	const XRRasterizeationStateDescription*	_rasterizationStateDescription;

	const XRResourceLayout*				_resourceLayout;
};

enum class XRDynamicState
{ // from Vulkan spec 1.1.121
	VIEWPORT = 0,
	SCISSOR = 1,
	LINE_WIDTH = 2,
	DEPTH_BIAS = 3,
	BLEND_CONSTANTS = 4,
	DEPTH_BOUNDS = 5,
	STENCIL_COMPARE_MASK = 6,
	STENCIL_WRITE_MASK = 7,
	STENCIL_REFERENCE = 8,
	VIEWPORT_W_SCALING_NV = 1000087000,
	DISCARD_RECTANGLE_EXT = 1000099000,
	SAMPLE_LOCATIONS_EXT = 1000143000,
	VIEWPORT_SHADING_RATE_PALETTE_NV = 1000164004,
	VIEWPORT_COARSE_SAMPLE_ORDER_NV = 1000164006,
	EXCLUSIVE_SCISSOR_NV = 1000205001,
	LINE_STIPPLE_EXT = 1000259000,
};

class XRBaseExport XRPipeline
{
public:
	XRPipeline(XRShaderStageDescription const* description) {}
	virtual ~XRPipeline() {}


public:
	virtual void bind() {}
};

struct XRAttachmentLayout
{
	XRFormat _format;
	/** @brief	Number of samples. Multisample 사용 시 설정. */
	uint32_t _numSamples = 1;
};

struct XRAttachmentDescription
{
	enum LoadOp
	{
		Load,
		Clear,
		DontCare,
	};

	enum StoreOp
	{
		Store,
		DontCare,
	};

public:
	/** @brief	The attachment layout. 별도 array로 분리되어 AttachmentDescriptor[]와 동일한 인덱스를 사용하도록 변경될 수 있음.*/
	XRAttachmentLayout	_attachmentLayout;
	/** @brief	The load operation. format이 color면 color attachment에 대응하여 처리, depth/stencil이면 depth attachment에 대응하여 처리. */
	LoadOp	_loadOp;
	/** @brief	The store operation. format이 color면 color attachment에 대응하여 처리, depth/stencil이면 depth attachment에 대응하여 처리. */
	StoreOp	_storeOp;
	/** @brief	The stencil load operation. format이 color인 경우 무시. */
	LoadOp	_stencilLoadOp;
	/** @brief	The stencil store operation. format이 color인 경우 무시. */
	StoreOp	_stencilStoreOp;
};

class XRRenderTargetView {};
class XRDepthStencilView {};

/**
 * @class	XRFramebuffer
 *
 * @brief	An xr framebuffer.
 *
 * @desc	RenderPass에서 사용할 모든 attachment에 대응하는 뷰를 관리하고, 이를 통해 렌더될 크기를 결정합니다.
 *
 * @author	Jiman Jeong
 * @date	2020-11-17
 */

class XRFramebuffer
{
public:
	XRFramebuffer(const XRFramebufferLayout& layout)
	{
		
	}

public:

	/**
	 * @fn	virtual void attachColor(XRRenderTargetView const& rtv, uint32_t attachmentIndex);
	 *
	 * @brief	Attach color
	 *
	 * @author	Jiman Jeong
	 * @date	2020-11-17
	 *
	 * @param 	rtv			   	The rtv.
	 * @param 	attachmentIndex	Zero-based index of the attachment.
	 * 							
	 * @desc	RenderTargetView를 [attachmentIndex] 의 RenderTarget slot에 설정한다.
	 */
	virtual void attachColor(XRRenderTargetView const& rtv, uint32_t attachmentIndex);

	/**
	 * @fn	virtual void attachColors(uint32_t numColors, XRRenderTargetView const* rtv, uint32_t baseAttachmentIndex);
	 *
	 * @brief	Attach colors
	 *
	 * @author	Jiman Jeong
	 * @date	2020-11-17
	 *
	 * @param 	numColors		   	Number of colors.
	 * @param 	rtv				   	The rtv.
	 * @param 	baseAttachmentIndex	Zero-based index of the base attachment.
	 * 								
	 * @desc	numColors 개의 RenderTargetView를 [baseAttachmentIndex] ~ [baseAttachmentIndex + numColors - 1] 의 RenderTarget slot에 설정한다.
	 */
	virtual void attachColors(uint32_t numColors, XRRenderTargetView const* rtv, uint32_t baseAttachmentIndex);

	/**
	 * @fn	virtual void attachColors(uint32_t numColors, XRRenderTargetView const* rtv, uint32_t const* attachmentIndices);
	 *
	 * @brief	Attach colors
	 *
	 * @author	Jiman Jeong
	 * @date	2020-11-17
	 *
	 * @param 	numColors		 	Number of colors.
	 * @param 	rtv				 	The rtv.
	 * @param 	attachmentIndices	The attachment indices.
	 * 								
	 * @desc	numColors 개의 RenderTargetView를 [attachmentIndices[0]] ~ [[attachmentIndices[numColors - 1]] 의 RenderTarget slot에 설정한다.
	 */
	virtual void attachColors(uint32_t numColors, XRRenderTargetView const* rtv, uint32_t const* attachmentIndices);

	virtual void attachDepthStencil(XRDepthStencilView const& dsv);

	virtual void attachResolve(XRDepthStencilView const& dsv);

public:
	void populateAttachmentDescriptions(std::vector<XRAttachmentDescription>& attachmentDescriptions) const;
};

struct XRRenderPassCreateInfo
{
	uint32_t _renderPassCreateTest;
};

struct SampleState
{
	int32_t _testInt = 0;
};

struct ElementInfo
{
	std::string _name;
	int32_t _count;
};

struct Element
{
	const ElementInfo* _info;
	int32_t _value;
};

struct XRPipelineStateDescription;
class XRPipeline;
class XRBaseExport XRRenderPassBase
{
public:
	using LinkInitializeFunc = void();
	using LinkUpdateFunc = void(SampleState& sampleState, int frameIndex);

protected: // Permutations
	std::vector<ElementInfo> _elementInfos;
	std::vector<Element> _elements;
	SampleState _sampleState;

protected: // Attachments
	/** @brief	The attachments. 이 렌더패스 내에서 사용할 모든 attachments에 대한 설명. */
	std::vector<XRAttachmentDescription> _attachments;

public:
	inline void SetElement(int32_t elementIndex, int32_t value) { _elements[elementIndex]._value = value; }

public:
	void Initialize(LinkInitializeFunc fp)
	{
		_initialize = fp;
	}

	void Update(LinkUpdateFunc fp)
	{
		_update = fp;
	}

private:
	// 공용으로 필요한 PipelineState를 지정합니다.
	// 여기서 지정한 PipelineState를 기본값으로 하여 CreatePipelineState()의 기본 인자로 사용합니다.
	virtual void DefaultPipelineState(XRPipelineStateDescription& outDefault) const = 0;
	// 파생 파이프라인이 필요한 경우에 override 합니다.
	virtual bool CreatePipelineState(XRPipelineStateDescription& outPipeline) const { return true; }

public:
	void dependency();
	void record()
	{
		//xrBeginRenderPass();

	}

private:
	LinkInitializeFunc* _initialize;
	LinkUpdateFunc* _update;
};

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRPipeline* xrCreatePipeline(XRShaderStageDescription const* description);
#else
extern XRPipeline* (*xrCreatePipeline)(XRShaderStageDescription const* description);
#endif