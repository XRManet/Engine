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
		Type _x;

		vec1() : _x(0) {}
		vec1(Type x) : _x(x) {}

		inline bool const operator == (vec1<Type> const& rhs) const { return _x == rhs._x; }
		inline bool const operator == (vec2<Type> const& rhs) const { return rhs == *this; }
		inline bool const operator == (vec3<Type> const& rhs) const { return rhs == *this; }
		inline bool const operator == (vec4<Type> const& rhs) const { return rhs == *this; }
	};

	template<typename Type>
	struct vec2 : public vec1<Type>
	{
		Type _y;

		vec2() : vec1<Type>(), _y(0) {}
		vec2(Type x, Type y) : vec1<Type>(x), _y(y) {}

		inline bool const operator == (vec1<Type> const& rhs) const { return vec1<Type>::operator ==(rhs) && (_y == 0); }
		inline bool const operator == (vec2<Type> const& rhs) const { return vec1<Type>::operator ==(rhs) && (_y == rhs._y); }
		inline bool const operator == (vec3<Type> const& rhs) const { return rhs == *this; }
		inline bool const operator == (vec4<Type> const& rhs) const { return rhs == *this; }
	};

	template<typename Type>
	struct vec3 : public vec2<Type>
	{
		Type _z;

		vec3() : vec2<Type>(), _z(0) {}
		vec3(Type x, Type y, Type z) : vec2<Type>(x, y), _z(z) {}

		inline bool const operator == (vec1<Type> const& rhs) const { return vec2<Type>::operator ==(rhs) && (_z == 0); }
		inline bool const operator == (vec2<Type> const& rhs) const { return vec2<Type>::operator ==(rhs) && (_z == 0); }
		inline bool const operator == (vec3<Type> const& rhs) const { return vec2<Type>::operator ==(rhs) && (_z == rhs._z); }
		inline bool const operator == (vec4<Type> const& rhs) const { return rhs == *this; }
	};

	template<typename Type>
	struct vec4 : public vec3<Type>
	{
		Type _w;

		vec4() : vec3<Type>(), _w(0) {}
		vec4(Type x, Type y, Type z, Type w) : vec3<Type>(x, y, z), _w(w) {}

		inline bool const operator == (vec1<Type> const& rhs) const	{ return vec3<Type>::operator ==(rhs) && (_w == 0); }
		inline bool const operator == (vec2<Type> const& rhs) const { return vec3<Type>::operator ==(rhs) && (_w == 0); }
		inline bool const operator == (vec3<Type> const& rhs) const { return vec3<Type>::operator ==(rhs) && (_w == 0); }
		inline bool const operator == (vec4<Type> const& rhs) const { return vec3<Type>::operator ==(rhs) && (_w == rhs._w); }
	};
}