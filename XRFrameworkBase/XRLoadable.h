#pragma once

#include "XRIndexedString.h"

// TODO) File Manager
class XRLoadable
{
private:
	xr::IndexedString<XRLoadable, uint32_t> _path;


protected:
	std::vector<unsigned char> _memory;


public:
	XRLoadable(std::string && path);
	XRLoadable(std::string const & path);


public:
	virtual bool LoadDataFromFile() { return false; }
	virtual bool ResetData() { return false; }

	virtual bool Write() { return false; }

public:
	std::string const& GetPath() const { return _path.string(); }
	const uint8_t* GetData() const { return _memory.data(); }
};
