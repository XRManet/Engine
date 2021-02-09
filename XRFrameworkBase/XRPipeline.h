#pragma once

#include "stdafx.h"
#include <XRFrameworkBase/XRPrimitiveTypes.h>
#include <XRFrameworkBase/XRGeometry.h>
#include <XRFrameworkBase/XRRenderCommon.h>
#include <XRFrameworkBase/XRIndexedString.h>

struct XRPipelineStateDescription;
class XRPipeline;

#ifdef XRRHIOPENGL_EXPORTS
XRRenderExport XRPipeline* xrCreatePipeline(XRPipelineStateDescription const* createInfo);
#else
extern XRPipeline* (*xrCreatePipeline)(XRPipelineStateDescription const* createInfo);
#endif


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
	Multisampling					MultisampleState
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

struct XRRasterizationStateDescription
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

/**
 * @struct	XRCompareOp
 *
 * @brief	An xr compare operation.
 *
 * @author	Jiman Jeong
 * @date	2020-11-28
 */

struct XRCompareOp
{
	STATIC_ENUM_BIT_BEGIN;
	ADD_BIT_ENUM(Less, _less);
	ADD_BIT_ENUM(Equal, _equal);
	ADD_BIT_ENUM(Greater, _greater);

	static constexpr uint32_t Never			= 0;
	static constexpr uint32_t LessOrEqual	= Less | Equal;
	static constexpr uint32_t GreaterOrEqual= Greater | Equal;
	static constexpr uint32_t NotEqual		= Less | Greater;
	static constexpr uint32_t Always		= Less | Equal | Greater;

	operator uint8_t const& () const { return reinterpret_cast<uint8_t const&>(*this); }
	operator uint8_t& () { return reinterpret_cast<uint8_t&>(*this); }

	XRCompareOp() { *this = 0; }
	XRCompareOp(uint8_t rhs) { *this = rhs; }
};
constexpr uint32_t sizeofXRCompareOp = sizeof(XRCompareOp);

enum class XRStencilOp : uint8_t
{
	Keep,
	Zero,
	Replace,
	IncrementAndClamp,
	DecrementAndClamp,
	Invert,
	IncrementAndWrap,
	DecrementAndWrap,
};

struct XRDepthStencilStateDescription
{
public: // Enable Tests
	bool _enableDepthTest = false; /** @brief	True to enable, false to disable the depth test */
	bool _enableStencilTest = false; /** @brief	True to enable, false to disable the stencil test */
	bool _enableDepthBoundsTest = false; /** @brief	True to enable, false to disable the depth bounds test */
	bool _enableDepthWrite = true; /** @brief	True to enable, false to disable the depth write */

public: // Depth Test
	XRCompareOp _depthCompareOp = XRCompareOp::Never; /** @brief	The depth compare operation */

public: // Stencil Front
	XRStencilOp _stencilFrontPassOp; /** @brief	The stencil front pass operation */
	XRStencilOp _stencilFrontFailOp; /** @brief	The stencil front fail operation */
	XRStencilOp _stencilFrontDepthFailOp; /** @brief	The stencil front depth fail operation */
	XRCompareOp _stencilFrontCompareOp = XRCompareOp::Never; /** @brief	The stencil front compare operation */
	uint32_t _stencilFrontCompareMask; /** @brief	The stencil front compare mask */
	uint32_t _stencilFrontWriteMask; /** @brief	The stencil front write mask */
	uint32_t _stencilFrontReference; /** @brief	The stencil front reference */

public: // Stencil Back
	XRStencilOp _stencilBackPassOp; /** @brief	The stencil back pass operation */
	XRStencilOp _stencilBackFailOp; /** @brief	The stencil back fail operation */
	XRStencilOp _stencilBackDepthFailOp; /** @brief	The stencil back depth fail operation */
	XRCompareOp _stencilBackCompareOp = XRCompareOp::Never; /** @brief	The stencil back compare operation */
	uint32_t _stencilBackCompareMask; /** @brief	The stencil back compare mask */
	uint32_t _stencilBackWriteMask; /** @brief	The stencil back write mask */
	uint32_t _stencilBackReference; /** @brief	The stencil back reference */
	
public: // Depth Bounds
	float _depthBoundsMin; /** @brief	The minimum depth bounds */
	float _depthBoundsMax; /** @brief	The maximum depth bounds */
};
constexpr uint32_t sizeofXRDepthStencilState = sizeof(XRDepthStencilStateDescription);

