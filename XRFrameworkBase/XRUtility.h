#pragma once

#include <XRFrameworkBase/XRDefault.h>

namespace XR { namespace util
{

/********************************************************
 * StringsEqual
 */
constexpr bool StringsEqual(const char* a, const char* b)
{
  return *a == *b && (*a == '\0' || StringsEqual(a + 1, b + 1));
}

/*************************************
 * GetIndexOfLiteralStringList
 */
template<int i, typename string_tuple>
struct LiteralStringListIndexGetter
{
  static constexpr int Get(string_tuple tuple, const char* find_string)
  {
    return StringsEqual(std::get<i>(tuple), find_string) ? i : LiteralStringListIndexGetter<i - 1, string_tuple>::Get(tuple, find_string);
  }
};

template<typename string_tuple>
struct LiteralStringListIndexGetter<0, string_tuple>
{
  static constexpr int Get(string_tuple tuple, const char* find_string)
  {
    return StringsEqual(std::get<0>(tuple), find_string) ? 0 : -1;
  }
};

template<typename string_tuple>
constexpr int GetIndexOfLiteralStringList(string_tuple tuple, const char* find_string)
{
  return LiteralStringListIndexGetter<std::tuple_size<string_tuple>::value - 1, string_tuple>::Get(tuple, find_string);
}

}} // namespace XR::utils

//XRExport Clonable* createClonable() { return new Clonable; }
template<typename Clonable>
struct CloneHelper
{
public:	XRExport static Clonable* clone() { return new Clonable; }
};

#define NEXT_ALIGN_2(offset, size_2) ((offset + size_2 - 1) & ~(size_2 - 1))

#define LinedName_Concat(name, line)					name##line
#define LinedName_Eval(name, line)						LinedName_Concat(name, line)
#define LinedName(name)									LinedName_Eval(name , __LINE__)

#define StaticName(type, name, fn, ...)					static type LinedName(name); fn(LinedName(name), __VA_ARGS__)

template<typename T> struct __INTERNAL_TypeCarrier;


#define STATIC_ENUM_BASE(BASE_NAME, VALUE_TYPE, ENUM_TYPE) \
namespace BASE_NAME##_SPACE { \
	typedef ENUM_TYPE ADD_ENUM_TYPE; \
	typedef VALUE_TYPE ADD_VALUE_TYPE; \
} \
struct BASE_NAME { \
	BASE_NAME() : _value(0) {} \
	constexpr BASE_NAME(VALUE_TYPE value) : _value(value) {} \
	operator VALUE_TYPE const () const { return _value; } \
	operator VALUE_TYPE& () { return _value; } \
public: \
	struct Layout { ENUM_TYPE const* _enums; uint32_t _numEnums; }; \
protected: \
	VALUE_TYPE _value; \
}; \

#define STATIC_ENUM_BEGIN(ENUM_CLASS_NAME, BASE_NAME) \
struct ENUM_CLASS_NAME; \
namespace ENUM_CLASS_NAME##_SPACE { \
	typedef BASE_NAME##_SPACE::ADD_ENUM_TYPE ADD_ENUM_TYPE; \
	typedef BASE_NAME##_SPACE::ADD_VALUE_TYPE ADD_VALUE_TYPE; \
	template<int Value> struct INNER_ENUM_TYPE { \
		constexpr const ADD_ENUM_TYPE* operator -> () const; \
		constexpr operator ENUM_CLASS_NAME const& () const; \
		constexpr operator unsigned const () const { return Value; } \
		ADD_VALUE_TYPE _value = Value; /* Note(jiman): printf와 같은 memory access 직접 접근 처리 시 필요 */ \
	}; \
	template<int Value> struct DECLARED_ENUM; \
	template<int Line> struct Add { enum { Value = 0 }; }; \
	template<int Line> struct Position { enum { Value = Position<Line-1>::Value +  Add<Line>::Value }; }; \
	template<> struct Position<__LINE__> { enum { Value = 0}; }; \
	template<int Value> struct ENUM_CLASS_NAME##_BASE; template<> struct ENUM_CLASS_NAME##_BASE<0> : public BASE_NAME { \
		constexpr ENUM_CLASS_NAME##_BASE(ADD_VALUE_TYPE value) : BASE_NAME(value) {} \
	};\
	template<int Value> struct ARRAY_ENUM_TYPE; template<> struct ARRAY_ENUM_TYPE<0> {}; \
} \

