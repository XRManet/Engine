#pragma once

namespace xr
{
template<typename T>
struct ContiguousReference
{
	uint32_t _size;
	T* _data;

	ContiguousReference() = default;
	ContiguousReference(ContiguousReference const&) = default;

	template<uint32_t size>
	inline ContiguousReference(T (&data)[size]) : _size(size), _data(data) {}
};


template<typename T>
struct ContiguousReference<const T>
{
	uint32_t _size;
	T const* _data;

	ContiguousReference() = default;
	ContiguousReference(ContiguousReference const&) = default;

	template<uint32_t size>
	inline ContiguousReference(T const(&data)[size]) : _size(size), _data(data) {}

	ContiguousReference(std::initializer_list<T> data) : _size(data.size()), _data(data.begin()) {}
};

} // namespace xr