struct XRColorBlendStateDescription
{
	enum class LogicOp : uint8_t
	{
		Clear,
		And,
		AndReverse,
		Copy,
		AndInverted,
		NoOp,
		Xor,
		Or,
		Nor,
		Equivalent,
		Invert,
		OrReverse,
		CopyInverted,
		OrInverted,
		Nand,
		Set
	};
	
	struct TargetSlotDescription
	{
		enum class BlendFactor : uint8_t
		{
			Zero,
			One,
			SrcColor,
			OneMinusSrcColor,
			DstColor,
			OneMinusDstColor,
			SrcAlpha,
			OneMinusSrcAlpha,
			DstAlpha,
			OneMinusDstAlpha,
			ConstantColor,
			OneMinusConstantColor,
			ConstantAlpha,
			OneMinusConstantAlpha,
			SrcAlphaSaturate,
			Src1Color,
			OneMinusSrc1Color,
			Src1Alpha,
			OneMinusSrc1Alpha,
		};

		enum class BlendOp : uint8_t
		{
			Add,				//
			Subtract,			//
			ReverseSubtract,	//
			Min,				//
			Max,				//

			// Belows are considered for f/X/Y/Z Advanced Blend Operations
			Zero,		// (X, Y, Z) = (0, 0, 0), f(Cs, Cd) = 0
			Src,		// (X, Y, Z) = (1, 1, 0), f(Cs, Cd) = Cs
			Dst,		// (X, Y, Z) = (1, 0, 1), f(Cs, Cd) = Cd
			SrcOver,	// (X, Y, Z) = (1, 1, 1), f(Cs, Cd) = Cs
			DstOver,	// (X, Y, Z) = (1, 1, 1), f(Cs, Cd) = Cd
			SrcIn,		// (X, Y, Z) = (1, 0, 0), f(Cs, Cd) = Cs
			DstIn,		// (X, Y, Z) = (1, 0, 0), f(Cs, Cd) = Cd
			SrcOut,		// (X, Y, Z) = (0, 1, 0), f(Cs, Cd) = 0
			DstOut,		// (X, Y, Z) = (0, 0, 1), f(Cs, Cd) = 0
			SrcAtop,	// (X, Y, Z) = (1, 0, 1), f(Cs, Cd) = Cs
			DstAtop,	// (X, Y, Z) = (1, 1, 0), f(Cs, Cd) = Cd
			Xor,		// (X, Y, Z) = (0, 1, 1), f(Cs, Cd) = 0
			Multiply,	// (X, Y, Z) = (1, 1, 1), f(Cs, Cd) = CsCd
			Screen,		// (X, Y, Z) = (1, 1, 1), f(Cs, Cd) = Cs + Cd - CsCd
			Overlay,	/* (X, Y, Z) = (1, 1, 1), f(Cs, Cd) = 
						 * if Cd <= 0.5, then 2CsCd 
						 * otherwise, 1 - 2(1 - Cs)(1 - Cd) */
			Darken,		// (X, Y, Z) = (1, 1, 1), f(Cs, Cd) = min(Cs, Cd)
			Lighten,	// (X, Y, Z) = (1, 1, 1), f(Cs, Cd) = max(Cs, Cd)
		};

		enum class ColorComponentBits : uint8_t { None = 0, R = 0b0001, G = 0b0010, B = 0b0100, A = 0b1000, All = 0xf };

	public:
		bool _enableBlend; // BLEND
		BlendFactor _colorBlendFactorSrc; // BLEND SRC RGB
		BlendFactor _colorBlendFactorDst; // BLEND DST RGB
		BlendOp _colorBlendOp;
		BlendFactor _alphaBlendFactorSrc; // BLEND SRC ALPHA
		BlendFactor _alphaBlendFactorDst; // BLEND DST ALPHA
		BlendOp _alphaBlendOp;
		ColorComponentBits _colorWriteMask = ColorComponentBits::All;
		// BLEND EQUATION RGB (legacy)
		// BLEND EQUATION ALPHA (legacy)
	};

public:
	bool _enableLogicOp; /** @brief	True to enable, false to disable the logic operation */ // LOGIC OP MODE
	LogicOp _logicOp; /** @brief	The logic operation */ // COLOR LOGIC OP
	// INDEX LOGIC OP (legacy)

public:
	uint32_t _numTargetSlots; /** @brief	Number of target slots */
	TargetSlotDescription* _targetSlotDescriptions;
	xr::vec4<float> _blendConstants; /** @brief	The blend constants */ // BLEND COLOR
};
constexpr uint32_t sizeofXRColorBlendStateDescription = sizeof(XRColorBlendStateDescription);

