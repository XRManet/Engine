#include "stdafx.h"

#include "XRWavefrontObject.h"

#include <assert.h>

#include "XRModel.h"

#include "XRHash.h"

// Per-material sub object.
// Note(jiman): 
struct XRWavefrontObjectSubmeshes
{
	XRWavefrontObjectSubmeshes()
	{

	}
	XRWavefrontObjectSubmeshes(XRWavefrontObjectSubmeshes&& rhs)
		: _indices(std::move(rhs._indices))
		, _materialName(std::move(rhs._materialName))
	{
		constexpr uint32_t num = sizeof(_vertices) / sizeof(_vertices[0]);
		for (uint32_t i = 0; i < num; ++i)
			_vertices[i] = std::move(rhs._vertices[i]);
	}
	~XRWavefrontObjectSubmeshes()
	{
	}
#if XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_SOA
	union {
		struct {
			std::vector<ReadUnit> _positions;
			std::vector<ReadUnit> _texcoords;
			std::vector<ReadUnit> _normals;
		};
		std::vector<ReadUnit> _vertices[3] = { std::vector<ReadUnit>(), std::vector<ReadUnit>(), std::vector<ReadUnit>() };
	};
#elif XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_AOS
	std::vector<ReadUnit> _vertices[1] = { std::vector<ReadUnit>() };
#endif
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

// An object formatted by WavefrontOBJ.
// Note(jiman): wavefront format data를 load하고 추가적인 가공을 거치지 않음.
struct XRWavefrontObjectMeshes
{
	// All data in an object
	std::vector<ReadUnit> _positions;
	std::vector<ReadUnit> _texcoords;
	std::vector<ReadUnit> _normals;

	std::vector<XRWavefrontObjectSubmeshes> _submeshes;

	uint32_t _numComponentsPosition = 0;
	uint32_t _numComponentsTexcoord = 0;
	uint32_t _numComponentsNormal = 0;