#define ADD_ENUM(ENUM_CLASS_NAME, id, ...) namespace ENUM_CLASS_NAME##_SPACE { \
	constexpr ADD_VALUE_TYPE __##id = Position<__LINE__>::Value; \
	template<> struct ENUM_CLASS_NAME##_BASE<__##id + 1> : public ENUM_CLASS_NAME##_BASE<__##id> { \
		constexpr ENUM_CLASS_NAME##_BASE(ADD_VALUE_TYPE value) : ENUM_CLASS_NAME##_BASE<__##id>(value) {} \
		static constexpr INNER_ENUM_TYPE<__##id> id {}; \
	};\
	template<> struct Add<__LINE__+1> { enum { Value = 1 }; }; \
	template<> struct DECLARED_ENUM<__##id> { constexpr static ADD_ENUM_TYPE declared { #id, __VA_ARGS__ }; }; \
	template<> struct ARRAY_ENUM_TYPE<__##id + 1> : public ARRAY_ENUM_TYPE<__##id> { \
		ARRAY_ENUM_TYPE() : ARRAY_ENUM_TYPE<__##id>(), \
							element(DECLARED_ENUM<__##id>::declared) {} \
		const ADD_ENUM_TYPE element; \
	}; \
} \

#define ADD_ENUM_CTOR(ENUM_CLASS_NAME, id, func, ...) namespace ENUM_CLASS_NAME##_SPACE { \
	constexpr ADD_VALUE_TYPE __##id = Position<__LINE__>::Value; \
	template<> struct ENUM_CLASS_NAME##_BASE<__##id + 1> : public ENUM_CLASS_NAME##_BASE<__##id> { \
		constexpr ENUM_CLASS_NAME##_BASE(ADD_VALUE_TYPE value) : ENUM_CLASS_NAME##_BASE<__##id>(value) {} \
		static constexpr INNER_ENUM_TYPE<__##id> id = {}; \
	};\
	template<> struct Add<__LINE__+1> { enum { Value = 1 }; }; \
	template<> struct DECLARED_ENUM<__##id> { constexpr static ADD_ENUM_TYPE declared = func(id, __##id, __VA_ARGS__); }; \
	template<> struct ARRAY_ENUM_TYPE<__##id + 1> : public ARRAY_ENUM_TYPE<__##id> { \
		ARRAY_ENUM_TYPE() : ARRAY_ENUM_TYPE<__##id>(), \
							element(DECLARED_ENUM<__##id>::declared) {} \
		const ADD_ENUM_TYPE element; \
	}; \
} \

#define STATIC_ENUM_END(ENUM_CLASS_NAME) namespace ENUM_CLASS_NAME##_SPACE { \
	static constexpr unsigned COUNT = Position<__LINE__>::Value; \
	template<int Value> constexpr const ADD_ENUM_TYPE* INNER_ENUM_TYPE<Value>::operator -> () const { return &DECLARED_ENUM<Value>::declared; } \
} \
struct ENUM_CLASS_NAME : public ENUM_CLASS_NAME##_SPACE::ENUM_CLASS_NAME##_BASE<ENUM_CLASS_NAME##_SPACE::COUNT> {\
public: \
	static constexpr ENUM_CLASS_NAME##_SPACE::ADD_VALUE_TYPE COUNT = ENUM_CLASS_NAME##_SPACE::COUNT; \
	static ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE const* GET_ALL_ENUMS() { \
		static ENUM_CLASS_NAME##_SPACE::ARRAY_ENUM_TYPE<COUNT> array; \
		static ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE const (*accessor)[COUNT] = reinterpret_cast<ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE const(*)[COUNT]>(&array); \
		return *accessor; \
	} \
	static Layout GetLayout() { \
		return Layout { GET_ALL_ENUMS(), COUNT }; \
	 }\
public: \
	constexpr ENUM_CLASS_NAME(ENUM_CLASS_NAME##_SPACE::ADD_VALUE_TYPE value) : ENUM_CLASS_NAME##_SPACE::ENUM_CLASS_NAME##_BASE<ENUM_CLASS_NAME##_SPACE::COUNT>(value) {} \
	template<int Value> ENUM_CLASS_NAME(ENUM_CLASS_NAME##_SPACE::INNER_ENUM_TYPE<Value> const& rhs) : ENUM_CLASS_NAME##_SPACE::ENUM_CLASS_NAME##_BASE<ENUM_CLASS_NAME##_SPACE::COUNT>(Value) {} \
public: \
	ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE const* operator -> () const { \
		static ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE const* accessor = GET_ALL_ENUMS(); \
		return &(accessor[_value]);\
	} \
	template<int Value> bool operator == (ENUM_CLASS_NAME##_SPACE::INNER_ENUM_TYPE<Value> const& rhs) const { return Value == _value; } \
}; \
namespace ENUM_CLASS_NAME##_SPACE { \
	template<int Value> constexpr INNER_ENUM_TYPE<Value>::operator ENUM_CLASS_NAME const& () const { constexpr ENUM_CLASS_NAME enumValue(Value); return enumValue; } \
} \

// Static Enum per Bit
#define STATIC_ENUM_BIT_BEGIN \
	template<int Line> struct Shift { enum { Value = 0 }; }; \
	template<int Line> struct Position { enum { Value = Position<Line-1>::Value << Shift<Line>::Value }; }; \
	template<> struct Position<__LINE__> { enum { Value = 1 }; }

#define ADD_BIT_ENUM(id, member) \
	template<> struct Shift<__LINE__+1> { enum { Value = 1 }; }; \
	static constexpr uint32_t id = Position<__LINE__>::Value; \
	bool member : 1
