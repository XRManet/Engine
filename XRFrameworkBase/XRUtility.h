#pragma once

namespace XR { namespace utils
{

constexpr bool strings_equal(const char* a, const char* b)
{
  return *a == *b && (*a == '\0' || strings_equal(a + 1, b + 1));
}

template<int i, typename string_tuple>
struct LiteralStringListIndexGetter
{
  static constexpr int Get(string_tuple tuple, const char* find_string)
  {
    return strings_equal(std::get<i>(tuple), find_string) ? i : LiteralStringListIndexGetter<i - 1, string_tuple>::Get(tuple, find_string);
  }
};

template<typename string_tuple>
struct LiteralStringListIndexGetter<0, string_tuple>
{
  static constexpr int Get(string_tuple tuple, const char* find_string)
  {
    return strings_equal(std::get<0>(tuple), find_string) ? 0 : -1;
  }
};

template<typename string_tuple>
constexpr int GetIndexOfLiteralStringList(string_tuple tuple, const char* find_string)
{
  return LiteralStringListIndexGetter<std::tuple_size<string_tuple>::value - 1, string_tuple>::Get(tuple, find_string);
}

}} // namespace XR::utils