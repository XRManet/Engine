#pragma once

#include <stdint.h>

struct XRPrimitiveTopology
{
	enum {
		UNKNOWN,
		PointList,
		LineList,
		LineListWithAdjacency,
		LineStrip,
		LineStripWithAdjacency,
		TriangleList,
		TriangleListWithAdjacency,
		TriangleFan,
		TriangleStrip,
		TriangleStripWithAdjacency,
		QuadList,
		PatchList,
	};

	//static constexpr XRPrimitiveTopology PointList = Value::PointList;
	//static constexpr XRPrimitiveTopology LineList = Value::LineList;
	//static constexpr XRPrimitiveTopology LineListWithAdjacency = Value::LineListWithAdjacency;
	//static constexpr XRPrimitiveTopology LineStrip = Value::LineStrip;
	//static constexpr XRPrimitiveTopology LineStripWithAdjacency = Value::LineStripWithAdjacency;
	//static constexpr XRPrimitiveTopology TriangleList = Value::TriangleList;
	//static constexpr XRPrimitiveTopology TriangleListWithAdjacency = Value::TriangleListWithAdjacency;
	//static constexpr XRPrimitiveTopology TriangleFan = Value::TriangleFan;
	//static constexpr XRPrimitiveTopology TriangleStrip = Value::TriangleStrip;
	//static constexpr XRPrimitiveTopology TriangleStripWithAdjacency = Value::TriangleStripWithAdjacency;
	//static constexpr XRPrimitiveTopology QuadList = Value::QuadList;
	//static constexpr XRPrimitiveTopology PatchList = Value::PatchList;

private:
	uint32_t _value = UNKNOWN;

public:
	XRPrimitiveTopology() = default;
	inline XRPrimitiveTopology(uint32_t rhs) : _value(rhs) {}
	//inline uint32_t operator = (uint32_t rhs) { return _value = static_cast<Value>(rhs); }
	//inline operator uint32_t() const 	{ return _value; }

	inline operator uint32_t const& () const { return reinterpret_cast<uint32_t const&>(_value); }
	inline operator uint32_t& () { return reinterpret_cast<uint32_t&>(_value); }

public:
	inline uint32_t getNumVertices() const
	{
		switch (_value)
		{
		case UNKNOWN:					return 0;
		case PointList:					return 1;
		case LineList:					return 2;
		case LineListWithAdjacency:		return 2;
		case LineStrip:					return 2;
		case LineStripWithAdjacency:		return 2;
		case TriangleList:				return 3;
		case TriangleListWithAdjacency:	return 3;
		case TriangleFan:				return 3;
		case TriangleStrip:				return 3;
		case TriangleStripWithAdjacency:return 3;
		case QuadList:					return 4;
		case PatchList:					return 0;
		}
		return 0;
	}
};

struct XRIndexType
{
	enum {
		Index8,
		Index16,
		Index32,
	};

private:
	uint32_t _value = Index32;

public:
	XRIndexType() = default;
	XRIndexType(uint32_t rhs) : _value(rhs) {}
	inline uint32_t operator = (uint32_t rhs) { return _value = rhs; }
	inline operator uint32_t() const { return _value; }

public:
	inline uint32_t getIndexSize() const
	{
		switch (_value)
		{
		case Index8:					return 1;
		case Index16:					return 2;
		case Index32:					return 4;
		}
		return 0;
	}

};