#include "stdafx.h"

#define XRRENDER_ENGINE			XR_DYNAMIC_LIBRARY(XRRenderEngineGL)
#include <XRFrameworkBase/XRRenderEngineLinker.hpp>

#define XRSHADER_BUILD_SYSTEM	XR_DYNAMIC_LIBRARY(XRSourceBuildSystemGLSL)
#include <XRFrameworkBase/XRSourceBuildSystemLinker.hpp>

#include <unordered_set>

#include "XRSceneTest.h"

#define COMPILE_CPP
#include "Shader/DeferredSample.h"

#include <XRFrameworkBase/XRLogging.h>
#include <XRFrameworkBase/XRUtility.h>

#include <XRFrameworkBase/XRObject.h>
#include <XRFrameworkBase/XRActorNode.h>
#include <XRFrameworkBase/XRTransformNode.h>

#include <GL/glew.h>

#include <stdio.h>

// Todo) Menifest를 별도로 둬서 scene에서 읽을 수 있는 형식을 갖출 것
XRSceneTest::XRSceneTest()
{
	_cameras.resize(1);

	_cameras[0].SetFrustum({ 16, 9 }, 1, 10000);
	_cameras[0].SetPosition(glm::vec4{ 0, 1, 2.5, 1 });

	XRActorNode* actor = new XRActorNode();
	XRTransformNode* transform = new XRTransformNode();

	_root->AddChild(actor);

	auto model = _resource_manager.GetModelByKey("wolf");
	actor->bindModel(model);
	actor->AddChild(transform);

	glm::vec4 position{ -2, 0, 0, 1 };
	transform->SetPosition(position);
}

XRSceneTest::~XRSceneTest()
{
}

void XRSceneTest::Initialize()
{
}

void XRSceneTest::Update(float dt)
{
	glm::vec3 cameraMove{
		(_input->keyboardPressed['D'] - _input->keyboardPressed['A']),
		(_input->keyboardPressed['E'] - _input->keyboardPressed['Q']),
		(_input->keyboardPressed['S'] - _input->keyboardPressed['W'])
	};
	glm::vec3 alignedMove = glm::mat3_cast(_cameras[0].GetQuaternion()) * cameraMove * _input->cameraStep;
	_cameras[0].Move(alignedMove);

	static bool anchorOrientation = false;
	static glm::quat orientation;
	if (true == _input->anchored)
	{
		static const auto up = glm::vec3(0, 1, 0);
		static const auto right = glm::vec3(1, 0, 0);

		if (false == anchorOrientation)
		{
			orientation = _cameras[0].GetQuaternion();
			anchorOrientation = true;
		}
		auto axisXangle = glm::radians(-float(_input->curX - _input->anchorX)) * _input->cameraStep;
		auto axisYangle = glm::radians(float(_input->curY - _input->anchorY)) * _input->cameraStep;

		auto rotation = glm::rotate(glm::rotate(orientation, axisXangle, up), axisYangle, right);
		_cameras[0].SetQuaternion(rotation);
	}
	else
	{
		anchorOrientation = false;
	}
}



#if 0
/**********************************
 * CommandSetter Test
 */
template<typename name_tuple, typename data_tuple>
struct BaseCommandSetter
{
	BaseCommandSetter(GLuint program, name_tuple& names, const char* name, data_tuple& data)
	{
		constexpr int index = XR::util::GetIndexOfLiteralStringList<name_tuple>(names, name);
		static int bindingLocation = glGetUniformLocation(program, name);
		glUniform1f(bindingLocation, std::get<index>(data));
	}
};

template<typename name_tuple, typename data_tuple, typename var_type>
void SetVariable(GLuint program, const name_tuple names, data_tuple data, const char* name, var_type value)
{
	int index = XR::util::GetIndexOfLiteralStringList<name_tuple>(names, name);
	static int bindingLocation = glGetUniformLocation(program, name);

	std::get<index>(data) = value;
	glUniform1f(bindingLocation, value);
}

void Test()
{
	GLuint program = 9;
	glm::mat4 mvp = glm::identity<glm::mat4>();

	auto data = std::make_tuple(0.1f, 5, mvp);

	SetVariable(program, g_deferredVar, data, "var_test1", 0.2f);

	//constexpr int index = XR::util::GetIndexOfLiteralStringList(g_deferredVar, "var_test1");
	//static int bindingLocation = glGetUniformLocation(program, "var_test1");
	//glUniform1f(bindingLocation, std::get<index>(data));
}
#endif