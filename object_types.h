#ifndef __DATATYPES_H__
#define __DATATYPES_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>


typedef struct Message {
  uint8_t type;
} Message;

typedef struct Msg ack;

typedef struct Error {
  uint8_t type;
  uint8_t tError;
} Error;

typedef struct ObjCreate {
  uint8_t type;
  uint8_t size;
  char * name;
} objCreate;

typedef struct ObjSize {
  uint8_t type;
  uint8_t size;
  char * name;
} ObjSize;

typedef struct SizeAck {
  uint8_t type;
  uint64_t size;
  uint8_t block;
} sizeAck;


typedef struct ObjRead {
  uint8_t type;
  uint8_t size;
  char * name;
  uint64_t offset;
  uint64_t size_read;
} ObjRead;

typedef struct ReadAck {
  uint8_t type;
  uint64_t size;
  char * data;
} ReadAck;

typedef struct ObjWrite {
  uint8_t type;
  uint8_t size;
  char * name;
  uint64_t offset;
  uint64_t size_write;
  char * data;
} ObjWrite;

typedef struct WriteAck {
  uint8_t type;
  uint64_t size_write;
} WriteAck;

typedef struct ObjTrunc {
  uint8_t type;
  uint8_t size;
  char * name;
  uint64_t tsize;
} ObjTrunc;

typedef struct Servers {
  char * ip;
  int port;
} tServers;

typedef struct ObjCreate objDelete;
typedef struct ObjSize objSize;
typedef struct ObjRead objRead;
typedef struct ObjWrite objWrite;
typedef struct ObjTrunc objTrunc;

objCreate * createObj(uint8_t size, char * name);
objRead * createObjRead(uint8_t size, char * name, uint64_t offset, uint64_t size_read);
objDelete * deleteObj(uint8_t size, char * name);
objSize * createObjSize(uint8_t size, char * name);
sizeAck * createAckSize(uint64_t size, uint8_t block);
ReadAck * createAckRead(uint64_t size, char * data);
Error * createError(uint8_t id);
objWrite * createObjWrite(uint8_t size, char * name, uint64_t offset, uint64_t size_write, char * data);
WriteAck * createAckWrite(uint64_t size_write);
objTrunc * createObjTrunc(uint8_t size, char * name, uint64_t tsize);

#endif
