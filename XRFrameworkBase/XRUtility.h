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