	XRWavefrontObjectMeshes()
	{
		_positions.resize(1);
		_texcoords.resize(1);
		_normals.resize(1);

		_submeshes.resize(1);
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

// Run-time parsing은 시간이 오래걸리니 차후 모듈로 분리, 리소스 패킹 단계를 따로 거치도록 한다.
bool XRWavefrontObject::LoadDataFromFile()
{
	int const MAX_LINE_CHARACTERS = 256;
	char line[MAX_LINE_CHARACTERS] = { 0, };
	FILE* fp = nullptr;
	errno_t error = fopen_s(&fp, GetPath().c_str(), "r");
	if (error != 0 || fp == nullptr)
		return false;

	ReadUnit unit;

	size_t texture_coordinate_count = 0;
	size_t vertex_normal_count = 0;
	uint32_t element_count_position = 0;
	uint32_t element_count_normal = 0;
	uint32_t element_count_tex = 0;

	std::unordered_map<XRWavefrontObjectFaceKey, uint32_t, std::hash<uint64_t>> indices;
	std::vector<XRWavefrontObjectMeshes> objects;
	objects.push_back(XRWavefrontObjectMeshes());
	// Quad mesh는 tri mesh로 둘로 쪼개서 기록

	XRWavefrontObjectMeshes* currentObject = &objects.back();
	XRWavefrontObjectSubmeshes* currentSubobject = &currentObject->_submeshes.back();
	bool hasManyObjects = false;
	bool hasManySubobjects = false;

	while (fgets(line, MAX_LINE_CHARACTERS, fp) != nullptr)
	{
		static const float default_value[4] = { 0.f, 0.f, 0.f, 1.f };
		static const uint32_t default_index[4] = { 0, };

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
				objects.push_back(XRWavefrontObjectMeshes());
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
			for (int32_t i = available_count; i < 4; ++i)
				unit.f[i] = default_value[i];

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
				currentObject->_submeshes.emplace_back();
				currentSubobject = &currentObject->_submeshes.back();
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
			char* token = xr::strtok(line + read_pos, " \r\n", &context);
			do {
				vertices.push_back(token);
			} while ((token = xr::strtok(nullptr, " \r\n", &context)) != nullptr);

			static const uint32_t MAX_NUM_VERTICES_IN_FACE = 8;
			uint32_t vertexIds[MAX_NUM_VERTICES_IN_FACE] = { 0, };
			uint32_t size = static_cast<uint32_t>(vertices.size());

			for (uint32_t i = 0; i < size; ++i)
			{
				// Note(jiman): Wavefront는 각 attribute를 /로 구분
				// Todo(jiman): 공란에 대한 처리(ex. 3//4) 필요
				available_count = sscanf(vertices[i], "%d/%d/%d/%d", unit.i + 0, unit.i + 1, unit.i + 2, unit.i + 3);
				for (int32_t i = available_count; i < 4; ++i)
					unit.i[i] = default_index[i];

				// Note(jiman): Wavefront의 한 vertex에 대해 /로 구분 가능한 vertex attribute는 최대 3개.
				assert(available_count > 0 && available_count <= 3);
				if (available_count == 0 || available_count > 3)
					continue;
				// Note(jiman): 각 vertex attribute의 유효 index 여부 확인
				assert(available_count < 1 || unit.i[XRVertexAttributeType::Position] < currentObject->_positions.size());
				assert(available_count < 2 || unit.i[XRVertexAttributeType::Texcoord] < currentObject->_texcoords.size());
				assert(available_count < 3 || unit.i[XRVertexAttributeType::Normal] < currentObject->_normals.size());

				XRWavefrontObjectFaceKey faceKey;
				faceKey._positionIndex = unit.i[XRVertexAttributeType::Position];
				faceKey._texcoordIndex = unit.i[XRVertexAttributeType::Texcoord];
				faceKey._normalIndex = unit.i[XRVertexAttributeType::Normal];

				//_indices.
				auto result = indices.find(faceKey);

				if (result == indices.end())
				{
					vertexIds[i] = static_cast<uint32_t>(indices.size());
					indices.insert({ faceKey, vertexIds[i] });

#if XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_SOA
					/* Note(jiman): 모델 데이터는 데이터를 최대한 빠른 시간 내에 올리는 목적으로 만들어져야 하므로,
					 *				차후엔 데이터 재구성을 통해 별도의 헤더를 마련하고 이를 통해 InputLayout을 결정받아
					 *				데이터를 읽도록 할 것.
					 *				현재 단계에선 단순 처리로 진행한다.
					 */
					 /* Note(jiman): Meshes에 기록된 데이터는 VertexAttributeData 이므로 유효한 Vertex로 묶여야만 의미를 갖는다.
					  */

					currentSubobject->_positions.push_back(currentObject->_positions[unit.i[XRVertexAttributeType::Position]]);
					if (available_count > 1)
						currentSubobject->_texcoords.push_back(currentObject->_texcoords[unit.i[XRVertexAttributeType::Texcoord]]);
					if (available_count > 2)
						currentSubobject->_normals.push_back(currentObject->_normals[unit.i[XRVertexAttributeType::Normal]]);
#endif
				}
				else vertexIds[i] = result->second;
			}

			uint32_t triangles = (size - 2);
			assert(triangles > 0);
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

	// Todo(jiman): vertex attribute의 갯수 비교는 sub object 별로 처리해야 함
	//assert(texture_coordinate_count == 0 || texture_coordinate_count == header->vertex_count);
	//assert(vertex_normal_count == 0 || vertex_normal_count == header->vertex_count);

	fclose(fp);

	return ProduceXRModelData(objects);
}

bool XRWavefrontObject::ProduceXRModelData(std::vector<XRWavefrontObjectMeshes>& objects)
{
	static std::unordered_map<uint32_t, XRInputLayout*> s_inputLayoutLibrary;
	static std::unordered_map<uint32_t, XRMaterial> s_materialLibrary;
	// Note(jiman): Fill to model data
	{
		std::vector<XRVertexBufferDesc> vertexBufferDescs;
		XRVertexBufferDesc vertexBufferDesc;
		XRVertexAttributeDesc vertexAttributeDesc;
		
		// Construct input layout desc
#if XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_SOA
		{	// per vertex buffer
			vertexBufferDesc.instanceDivisor = 0;
			
			vertexAttributeDesc.format = XRFormat::R32G32B32_SFLOAT;
			vertexBufferDesc.attributes.push_back(vertexAttributeDesc);
			vertexBufferDescs.push_back(std::move(vertexBufferDesc));
			
			vertexAttributeDesc.format = XRFormat::R32G32_SFLOAT;
			vertexBufferDesc.attributes.push_back(vertexAttributeDesc);
			vertexBufferDescs.push_back(std::move(vertexBufferDesc));
			
			vertexAttributeDesc.format = XRFormat::R32G32B32_SFLOAT;
			vertexBufferDesc.attributes.push_back(vertexAttributeDesc);
			vertexBufferDescs.push_back(std::move(vertexBufferDesc));
		}
#elif XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_AOS
		{
			vertexBufferDesc.instanceDivisor = 0;

			// for each vertex attribute in vb
			vertexAttributeDesc.format = XRFormat::R32G32B32_SFLOAT;
			vertexBufferDesc.attributes.push_back(vertexAttributeDesc);
			vertexAttributeDesc.format = XRFormat::R8G8_UNORM;
			vertexBufferDesc.attributes.push_back(vertexAttributeDesc);
			vertexAttributeDesc.format = XRFormat::R32G32B32_SFLOAT;
			vertexBufferDesc.attributes.push_back(vertexAttributeDesc);

			vertexBufferDescs.push_back(std::move(vertexBufferDesc));
		}
#endif
		
		XRInputLayoutDesc inputLayoutDesc(std::move(vertexBufferDescs));
		uint32_t inputLayoutDescKey = inputLayoutDesc.getHash();
		XRInputLayout* inputLayout = nullptr;
		
		auto ii = s_inputLayoutLibrary.find(inputLayoutDescKey);
		if (ii != s_inputLayoutLibrary.end())
			inputLayout = ii->second;
		else
		{
			inputLayout = xrCreateInputLayout(std::move(inputLayoutDesc), 0);
			s_inputLayoutLibrary.insert({inputLayoutDescKey, inputLayout});
		}
		const uint32_t numVertexBuffers = inputLayout->getNumVertexBuffers();
		
		XRObjectHeader *header = GetHeader();
		size_t totalDataSize = sizeof(XRObjectHeader);
		
		header->_numMeshes = static_cast<uint32_t>(objects.size());
		totalDataSize += (sizeof(XRMeshHeader) + sizeof(XRMeshHeader*)) * header->_numMeshes;

		for (uint32_t i = 0; i < header->_numMeshes; ++i)
		{
			uint32_t numSubmeshes = static_cast<uint32_t>(objects[i]._submeshes.size());
			totalDataSize += (sizeof(XRSubmeshHeader) + (sizeof(uint32_t) * numVertexBuffers) + sizeof(XRSubmeshHeader*)) * numSubmeshes;
			
			for (uint32_t j = 0; j < numSubmeshes; ++j)
			{
				const uint32_t numMaterials = 1;
				totalDataSize += numMaterials * sizeof(uint32_t);

				totalDataSize += static_cast<uint32_t>(sizeof(uint16_t) * objects[i]._submeshes[j]._indices.size());
				
				for (uint32_t k = 0; k < numVertexBuffers; ++k)
				{
					if (objects[i]._submeshes[j]._vertices[k].size() == 0)
						continue;
					
					uint32_t bufferSize = static_cast<uint32_t>(inputLayout->getStride(k) * objects[i]._submeshes[j]._vertices[k].size());
					totalDataSize += bufferSize;
					//totalDataSize += (bufferSize + 3 & (~3u)); // 4byte-align
				}
			}
		}

		_memory.resize(totalDataSize);
		header = GetHeader();
		XRMeshHeader** meshHeaders = header->_meshes;
		PrimitiveTopology topology(PrimitiveTopology::TriangleList);
		uint8_t* meshBaseAddress = reinterpret_cast<uint8_t*>(header->_meshes);
		uint32_t meshOffset = sizeof(XRMeshHeader) * header->_numMeshes;
		for (uint32_t i = 0; i < header->_numMeshes; ++i)
		{
			uint32_t numSubmeshes = static_cast<uint32_t>(objects[i]._submeshes.size());

			meshBaseAddress = meshBaseAddress + meshOffset;
			meshOffset = 0;

			meshHeaders[i] = reinterpret_cast<XRMeshHeader*>(meshBaseAddress);
			
			meshHeaders[i]->_topology = topology; // Temp
			meshHeaders[i]->_numSubmeshes = numSubmeshes;

			XRSubmeshHeader** submeshHeader = meshHeaders[i]->_submeshes;
			uint8_t* submeshBaseAddress = reinterpret_cast<uint8_t*>(submeshHeader);
			uint32_t submeshOffset = sizeof(XRSubmeshHeader*) * numSubmeshes;
			for (uint32_t j = 0; j < numSubmeshes; ++j)
			{
				const uint32_t numMaterials = 1; // Temp

				submeshBaseAddress = reinterpret_cast<uint8_t*>(submeshBaseAddress + submeshOffset);
				submeshOffset = 0;

				submeshHeader[j] = reinterpret_cast<XRSubmeshHeader*>(submeshBaseAddress);
				
				submeshHeader[j]->_inputLayoutKey = inputLayoutDescKey;
				submeshHeader[j]->_numMaterials = numMaterials;

				submeshOffset += sizeof(XRSubmeshHeader) + (sizeof(submeshHeader[j]->_offsetVertexOffsets[0]) * numVertexBuffers);
				submeshHeader[j]->_offsetMaterialKeys = submeshOffset;

				uint32_t* const materialKeys = submeshHeader[j]->getMaterialKeys();
				for(uint32_t k = 0; k < numMaterials; ++k)
				{
					materialKeys[k] = GetHash(objects[i]._submeshes[j]._materialName.data(), objects[i]._submeshes[j]._materialName.length());
				}

				submeshOffset += sizeof(uint32_t) * numMaterials;
				submeshHeader[j]->_offsetIndex = submeshOffset;

				uint16_t* const indexBuffer = submeshHeader[j]->getIndexBuffer();
				uint32_t bufferSize = static_cast<uint32_t>(sizeof(uint16_t) * objects[i]._submeshes[j]._indices.size());
				memcpy(indexBuffer, objects[i]._submeshes[j]._indices.data(), bufferSize);
				submeshOffset += bufferSize;

				for (uint32_t k = 0; k < numVertexBuffers; ++k)
				{
					submeshHeader[j]->_offsetVertexOffsets[k] = submeshOffset;

					uint8_t* const vertexBuffer = submeshHeader[j]->getVertexBuffer(k);
					if (objects[i]._submeshes[j]._vertices[k].size() == 0)
						continue;

					uint32_t strideSize = inputLayout->getStride(k);
					uint32_t numVertices = objects[i]._submeshes[j]._vertices[k].size();
					bufferSize = static_cast<uint32_t>(strideSize * numVertices);
					for (uint32_t l = 0; l < numVertices; ++l)
					{
						memcpy(vertexBuffer + l * strideSize, &objects[i]._submeshes[j]._vertices[k][l], strideSize);
					}
					//memcpy(vertexBuffer, objects[i]._submeshes[j]._vertices[k].data(), bufferSize);
					submeshOffset += bufferSize;
				}
				submeshHeader[j]->_offsetVertexOffsets[numVertexBuffers] = submeshOffset;
			}
			submeshBaseAddress = reinterpret_cast<uint8_t*>(submeshBaseAddress + submeshOffset);
			meshOffset = submeshBaseAddress - meshBaseAddress;
		}
		meshBaseAddress = meshBaseAddress + meshOffset;
		assert(totalDataSize == meshBaseAddress - reinterpret_cast<uint8_t*>(header));
	}

	return true;
}
