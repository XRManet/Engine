#pragma once

#include <stdafx.h>

// TODO) File Manager
class XRLoadable
{
private:
	std::string _path;


protected:
	std::vector<unsigned char> _memory;


public:
	XRLoadable(std::string && path);
	XRLoadable(std::string const & path);


protected:
	std::string const & GetPath() const { return _path; }


public:
	virtual bool LoadDataFromFile() { return false; }
	virtual bool ResetData() { return false; }

	virtual bool Write() { return false; }

public:
	const uint8_t* GetData() const { return _memory.data(); }
};
