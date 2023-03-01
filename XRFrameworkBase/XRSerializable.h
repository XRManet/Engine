#pragma once

class XRSerializable
{
public:
  virtual bool SerializeDataIntoFile() { return false; }
};
