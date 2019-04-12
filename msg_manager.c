#include "msg_manager.h"
#include "log.h"

/* -------------- AUX FUNCS --------------------------- */

void * readBytes(int fd, int n){
  char * buffer = (char *)malloc(n);
  int readBytes = 0;
  int byteOffset = 0;
  int nread = 0;

  while ((readBytes = read(fd, buffer + byteOffset, n)) > 0
        && n > 0){
    n -= readBytes;
    byteOffset += readBytes;
    nread += readBytes;
  }
  if (readBytes < 0){
    perror("Error reading from socket.\n");
  }
  if (n != 0){
    perror("Some bytes are waiting to be read.\n");
  }
  //printf("Readed: %d\n", nread);
  return buffer;
}

int writeBytes(int fd, void * data, int n)
{
  int written = 0;
  int writtenBytes = 0;

  while ((written = write(fd, data + writtenBytes, n)) > 0 && n > 0)
  {
    n -= written;
    writtenBytes += written;
  }

  if (writtenBytes < 0)
    perror("Server disconnected\n");

  if (n != 0)
    perror("There are bytes pending\n");

return writtenBytes;
}

uint8_t readByte(int fd){
  uint8_t * buffer = (uint8_t *)readBytes(fd, 1);
  uint8_t data = *buffer;
  free(buffer);
  return data;
}

/* --------------             ---------------------------*/

/* -------------- SENDERS -------------------------------*/

int sendConnect(int fd)
{
  Message * msg = create_msg(3);
  writeBytes(fd, msg, sizeof(Message));
  free(msg);
  return 0;
}

int sendAck(int fd)
{
  Message * msg = create_msg(1);
  writeBytes(fd, msg, sizeof(Message));
  return 0;
}

int sendDisconnect(int fd)
{
  Message * msg = create_msg(4);
  writeBytes(fd, msg, sizeof(Message));
  free(msg);
  return 0;
}

int sendError(Error * err, int fd)
{
  int n = 0;
  n += writeBytes(fd, (char*)&err->type, 1);
  n += writeBytes(fd, (char*)&err->tError, 1);
  return 0 ;
}

/*Server reply with the ack size struct*/
int sendSize(sizeAck * ack, int fd)
{
  int n = 0;
  n += writeBytes(fd, (char*)&ack->type, 1);
  n += writeBytes(fd, (char*)&ack->size, 8);
  n += writeBytes(fd, (char*)&ack->block, 1);
  return 0;
}

int sendReadAck(ReadAck * rack, int fd)
{
  int n = 0;
  n += writeBytes(fd, (char*)&rack->type, 1);
  n += writeBytes(fd, (char*)&rack->size, 8);
  n += writeBytes(fd, rack->data, rack->size);

  return n;
}

int sendWriteAck(WriteAck * wack, int fd)
{
  int n = 0;
  n += writeBytes(fd, (char*)&wack->type, 1);
  n += writeBytes(fd, (char*)&wack->size_write, 8);

  return n;
}

int sendCreateObject(int fd, objCreate * obj)
{
  int n = 0;
  n += writeBytes(fd, (char*)&obj->type, 1);
  n += writeBytes(fd, (char*)&obj->size, 1);
  n += writeBytes(fd, obj->name, obj->size);

  return n;
}

int sendCreateObjectRead(int fd, objRead * obj)
{
  int n = 0;
  n += writeBytes(fd, (char*)&obj->type, 1);
  n += writeBytes(fd, (char*)&obj->size, 1);
  n += writeBytes(fd, obj->name, obj->size);
  n += writeBytes(fd, (char*)&obj->offset, 8);
  n += writeBytes(fd, (char*)&obj->size_read, 8);

  return n;
}

int sendCreateObjectWrite(int fd, objWrite * obj)
{
  int n = 0;
  n += writeBytes(fd, (char*)&obj->type, 1);
  n += writeBytes(fd, (char*)&obj->size, 1);
  n += writeBytes(fd, obj->name, obj->size);
  n += writeBytes(fd, (char*)&obj->offset, 8);
  n += writeBytes(fd, (char*)&obj->size_write, 8);
  n += writeBytes(fd, obj->data, obj->size_write);

  return n;
}

int sendCreateObjectSize(int fd, objSize * obj)
{
  int n = 0;
  n += writeBytes(fd, (char*)&obj->type, 1);
  n += writeBytes(fd, (char*)&obj->size, 1);
  n += writeBytes(fd, obj->name, obj->size);

  return n;
}

