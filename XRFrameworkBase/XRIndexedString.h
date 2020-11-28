#pragma once

#include "stdafx.h"

#include <memory.h>
#include <unordered_map>
#include <vector>
#include <string>

#include <XRFrameworkBase/XRLock.h>

namespace xr
{
struct DefaultStringCategory {};
template<typename Category = DefaultStringCategory, typename Capacity = uint16_t>
class IndexedString
{
public:
	IndexedString()
		: _index(0)
	{
	}

	IndexedString(IndexedString const& rhs)
		: _index(rhs._index)
	{
	}

	IndexedString(IndexedString&& rhs)
		: _index(rhs._index)
	{
		rhs._index = 0;
	}

	IndexedString(std::string const& string)
	{
		ScopedLocker<MutexLock> locker(sMutexLock);
		auto found = sAllStrings.find(string);
		if (found != sAllStrings.end())
			_index = found->second;
		else
		{
			_index = sIndexCounter++;
			sAllStrings.insert({string, _index});
		}
	}

public:
	inline bool operator == (IndexedString const& rhs) const { return (_index == rhs._index); }
	inline bool operator != (IndexedString const& rhs) const { return (_index != rhs._index); }

	friend std::hash<xr::IndexedString<Category, Capacity>>;

public:
	inline char const* c_str() const { return sAllAccessors[_index]->c_str(); }
	inline size_t length() const { return sAllAccessors[_index]->length(); }

private:
	Capacity _index = 0;

private:
	XRBaseExport static std::unordered_map<std::string, Capacity> sAllStrings;
	XRBaseExport static std::vector<std::string*> sAllAccessors;
	XRBaseExport static Capacity sIndexCounter;
	XRBaseExport static MutexLock sMutexLock;
};

}

/**
 * @struct	std::hash<xr::IndexedString<Category,Capacity>>
 *
 * @brief	An hash for xr::IndexedString<Category,Capacity>. std::hash<Capacity> 와 동일
 *
 * @author	Jiman Jeong
 * @date	2020-11-28
 *
 * @tparam	Category	Type of the category.
 * @tparam	Capacity	Type of the capacity.
 */

template<typename Category, typename Capacity>
struct std::hash<xr::IndexedString<Category, Capacity>>
{
	inline size_t operator () (xr::IndexedString<Category, Capacity> const& indexedString) const
	{
		return std::hash<Capacity>()(indexedString._index);
	}
};
