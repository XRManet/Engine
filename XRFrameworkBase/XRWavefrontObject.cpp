#include "stdafx.h"

#include "XRWavefrontObject.h"

#include <assert.h>

// Run-time parsing은 시간이 오래걸리니 차후 모듈로 분리, 리소스 컴파일 단계를 따로 거치도록 한다.
bool XRWavefrontObject::LoadDataFromFile()
{
	int const MAX_LINE_CHARACTERS = 256;
	char line[MAX_LINE_CHARACTERS] = { 0, };
	FILE* fp = fopen(GetPath().c_str(), "r");
	if (fp == nullptr)
		return false;

	ReadUnit unit;
	ModelHeader *header = GetHeader();

	size_t texture_coordinate_count = 0;
	size_t vertex_normal_count = 0;
	uint32_t element_count_position = 0;
	uint32_t element_count_normal = 0;
	uint32_t element_count_tex = 0;

	struct XRWavefrontObjectSubobject // Per-material
	{
		std::vector<ReadUnit> _positions;
		std::vector<ReadUnit> _texcoords;
		std::vector<ReadUnit> _normals;

		std::vector<uint32_t> _indices;
		std::string _materialName;
	};

	struct XRVertexAttributeType {
		enum {
			Position,
			Texcoord,
			Normal,
			Count
		};
	};

	struct XRWavefrontObjectObject // Wavefront-obj에 포함된 하나의 오브젝트
	{
		// All data in an object
		std::vector<ReadUnit> _positions;
		std::vector<ReadUnit> _texcoords;
		std::vector<ReadUnit> _normals;

		std::vector<XRWavefrontObjectSubobject> _subobjects;

		uint32_t _dimPosition;
		uint32_t _dimTexcoord;
		uint32_t _dimNormal;

		XRWavefrontObjectObject()
		{
			_positions.resize(1);
			_texcoords.resize(1);
			_normals.resize(1);

			_subobjects.resize(1);
		}
	};

	struct XRWavefrontObjectFaceKey
	{
	public:
		union {
			struct {
				uint16_t	_positionIndex;
				uint16_t	_texcoordIndex;
				uint16_t	_normalIndex;
			};
			uint64_t		_compare = 0;
		};

	public: // unordered_map key로 쓰기 위한 operator 재정의
		inline bool operator == (const XRWavefrontObjectFaceKey& _rhs) const
		{
			return _compare == _rhs._compare;
		}
		// std::hash<uint64_t>를 이용하기 위함
		inline operator uint64_t() const { return _compare; }
	};

	std::unordered_map<XRWavefrontObjectFaceKey, uint32_t, std::hash<uint64_t>> _indices;
	std::vector<XRWavefrontObjectObject> objects;
	objects.push_back(XRWavefrontObjectObject());
	// Quad mesh는 tri mesh로 둘로 쪼개서 기록

	XRWavefrontObjectObject* currentObject = &objects.back();
	XRWavefrontObjectSubobject* currentSubobject = &currentObject->_subobjects.back();
	bool hasManyObjects = false;
	bool hasManySubobjects = false;

	while (fgets(line, MAX_LINE_CHARACTERS, fp) != nullptr)
	{
		static const float default_value[4] = { 0.f, 0.f, 0.f, 1.f };

		int read_pos = 2;
		int available_count = 0;
		int * p_count = nullptr;

		size_t size = _memory.size();

		if (line[0] == 'm' && strncmp(line, "mtllib", sizeof("mtllib") - 1) == 0) // mtllib
		{

		}
		else if (line[0] == 'o')
		{
			if (hasManyObjects)
			{
				objects.push_back(XRWavefrontObjectObject());
				currentObject = &objects.back();
				hasManySubobjects = false;
			}
			else hasManyObjects = true;
		}
		else if (line[0] == 'v')
		{
			available_count = sscanf(line + read_pos,
				"%f %f %f %f",
				unit.f + 0, unit.f + 1, unit.f + 2, unit.f + 3);

			std::vector<ReadUnit>* target = nullptr;
			switch (line[1])
			{
			case ' ': target = &currentObject->_positions;	break; // vertex position
			case 't': target = &currentObject->_texcoords;	break; // texture coordinate
			case 'n': target = &currentObject->_normals;	break; // vertex normal
			case 'p': assert(0); break; // parameter space coordinate
			}
			assert(target != nullptr);

			target->push_back(unit);
		}
		else if (line[0] == 'u' && strncmp(line, "usemtl", sizeof("usemtl") - 1) == 0) // usemtl
		{
			char buffer[256];
			sscanf(line + sizeof("usemtl"), "%s", buffer);
			
			if (hasManySubobjects)
			{
				currentObject->_subobjects.push_back(XRWavefrontObjectSubobject());
				currentSubobject = &currentObject->_subobjects.back();
			}
			else hasManySubobjects = true;

			currentSubobject->_materialName = buffer;
		}
		else if (line[0] == 's') // smooth
		{

		}
		else if (line[0] == 'f') // Face element
		{
			// 1 2 3 4 5 ...
			// 123 134 145

			std::vector<char*> vertices;
			char* context = nullptr;
			char* token = strtok_s(line + read_pos, " \r\n", &context);
			do {
				vertices.push_back(token);
			} while ((token = strtok_s(nullptr, " \r\n", &context)) != nullptr);

			uint32_t size = vertices.size();
			uint32_t vertexIds[8] = { 0, };
			for (uint32_t i = 0; i < 4; ++i) unit.i[i] = 0;
			for (uint32_t i = 0; i < size; ++i)
			{
				available_count = sscanf(vertices[i], "%d %d %d %d", unit.i + 0, unit.i + 1, unit.i + 2, unit.i + 3);

				assert(available_count > 0 && available_count < 3);
				assert(available_count < 1 || unit.i[XRVertexAttributeType::Position] < currentObject->_positions.size());
				assert(available_count < 2 || unit.i[XRVertexAttributeType::Texcoord] < currentObject->_texcoords.size());
				assert(available_count < 3 || unit.i[XRVertexAttributeType::Normal] < currentObject->_normals.size());

				XRWavefrontObjectFaceKey faceKey;
				faceKey._positionIndex = unit.i[XRVertexAttributeType::Position];
				faceKey._texcoordIndex = unit.i[XRVertexAttributeType::Texcoord];
				faceKey._normalIndex = unit.i[XRVertexAttributeType::Normal];

				auto result = _indices.find(faceKey);
				if (result == _indices.end())
				{
					vertexIds[i] = _indices.size();

					currentSubobject->_positions.push_back(currentObject->_positions[unit.i[XRVertexAttributeType::Position]]);
					currentSubobject->_texcoords.push_back(currentObject->_texcoords[unit.i[XRVertexAttributeType::Texcoord]]);
					currentSubobject->_normals.push_back(currentObject->_normals[unit.i[XRVertexAttributeType::Normal]]);
				}
				else vertexIds[i] = result->second;
			}

			uint32_t triangles = (size - 1) / 2;
			for (uint32_t i = 0; i < triangles; ++i)
			{
				currentSubobject->_indices.push_back(vertexIds[0]);
				currentSubobject->_indices.push_back(vertexIds[i * 2 + 1]);
				currentSubobject->_indices.push_back(vertexIds[i * 2 + 2]);
			}
		}
		else if (line[0] == 'l') assert(0);
		else if (line[0] == 'g') assert(0);
	}

	assert(texture_coordinate_count == 0 || texture_coordinate_count == header->vertex_count);
	assert(vertex_normal_count == 0 || vertex_normal_count == header->vertex_count);

	fclose(fp);

	return true;
}
