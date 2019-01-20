#include "stdafx.h"

#include "XRLoadable.h"

XRLoadable::XRLoadable(std::string && path) : _path(std::move(path))
{}

XRLoadable::XRLoadable(std::string const & path) : _path(path)
{}
