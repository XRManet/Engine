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


template<typename T> struct __INTERNAL_TypeCarrier;

// Static Enum per count
#define STATIC_ENUM_BEGIN(ENUM_CLASS_NAME, ENUM_TYPE) \
union ENUM_CLASS_NAME##_WRAPPER; \
using ENUM_CLASS_NAME = __INTERNAL_TypeCarrier<const ENUM_CLASS_NAME##_WRAPPER>; \
constexpr static const union ENUM_CLASS_NAME##_WRAPPER \
{ \
	typedef ENUM_TYPE ADD_ENUM_TYPE; \
	struct INCREASE_ENUM { \
		template<int Value> \
		struct INNER_ENUM_TYPE { \
			constexpr const ADD_ENUM_TYPE* operator -> () const; \
			constexpr operator uint32_t const& () const { return _value; } \
			uint32_t _value; \
		}; \
		template<int Value> struct DECLARED_ENUM; \
		template<int Line> struct Add { enum { Value = 0 }; }; \
		template<int Line> struct Position { enum { Value = Position<Line-1>::Value + Add<Line>::Value }; }; \
		template<> struct Position<__LINE__> { enum { Value = 0 }; }; \

#define ADD_ENUM(id, ...) \
	public: \
		static constexpr INNER_ENUM_TYPE<Position<__LINE__>::Value> id = { Position<__LINE__>::Value }; \
		template<> struct Add<__LINE__+1> { enum { Value = 1 }; }; \
	private: \
		 template<> struct DECLARED_ENUM<id._value> { constexpr static ADD_ENUM_TYPE declared { #id, __VA_ARGS__ }; }; \

#define ADD_ENUM_CTOR(id, func, ...) \
	public: \
		static constexpr INNER_ENUM_TYPE<Position<__LINE__>::Value> id = { Position<__LINE__>::Value }; \
		template<> struct Add<__LINE__+1> { enum { Value = 1 }; }; \
	private: \
template<> struct DECLARED_ENUM<id._value> { constexpr static ADD_ENUM_TYPE declared = func(id, __VA_ARGS__); }; \

#define STATIC_ENUM_END(name) \
	public: \
		static constexpr uint32_t COUNT = Position<__LINE__>::Value; \
	}; \
} name = {}; using name##_wrapper = decltype(name); \
template<> struct __INTERNAL_TypeCarrier<name##_wrapper> : public name##_wrapper::INCREASE_ENUM {}; \
template<int Value> constexpr const name##_wrapper::ADD_ENUM_TYPE* name##_wrapper::INCREASE_ENUM::INNER_ENUM_TYPE<Value>::operator -> () const { return &name##_wrapper::INCREASE_ENUM::DECLARED_ENUM<Value>::declared; }

// Static Enum per Bit
#define STATIC_ENUM_BIT_BEGIN \
	template<int Line> struct Shift { enum { Value = 0 }; }; \
	template<int Line> struct Position { enum { Value = Position<Line-1>::Value << Shift<Line>::Value }; }; \
	template<> struct Position<__LINE__> { enum { Value = 1 }; }

#define ADD_BIT_ENUM(id, member) \
	template<> struct Shift<__LINE__+1> { enum { Value = 1 }; }; \
	static constexpr uint32_t id = Position<__LINE__>::Value; \
	bool member : 1
