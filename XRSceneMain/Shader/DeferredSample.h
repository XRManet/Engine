#pragma once

#if defined(COMPILE_CPP)
#include <tuple>

#define STRUCT_BEGIN(name)      constexpr auto name = std::make_tuple(
#define VAR_DECLARE(type, name) type name;
#define STRUCT_END()            )
#elif defined(COMPILE_GLSL)
#define STRUCT_BEGIN(name)
#define STRUCT_END()
#endif

STRUCT_BEGIN(g_deferredVar)
  "var_test1",
  "var_test2",
  "var_test3"
STRUCT_END();

STRUCT_BEGIN(g_programNameList)
"program_name1",
"program_name2",
"program_name3",
"program_name4",
"program_name5"
STRUCT_END();