//struct XRFramebufferControlDescription
//{
//	// INDEX WRITEMASK
//	// COLOR WRITEMASK
//	// DEPTH WRITEMASK
//	// STENCIL WRITEMASK
//	// STENCIL BACK WRITEMASK
//	// COLOR CLEAR VALUE
//	// INDEX CLEAR VALUE
//	// DEPTH CLEAR VALUE
//	// STENCIL CLEAR VALUE
//	// ACCUM CLEAR VALUE
//};

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
	const XRInputLayout*			_inputLayout;
	XRPrimitiveTopology				_primitiveTopology;
	bool							_isEnabledPrimitivaRestart;

	inline void init(const XRInputLayout* inputLayout, XRPrimitiveTopology primitiveTopology) {
		init(inputLayout, primitiveTopology, false);
	}
	inline void init(const XRInputLayout* inputLayout, XRPrimitiveTopology primitiveTopology, bool enablePrimitiveRestart) {
		_inputLayout = inputLayout;
		_primitiveTopology = primitiveTopology;
		_isEnabledPrimitivaRestart = enablePrimitiveRestart;
	}
};

class XRViewport;
struct XRViewportStateDescription
{
	uint32_t						_numViewports;
	const XRViewport*				_viewports;

	uint32_t						_numScissors;
	const XRScissorRect*			_scissorRects;
};

struct XRResourceLayout
{

};

struct XRPipelineStateDescription
{
	XRShaderStageDescription*			_shaderStageDescription = nullptr;
	XRVertexInputStateDescription*		_vertexInputStateDescription = nullptr;

//	XRTessellationStateDescription*		_tessellationStateDescription;

	XRRasterizationStateDescription*	_rasterizationStateDescription = nullptr;
	XRMultisampleDescription*			_multisampleStateDescription = nullptr;
	XRDepthStencilStateDescription*		_depthStencilStateDescription = nullptr;
	XRColorBlendStateDescription*		_colorBlendStateDescription = nullptr;