int sendObjectTrunc(int fd, ObjTrunc * obj)
{
  int n = 0;
  n += writeBytes(fd, (char*)&obj->type, 1);
  n += writeBytes(fd, (char*)&obj->size, 1);
  n += writeBytes(fd, obj->name, obj->size);
  n += writeBytes(fd, (char*)&obj->tsize, 8);

  return n;
}

int sendRemoveObject(int fd, objDelete * obj)
{
  int n = 0;
  n += writeBytes(fd, (char*)&obj->type, 1);
  n += writeBytes(fd, (char*)&obj->size, 1);
  n += writeBytes(fd, obj->name, obj->size);

  return n;
}


/* --------------          -------------------------------*/


/* -------------- GETTERS  -------------------------------*/
Message * getCreateObject(int fd)
{
  uint8_t size = readByte(fd);
  char * name = malloc(size);
  name = readBytes(fd, size);
  objCreate * obj = createObj(size, name);
  return (Message*) obj;
}

Message * getTruncObject(int fd)
{
  uint8_t size = readByte(fd);
  char * name = malloc(size);
  name = readBytes(fd, size);
  uint64_t tsize = *(uint64_t *)readBytes(fd, 8);
  objTrunc * obj = createObjTrunc(size, name, tsize);
  return (Message*) obj;
}

Message * getDeleteObject(int fd)
{
  uint8_t size = readByte(fd);
  char * name = malloc(size);
  name = readBytes(fd, size);
  objDelete * obj = deleteObj(size, name);
  return (Message*) obj;
}

Message * getReadObject(int fd)
{
  uint8_t size = readByte(fd);
  char * name = malloc(size);
  name = readBytes(fd, size);
  uint64_t offset = *(uint64_t *)readBytes(fd, 8);
  uint64_t size_read = *(uint64_t *)readBytes(fd, 8);
  objRead * obj = createObjRead(size, name, offset, size_read);
  return (Message*) obj;
}

Message * getWriteObject(int fd)
{
  uint8_t size = readByte(fd);
  char * name = malloc(size);
  name = readBytes(fd, size);
  uint64_t offset = *(uint64_t *)readBytes(fd, 8);
  uint64_t size_write = *(uint64_t *)readBytes(fd, 8);
  char * data = malloc(size_write);
  data = readBytes(fd, size_write);
  objWrite * obj = createObjWrite(size, name, offset, size_write, data);
  return (Message*) obj;
}

Message * getCreateSizeObject(int fd)
{
  uint8_t size = readByte(fd);
  char * name = malloc(size);
  name = readBytes(fd, size);
  objSize * obj = createObjSize(size, name);
  return (Message*) obj;
}

Message * getSizeAck(int fd)
{
  char * buf = readBytes(fd, 8);
  uint64_t size = *(uint64_t *)buf;
  uint8_t blocks = readByte(fd);
  sizeAck * obj = createAckSize(size, blocks);
  free(buf);
  return (Message*) obj;
}

Message * getReadAck(int fd)
{
  char * buf = readBytes(fd, 8);
  uint64_t size = *(uint64_t *)buf;
  char * data = readBytes(fd, size);
  ReadAck * rack = createAckRead(size, data);
  free(buf);
  return (Message*)rack;
}

Message * getWriteAck(int fd)
{
  char * buf = readBytes(fd, 8);
  uint64_t size_write = *(uint64_t *)buf;
  WriteAck * wack = createAckWrite(size_write);
  free(buf);
  return (Message*)wack;
}

Message * getError(int fd)
{
  uint8_t id = readByte(fd);
  Error * err = createError(id);
  return (Message*) err;
}


Message * create_msg(uint8_t type){
  Message * msg = malloc(sizeof(Message));
  msg->type = type;
  return msg;
}

/* --------------          -------------------------------*/

