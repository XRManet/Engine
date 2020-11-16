#pragma once

#include "stdafx.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

template<typename T>
struct XRBaseExport XRSize {
    T _width;
    T _height;
};

template<typename T>
struct XRBaseExport XRPosition {
    T _x;
    T _y;
};

template<typename T>
struct XRBaseExport XRBound {
    union {
        struct {
            XRPosition<T> _position;
            XRSize<T> _size;
        };
        struct {
            T _x;
            T _y;
            T _width;
            T _height;
        };
    };
};

#if defined(_M_X64) || defined(_M_AMD64)
#else
//#error "TODO: XRGeometry is not defined for all type of architectures but only x64 or amd64"
#endif