	XRResourceLayout*					_resourceLayout;
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
	XRPipeline(XRPipelineStateDescription const* description) {}
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
		DontCareLoad,
	};

	enum StoreOp
	{
		Store,
		DontCareStore,
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

class XRImageView {};
class XRRenderTargetView : public XRImageView {};
class XRDepthStencilView : public XRImageView {};
class XRRenderPassBase;

struct XRFramebufferLayout
{
	xr::vec3<uint32_t> _extent; // width, height, layers
	XRRenderPassBase* _compatibleRenderPass;
};

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
private:
	XRFramebufferLayout _layout;
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

struct XRPermutationElementInfo
{
	const char* _name;
	int16_t _count;
};

STATIC_ENUM_BASE(XRPermutationBase, uint16_t, XRPermutationElementInfo);

struct XRPermutationElementArgument
{
	XRPermutationBase _id;
	int16_t _value;

	XRPermutationElementArgument() = default;
	XRPermutationElementArgument(XRPermutationBase id, int16_t value)
		: _id(id), _value(value) {}
};

struct XRPipelineCreateInfo
{
	xr::IndexedString<XRPipeline>		_name;
	XRPipelineStateDescription			_description;
	XRPermutationBase::Layout			_permutationLayout;
	std::function<bool(XRPipelineStateDescription&, std::vector<XRPermutationElementArgument> const&)>	_permute;
};

// Note(jiman): 이름 적절한 것으로 변경 필요
class XRBaseExport XRPipelineGroup
{
	XRPipelineCreateInfo const*					_createInfo;

	std::unordered_map<uint32_t, XRPipeline*>	_pipelineLookup;
	std::vector<XRPipeline*>					_pipelines;

	std::vector<XRPermutationElementArgument>	_defaultFingerprint;
	uint32_t									_defaultHash;

public:
	XRPipelineGroup() = default;
	XRPipelineGroup(XRPipelineCreateInfo const& createInfo)
		: _createInfo(&createInfo), _defaultHash(0) {}
	virtual ~XRPipelineGroup() {}

public:
	void AddPipelineWithPermutation(XRPipeline* pipeline, std::vector<XRPermutationElementArgument>& permutationDefinition);

public:
	std::vector<XRPermutationElementArgument> const& GetDefaultPermutation() const { return _defaultFingerprint; }
	void RetrieveDefaultPermutation(std::vector<XRPermutationElementArgument>& elementArguments) { elementArguments = _defaultFingerprint; }
	void SetDefaultPermutation(std::vector<XRPermutationElementArgument> const& elementArguments);

public:
	uint32_t CalcPermutationHash(std::vector<XRPermutationElementArgument> const& elementArguments);

public:
	inline XRPipeline* GetPipeline() { return _pipelineLookup[_defaultHash]; }
	inline XRPipeline* GetPipeline(uint32_t permutationHash) { return _pipelineLookup.find(permutationHash)->second; }
};

class XRPipeline;
class XRBaseExport XRPipelineManager
{
	std::unordered_map<xr::IndexedString<XRPipeline>, XRPipelineGroup*> _pipelines;
	std::vector<XRPipelineCreateInfo>	_pipelineCreateInfos;

public:
	inline XRPipelineGroup* GetPipelineGroup(xr::IndexedString<XRPipeline> pipelineName)
	{
		auto found = _pipelines.find(pipelineName);
		return (found != _pipelines.end()) ? found->second : nullptr;
	}

	bool CreatePipeline(XRPipelineCreateInfo const& createInfo);
};

// Note(jiman): VC++에서 'RenderPassAutoGenerator<RenderPass>::generate' 형식으로 처리
template<typename RenderPass> struct RenderPassAutoGenerator { public:	XRExport static RenderPass* generate() { return new RenderPass; } };
// Note(jiman): #RequiredClangOption (-fdelayed-template-parsing)
#define RenderPass(XRRenderPassName) XRRenderPassName; template struct RenderPassAutoGenerator<XRRenderPassName>; class XRRenderPassName : public XRRenderPassBase

class XRBaseExport XRRenderPassBase
{
public:
	using LinkInitializeFunc = void();
	using LinkUpdateFunc = void(SampleState& sampleState, int frameIndex);

protected: // Permutations
	std::unordered_set<xr::IndexedString<XRPipeline>> _pipelineNames;
	std::vector<XRPipelineCreateInfo>	_pipelineCreateInfos;

	std::vector<XRPermutationElementArgument> _elements;

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
	virtual void DefaultPipelineState(XRPipelineStateDescription& outDefault) = 0;
	// 파생 파이프라인이 필요한 경우에 override 합니다.
	virtual bool CreatePipelineState(XRPipelineStateDescription& outPipeline) { return true; }

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

using XRPFN_GENERATE_RENDERPASS = XRRenderPassBase * (*)(void);

class XRBaseExport XRRenderPassManager
{
private:
	struct RenderPassInternal
	{
		XRRenderPassBase*			_renderPass;
		XRPFN_GENERATE_RENDERPASS	_generate;
	};

private:
	std::unordered_map<xr::IndexedString<XRRenderPassBase>, RenderPassInternal> _renderPasses;

public:
	bool RegisterRenderPassGenerator(std::string&& string, void* generator);
	XRRenderPassBase* GetRenderPass(xr::IndexedString<XRRenderPassBase> const& renderPassName) const { return _renderPasses.find(renderPassName)->second._renderPass; }
};

struct CommandStep
{
	xr::IndexedString<CommandStep>	_name;
	uint16_t						_step;
};

class XRCommandBuffer;
class CommandFootprint
{
	std::vector<CommandStep> _steps;
	std::vector<std::function<void(XRCommandBuffer*)>> _capturedCcommands;

public:
	void push_back(CommandStep&& step, std::function<void(XRCommandBuffer*)>&& capturedCommand)
	{
		_steps.push_back(std::move(step));
		_capturedCcommands.push_back(std::move(capturedCommand));
	}
};