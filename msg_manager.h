#ifndef __MSG_MANAGER_H__
#define __MSG_MANAGER_H__

#include "obj_func.h"


void * readBytes(int fd, int n);
uint8_t readByte(int fd);
Message * create_msg(uint8_t type);
void manage_msg(int fd, Message * msg);
Message * receive_msg_client(int socket);
Message * receive_msg_server(int socket);
int sendConnect(int fd);
int sendDisconnect(int fd);
Message * receive_connection_client(int socket);
int sendCreateObject(int fd, objCreate * obj);
int sendCreateObjectRead(int fd, objRead * obj);
int sendCreateObjectSize(int fd, objSize * obj);
int sendRemoveObject(int fd, objDelete * obj);
int sendCreateObjectWrite(int fd, objWrite * obj);
int sendObjectTrunc(int fd, ObjTrunc * obj);


#endif
