#pragma once

namespace xr
{
  template<typename Type>
  struct vec1
  {
    Type x;
  };

  template<typename Type>
  struct vec2 : public vec1<Type>
  {
    Type y;
  };

  template<typename Type>
  struct vec3 : public vec2<Type>
  {
    Type z;
  };

  template<typename Type>
  struct vec4 : public vec3<Type>
  {
    Type w;
  };
}