#pragma once

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

#define NEXT_ALIGN_2(offset, size_2) ((offset + size_2 - 1) & ~(size_2 - 1))

#define LinedName_Concat(name, line)					name##line
#define LinedName_Eval(name, line)						LinedName_Concat(name, line)
#define LinedName(name)									LinedName_Eval(name , __LINE__)

#define StaticName(type, name, fn, ...)					static type LinedName(name); fn(LinedName(name), __VA_ARGS__)

template<typename T> struct __INTERNAL_TypeCarrier;

struct STATIC_ENUM_TYPE
{
	const char* _id;
};

#define STATIC_ENUM_BEGIN(ENUM_CLASS_NAME, ENUM_TYPE) \
struct ENUM_CLASS_NAME; \
namespace ENUM_CLASS_NAME##_SPACE { \
	typedef ENUM_TYPE ADD_ENUM_TYPE; \
	template<int Value> struct INNER_ENUM_TYPE { \
		constexpr const ADD_ENUM_TYPE* operator -> () const; \
		constexpr operator ENUM_CLASS_NAME const& () const; \
		constexpr operator unsigned const () const { return Value; } \
		unsigned _value = Value; /* Note(jiman): printf와 같은 memory access 직접 접근 처리 시 필요 */ \
	}; \
	template<int Value> struct DECLARED_ENUM; \
	template<int Line> struct Add { enum { Value = 0 }; }; \
	template<int Line> struct Position { enum { Value = Position<Line-1>::Value +  Add<Line>::Value }; }; \
	template<> struct Position<__LINE__> { enum { Value = 0}; }; \
	template<int Value> struct ENUM_CLASS_NAME##_BASE; template<> struct ENUM_CLASS_NAME##_BASE<0> {};\
	template<int Value> struct ARRAY_ENUM_TYPE; template<> struct ARRAY_ENUM_TYPE<0> {};\
} \

#define ADD_ENUM(ENUM_CLASS_NAME, id, ...) namespace ENUM_CLASS_NAME##_SPACE { \
	constexpr unsigned __##id = Position<__LINE__>::Value; \
	template<> struct ENUM_CLASS_NAME##_BASE<__##id + 1> : public ENUM_CLASS_NAME##_BASE<__##id> { \
		static constexpr INNER_ENUM_TYPE<__##id> id {}; \
	};\
	template<> struct Add<__LINE__+1> { enum { Value = 1 }; }; \
	template<> struct DECLARED_ENUM<__##id> { constexpr static ADD_ENUM_TYPE declared { #id, __VA_ARGS__ }; }; \
	template<> struct ARRAY_ENUM_TYPE<__##id + 1> : public ARRAY_ENUM_TYPE<__##id> { \
		ARRAY_ENUM_TYPE() : ARRAY_ENUM_TYPE<__##id>(), \
							element(&DECLARED_ENUM<__##id>::declared) {} \
		const ADD_ENUM_TYPE* element; \
	}; \
} \

#define ADD_ENUM_CTOR(ENUM_CLASS_NAME, id, func, ...) namespace ENUM_CLASS_NAME##_SPACE { \
	constexpr unsigned __##id = Position<__LINE__>::Value; \
	template<> struct ENUM_CLASS_NAME##_BASE<__##id + 1> : public ENUM_CLASS_NAME##_BASE<__##id> { \
		static constexpr INNER_ENUM_TYPE<__##id> id = {}; \
	};\
	template<> struct Add<__LINE__+1> { enum { Value = 1 }; }; \
	template<> struct DECLARED_ENUM<__##id> { constexpr static ADD_ENUM_TYPE declared = func(id, __##id, __VA_ARGS__); }; \
	template<> struct ARRAY_ENUM_TYPE<__##id + 1> : public ARRAY_ENUM_TYPE<__##id> { \
		ARRAY_ENUM_TYPE() : ARRAY_ENUM_TYPE<__##id>(), \
							element(&DECLARED_ENUM<__##id>::declared) {} \
		const ADD_ENUM_TYPE* element; \
	}; \
} \

#define STATIC_ENUM_END(ENUM_CLASS_NAME) namespace ENUM_CLASS_NAME##_SPACE { \
	static constexpr unsigned COUNT = Position<__LINE__>::Value; \
	template<int Value> constexpr const ADD_ENUM_TYPE* INNER_ENUM_TYPE<Value>::operator -> () const { return &DECLARED_ENUM<Value>::declared; } \
} \
struct ENUM_CLASS_NAME : public ENUM_CLASS_NAME##_SPACE::ENUM_CLASS_NAME##_BASE<ENUM_CLASS_NAME##_SPACE::COUNT> {\
	static constexpr unsigned COUNT = ENUM_CLASS_NAME##_SPACE::COUNT; \
public: \
	constexpr ENUM_CLASS_NAME(unsigned value) : _value(value) {} \
	template<int Value> ENUM_CLASS_NAME(ENUM_CLASS_NAME##_SPACE::INNER_ENUM_TYPE<Value> const& rhs) : _value(Value) {} \
public: \
	const ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE* operator -> () const { \
		static ENUM_CLASS_NAME##_SPACE::ARRAY_ENUM_TYPE<COUNT> array;\
		static ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE const* (*accessor)[COUNT] = reinterpret_cast<ENUM_CLASS_NAME##_SPACE::ADD_ENUM_TYPE const*(*)[COUNT]>(&array); \
		return (*accessor)[_value];\
	} \
	operator unsigned const () const { return _value; } \
	operator unsigned& () { return _value; } \
	template<int Value> bool operator == (ENUM_CLASS_NAME##_SPACE::INNER_ENUM_TYPE<Value> const& rhs) const { return Value == _value; } \
private: \
	unsigned _value; \
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
