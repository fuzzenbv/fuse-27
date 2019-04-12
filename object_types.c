#include "object_types.h"

objCreate * createObj(uint8_t size, char * name)
{
  objCreate * obj = (objCreate*)malloc(sizeof(objCreate));
  obj->type = 5;
  obj->size = size;
  obj->name = name;
  return obj;
}

objRead * createObjRead(uint8_t size, char * name, uint64_t offset, uint64_t size_read)
{
  objRead * obj = (objRead*)malloc(sizeof(objRead));
  obj->type = 7;
  obj->size = size;
  obj->name = name;
  obj->offset = offset;
  obj->size_read = size_read;
  return obj;
}

objWrite * createObjWrite(uint8_t size, char * name, uint64_t offset, uint64_t size_write, char * data)
{
  objWrite * obj = (objWrite*)malloc(sizeof(objWrite));
  obj->type = 9;
  obj->size = size;
  obj->name = name;
  obj->offset = offset;
  obj->size_write = size_write;
  obj->data = data;
  return obj;
}

objTrunc * createObjTrunc(uint8_t size, char * name, uint64_t tsize)
{
  objTrunc * obj = (objTrunc*)malloc(sizeof(objTrunc));
  obj->type = 13;
  obj->size = size;
  obj->name = name;
  obj->tsize = tsize;
  return obj;
}


objSize * createObjSize(uint8_t size, char * name)
{
  objSize * obj = malloc(sizeof(objSize));
  obj->type = 11;
  obj->size = size;
  obj->name = malloc(size);
  obj->name = name;
  return obj;
}

sizeAck * createAckSize(uint64_t size, uint8_t block)
{
  sizeAck * obj = malloc(sizeof(sizeAck));
  obj->type = 12;
  obj->size = size;
  obj->block = block;
  return obj;
}

ReadAck * createAckRead(uint64_t size, char * data)
{
  ReadAck * obj = malloc(sizeof(ReadAck));
  obj->type = 8;
  obj->size = size;
  obj->data = data;
  return obj;
}

WriteAck * createAckWrite(uint64_t size_write)
{
  WriteAck * obj = malloc(sizeof(WriteAck));
  obj->type = 10;
  obj->size_write = size_write;
  return obj;
}

objDelete * deleteObj(uint8_t size, char * name)
{
  objCreate * obj = malloc(sizeof(objDelete));
  obj->type = 6;
  obj->size = size;
  obj->name = malloc(size);
  obj->name = name;
  return obj;
}

Error * createError(uint8_t id)
{
  Error * err = malloc(sizeof(Error));
  err->type = 2;
  err->tError = id;
  return err;
}
