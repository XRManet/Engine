#pragma once

namespace xr
{
	template<typename Type> struct vec1;
	template<typename Type> struct vec2;
	template<typename Type> struct vec3;
	template<typename Type> struct vec4;

	template<typename Type>
	struct vec1
	{
		Type x;

		inline bool const operator == (vec1<Type> const& rhs) const { return x == rhs.x; }
		inline bool const operator == (vec2<Type> const& rhs) const { return rhs == *this; }
		inline bool const operator == (vec3<Type> const& rhs) const { return rhs == *this; }
		inline bool const operator == (vec4<Type> const& rhs) const { return rhs == *this; }
	};

	template<typename Type>
	struct vec2 : public vec1<Type>
	{
		Type y;

		inline bool const operator == (vec1<Type> const& rhs) const { return vec1<Type>::operator ==(rhs) && (y == 0); }
		inline bool const operator == (vec2<Type> const& rhs) const { return vec1<Type>::operator ==(rhs) && (y == rhs.y); }
		inline bool const operator == (vec3<Type> const& rhs) const { return rhs == *this; }
		inline bool const operator == (vec4<Type> const& rhs) const { return rhs == *this; }
	};

	template<typename Type>
	struct vec3 : public vec2<Type>
	{
		Type z;

		inline bool const operator == (vec1<Type> const& rhs) const { return vec2<Type>::operator ==(rhs) && (z == 0); }
		inline bool const operator == (vec2<Type> const& rhs) const { return vec2<Type>::operator ==(rhs) && (z == 0); }
		inline bool const operator == (vec3<Type> const& rhs) const { return vec2<Type>::operator ==(rhs) && (z == rhs.z); }
		inline bool const operator == (vec4<Type> const& rhs) const { return rhs == *this; }
	};

	template<typename Type>
	struct vec4 : public vec3<Type>
	{
		Type w;

		inline bool const operator == (vec1<Type> const& rhs) const	{ return vec3<Type>::operator ==(rhs) && (w == 0); }
		inline bool const operator == (vec2<Type> const& rhs) const { return vec3<Type>::operator ==(rhs) && (w == 0); }
		inline bool const operator == (vec3<Type> const& rhs) const { return vec3<Type>::operator ==(rhs) && (w == 0); }
		inline bool const operator == (vec4<Type> const& rhs) const { return vec3<Type>::operator ==(rhs) && (w == rhs.w); }
	};
}