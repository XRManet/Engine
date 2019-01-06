#include "stdafx.h"

#include "XRScene.h"

#include <assert.h>

static int const PAGE_SIZE = 4'096;

XRLoadable::XRLoadable(std::string && path) : _path(std::move(path))
{}

XRLoadable::XRLoadable(std::string const & path) : _path(path)
{}

void XRModelData::Initialize()
{
  _memory.reserve(PAGE_SIZE);

  _memory.resize(sizeof(ModelHeader));
  new (GetHeader()) ModelHeader;
}

bool XRWavefrontObject::LoadDataFromFile()
{
  int const MAX_LINE_CHARACTERS = 256;
  char line[MAX_LINE_CHARACTERS] = {0, };
  FILE* fp = fopen(GetPath().c_str(), "r");
  ReadUnit unit;
  ModelHeader *header = GetHeader();

  while (fgets(line, MAX_LINE_CHARACTERS, fp) != nullptr)
  {
    static const float default_value[4] = { 0.f, 0.f, 0.f, 1.f };

    int read_pos = 2;
    int available_count = 0;
    int * p_count = nullptr;

    if (line[0] == 'v')
    {
      if (line[1] == ' ') // vertex position
      {
        header->vertex_count++;

        if (header->vertex_offset == 0) {
          header->vertex_offset = _memory.size();
        }
      }
      else if (line[1] == 't') // texture coordinate
      {
        header->texture_count++;

        if (header->texture_offset == 0) {
          header->texture_offset = _memory.size();
        }
      }
      else if (line[1] == 'n') // vertex normal
      {
        header->normal_count++;

        if (header->normal_offset == 0) {
          header->normal_offset = _memory.size();
        }
      }
      else if (line[1] == 'p') // parameter space coordinate
      {
        assert(0);
      }

      available_count = sscanf(line + read_pos,
        "%f %f %f %f",
        unit.f + 0, unit.f + 1, unit.f + 2, unit.f + 3);

      for (int i = available_count; i < 4; ++i)
        unit.f[i] = default_value[i];

      if (_memory.size() + sizeof(unit) > _memory.capacity()) {
        _memory.reserve(_memory.capacity() * 2);
      }
      _memory.resize(_memory.size() + sizeof(unit));
      memcpy(_memory.data() + _memory.size(), &unit, sizeof(unit));
    }
    else if (line[0] == 'f') // Face element
    {
      if (header->index_offset == 0) {
        header->index_offset = _memory.size();
      }

      available_count = sscanf(line + read_pos,
        "%d %d %d %d",
        unit.i + 0, unit.i + 1, unit.i + 2, unit.i + 3);

      header->index_count += available_count;
      
      size_t index_buffer_size = available_count * sizeof(int);
      if (_memory.size() + index_buffer_size > _memory.capacity()) {
        _memory.reserve(_memory.capacity() * 2);
      }
      _memory.resize(_memory.size() + index_buffer_size);
      memcpy(_memory.data() + _memory.size(), &unit, index_buffer_size);
    }
    else if (line[0] == 'l') assert(0);
    else if (line[0] == 'o') assert(0);
    else if (line[0] == 'g') assert(0);
    else if (line[0] == 's') assert(0);
  }

  fclose(fp);

  return true;
}

XRModel* XRModel::GenerateModelFromData(XRModelData* loadableData)
{
  XRModel* model = nullptr;
  if (loadableData->LoadDataFromFile() == true)
  {
    model = new XRModel(loadableData);
  }

  return model;
}

XRModel::XRModel(XRModelData* lodableData) : _data(lodableData)
{

}

XRResourceManager::XRResourceManager()
{
  // TODO) Parse manifest for resources and enroll those.
  XRModelData * loadable = new XRWavefrontObject("../Resources/teapot.obj");
  XRModel * model = XRModel::GenerateModelFromData(loadable);
}

XRScene::XRScene()
{
}


XRScene::~XRScene()
{
}