void manage_msg(int fd, Message * msg){
  long retval = 0;
  uint64_t size;

  switch(msg->type){
      case 5:
        if ((retval = open_object((objCreate*)msg)) < 0)
        {
          fprintf(stderr, "open_object Error id: %ld\n", retval);
          Error * err = createError(retval);
          sendError(err, fd);
          free(err);
        }
        else
          sendAck(fd);
      //puts("[SERVER] Creating Object \n");
      break;

      case 6:
        if ((retval = remove_object((objDelete*)msg)) < 0)
        {
          retval *= -1; // change the sign
          fprintf(stderr, "remove_object Error id: %ld\n", retval);
          Error * err = createError(retval);
          sendError(err, fd);
          free(err);
        }
        else
          sendAck(fd);
      //puts("[SERVER] Removing Object \n");
      break;

      case 7:
        ;
        char * data = NULL;
        if ((data = read_object((objRead*)msg)) == NULL)
        {
          retval = get_error() * (-1); // change the sign
          fprintf(stderr, "[SERVER] Error %ld Reading object\n", retval);
          Error * err = createError(retval);
          sendError(err, fd);
          free(err);
        }
        else
        {
          //printf("data: %s\n", data);
          //puts("[SERVER] Creating Read Object\n");
          ReadAck * rack = createAckRead(strlen(data)+1, data);
          sendReadAck(rack, fd);
          free(rack);
        }
      break;

      case 9:
        if ((retval = write_object((objWrite*)msg)) < 0)
        {
          retval *= -1; // change the sign
          fprintf(stderr, "write_object Error id: %ld\n", retval);
          Error * err = createError(retval);
          sendError(err, fd);
          free(err);
        }
        else
        {
          //printf("wrote: %ld\n", retval);
          WriteAck * wack = createAckWrite(retval);
          sendWriteAck(wack, fd);
          free(wack);
        }
      break;

      case 11:
        if ((retval = get_object_size((objSize*)msg)) < 0)
        {
          fprintf(stderr, "get_object_size Error id: %ld\n", retval);
          Error * err = createError(retval);
          sendError(err, fd);
          free(err);
        } else size = retval;
        //puts("[SERVER] Creating Size object\n");
        uint8_t block = size / 512;
        sizeAck * ack = createAckSize(size, block);
        sendSize(ack, fd);
        free(ack);
      break;

      case 13:
      if ((retval = truncate_object((objTrunc*)msg)) < 0)
      {
        fprintf(stderr, "trunc_object Error id: %ld\n", retval);
        Error * err = createError(retval);
        sendError(err, fd);
        free(err);
      } else sendAck(fd);
      //puts("[SERVER] Creating Trunc object\n");
      break;

      default:
        fprintf(stderr, "Couldn't manage the message type %d\n", msg->type);

  }
}

Message * receive_connection_client(int socket)
{
  uint8_t type = readByte(socket);
  Message * received = create_msg(type);
  if (received->type == CONNECTION)
  {
    puts("<----------[SERVER] Connect received, sending ACK.. -------->\n");
    sendAck(socket);
    return received;
  }

  return NULL;
}

/* Receive message from the client */
Message * receive_msg_client(int socket){
  uint8_t type = readByte(socket);
  Message * received = create_msg(type);

  switch (received->type) {
    case 4:
      puts("[SERVER] Disconnect received, sending ACK..\n");
      sendAck(socket);
      return received;
    case 5:
      free(received);
      return getCreateObject(socket);
    case 6: // Delete Object
      free(received);
      return getDeleteObject(socket);
    case 7: // Read Object
      free(received);
      return getReadObject(socket);
    case 9: // Write Object
      free(received);
      return getWriteObject(socket);
    case 11:  // Size Object
      free(received);
      return getCreateSizeObject(socket);
    case 13: // Trunc Object
      free(received);
      return getTruncObject(socket);
    default:
      free(received);
      fprintf(stderr, "[SERVER] - Unknown message type %d\n", received->type);
      return NULL;
  }
return NULL;
}


/* Receive message from the server */
Message * receive_msg_server(int socket){
  uint8_t type = readByte(socket);
  Message * received = create_msg(type);

  switch (received->type) {
    case 1: // ACK
      //puts("[CLIENT] Received ACK..\n");
      return received;
    break;

    case 2: // ERROR
      free(received);
      received = getError(socket);
      return received;
    break;

    case 8: // Read ack
      free(received);
      received = getReadAck(socket);
      return received;
    break;

    case 10: // Write ack
      free(received);
      received = getWriteAck(socket);
      return received;

    case 12:
      //puts("[CLIENT] Received SIZE_ACK..\n");
      free(received);
      received = getSizeAck(socket);
      return received;
    break;
    default:
      fprintf(stderr, "From server - Unknown message type %d\n", received->type);
      return NULL;
    break;
  }
return NULL;
}
