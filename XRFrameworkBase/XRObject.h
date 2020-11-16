#pragma once

#include "stdafx.h"

#include "XRGeometry.h"
#include "XRType.h"

// Declared from here
class XRInteractive;
class XRObject;
class XRCamera;
class XRBackground;

class XRNonInteractive;
class XREffect;


class XRBaseExport XRInteractive
{
public:
};

class XRBaseExport XRBackground : XRInteractive
{
public:
};

static const int64_t    NO_ID = static_cast<int64_t>(-1);

class XRBaseExport XRObject : XRInteractive
{
private:
	int64_t   _id = NO_ID;
	void*     _context = nullptr;

	glm::vec4 _position{};
	glm::quat _orientation{ glm::identity<glm::quat>() };

public:
	XRObject() {}
	virtual ~XRObject() {}

public:
	void SetPosition(const glm::vec4& position) { _position = position; }
	void SetQuaternion(const glm::quat& quaternion) { _orientation = quaternion; }

	glm::quat GetQuaternion() const { return _orientation; }


public:
	void Move(const glm::vec3& distance) { _position += glm::vec4(distance, 0); }
	void Rotate(float angle, const glm::vec3& v) { _orientation = glm::rotate(_orientation, angle, v); }


public:
	glm::mat4 GetTransform() const {
		glm::mat4 transform = glm::mat4_cast(_orientation);
		transform[3] = _position;
		return transform;
	}

	glm::mat4 GetInvTransform() const {
		glm::mat4 inv_transform = glm::transpose(glm::mat4_cast(_orientation));
		inv_transform[3] = -(inv_transform * _position);
		inv_transform[3][3] = 1.f;
		return inv_transform;
	}
};

typedef enum XRCameraProjectionMode {
	Perspective,
	Orthogonal,
	Size
} CPM;

class XRBaseExport XRCamera : public XRObject
{
private:
	XRSize<float> _half_size{};
	float _near_positive{};
	float _far_positive{};
	CPM _projection_mode{ CPM::Perspective };


public:
	CPM GetProjectionMode() { return _projection_mode; }
	void SetProjectionMode(const CPM& projection_mode) { _projection_mode = projection_mode; }

public:
	glm::mat4 GetProjectionTransform() const {
		return glm::perspective(glm::radians(60.f), _half_size._width / _half_size._height, _near_positive, _far_positive);
		return glm::frustum(-_half_size._width, _half_size._width,
			-_half_size._height, _half_size._height,
			_near_positive, _far_positive);
		return glm::perspective(atan(_half_size._height / _near_positive) * 2, _half_size._width / _half_size._height, _near_positive, _far_positive);
	}

	void SetFrustum(const XRSize<float>& size, float near_positive, float far_positive) {
		_half_size._width = size._width / 2.f;
		_half_size._height = size._height / 2.f;

		_near_positive = near_positive;
		_far_positive = far_positive;
	}
};

class XRScene;

class XRBaseExport XRViewport
{
private:
	XRBound<float> _viewport;

private:
	std::vector<XRScene*> _scenes;
};

class XRBaseExport XRNonInteractive
{
public:
};

class XRBaseExport XREffect : XRNonInteractive
{
public:
};