#include "stdafx.h"

#include "XRWavefrontObject.h"

#include <assert.h>

bool XRWavefrontObject::LoadDataFromFile()
{
  int const MAX_LINE_CHARACTERS = 256;
  char line[MAX_LINE_CHARACTERS] = { 0, };
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
        header = GetHeader();
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
        header = GetHeader();
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
