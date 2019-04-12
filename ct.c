#include "config.h"
#include "params.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include "msg_manager.h"
#include "object_types.h"

#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "log.h"

int fdsocket = 0;
char * fpath = NULL;
char * path = NULL;

char * rand_str(size_t sz)
{
  char * dest = malloc(sz);
  char charset[] = "abcdefghijklmnopqrstuvwxyz"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "0123456789";

  time_t t;
  srand((unsigned) time(&t));

  for (size_t i = 0; i <= sz - 2; i++) {
    size_t index = (double) rand() / RAND_MAX * (strlen(charset) - 1);
    dest[i] = charset[index];
  }
  dest[sz - 1] = '\0';

  return dest;
}

void release_metadata(char * fpath, char * str)
{
  FILE * f = fopen(fpath, "w");
  // Write the metadata random_string to the file
  fprintf(f, "%s", str);
  fclose(f);
}

char * parse_file(char * filename)
{
  char * buffer = NULL;
  FILE * fp = fopen(filename, "r");
  int size = 0;

  if (fp == NULL)
  {
    perror("file open error");
    exit(-1);
  }

  if (fp)
  {
    /* Go to the end of the file. */
    if (fseek(fp, 0L, SEEK_END) == 0) {
        /* Get the size of the file. */
        long bufsize = ftell(fp);
        if (bufsize == -1) { /* Error */ }

        /* Allocate our buffer to that size. */
        buffer = malloc(sizeof(char) * (bufsize + 1));

        /* Go back to the start of the file. */
        if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

        /* Read the entire file into memory. */
        size_t newLen = fread(buffer, sizeof(char), bufsize, fp);
        if ( ferror( fp ) != 0 ) {
            fputs("Error reading file", stderr);
        } else {
            buffer[newLen++] = '\0'; /* Just to be safe. */
        }
    }
    fclose(fp);
  }

return buffer;


}

void create_object()
{
  objCreate * obj;
  Message * reply = NULL;
  char * str_rand;
  str_rand = rand_str(strlen(path + 1) + 2);
  obj = createObj(strlen(str_rand), str_rand);
  int n = sendCreateObject(fdsocket, obj);
  reply = receive_msg_server(fdsocket);
  if (reply != NULL)
  {
    if (reply->type == 2)
    {
      Error * err = (Error*) reply;
      fprintf(stderr, "[CLIENT] create_object Error\n");
    }
    else if (reply->type == 1)
    {
      release_metadata(fpath, str_rand);
      fprintf(stdout, "[CLIENT] Object %s created successfully\n", str_rand);
    }

  }
  free(str_rand);
  free(obj);
  free(reply);
}

void create_object_read()
{
  Message * reply;
  char * hash = parse_file(fpath);
  objRead * obj = createObjRead(strlen(hash), hash, 0, 1);
  int n = sendCreateObjectRead(fdsocket, obj);
  reply = receive_msg_server(fdsocket);
  if (reply != NULL)
  {
    if (reply->type == 2)
    {
      Error * err = (Error*) reply;
      fprintf(stderr, "[CLIENT] create_object_read Error %d\n", err->tError);
      free(err);
    }
    else if (reply->type == 8)
    {
      ReadAck * rack = (ReadAck*)reply;
      fprintf(stdout, "[CLIENT - READ ACK] Object %s readed successfully\n", hash);
      fprintf(stdout, "Data: %s\n", rack->data);
      free(rack->data);
      free(rack);
    }

  }

  if (obj != NULL)
  {
    free(obj->name);
    free(obj);
    obj = NULL;
  }


}

void create_object_write()
{
  Message * reply;
  char * hash = parse_file(fpath);
  char * data = malloc(2);
  data[0] = 'x';
  data[1] = '\0';
  ObjWrite * obj = createObjWrite(strlen(hash), hash, 0, 1, data);
  int n = sendCreateObjectWrite(fdsocket, obj);
  reply = receive_msg_server(fdsocket);

  if (reply != NULL)
  {
    if (reply->type == 2)
    {
      Error * err = (Error*) reply;
      fprintf(stderr, "[CLIENT] create_object_write Error %d\n", err->tError);
      free(err);
    }
    else if (reply->type == 10)
    {
      WriteAck * wack = (WriteAck*) reply;
      fprintf(stdout, "[CLIENT - WRITE_OBJECT] Object %s wrote successfully with %ld bytes\n", hash, wack->size_write);
      free(wack);
    }

  }
  free(data);
  if (obj != NULL)
  {
    free(obj->name);
    free(obj);
    obj = NULL;
  }

}

void create_remove_object()
{
  Message * reply = NULL;
  char * hash = parse_file(fpath);
  objDelete * obj = deleteObj(strlen(hash), hash);
  int n = sendRemoveObject(fdsocket, obj);
  reply = receive_msg_server(fdsocket);

  if (reply != NULL)
  {
    if (reply->type == 2)
    {
      Error * err = (Error*) reply;
      fprintf(stderr, "[CLIENT] create_remove_object Error %d\n", err->tError);
    }
    else if (reply->type == 1)
    {
      fprintf(stdout, "[CLIENT - REMOVE_OBJECT] Object %s removed successfully\n", hash);
    }

  }
  if (obj != NULL)
  {
    free(obj->name);
    free(obj);
    obj = NULL;
  }
  free(reply);

}

void create_size_object()
{
  char * hash = parse_file(fpath);
  objSize * obj = createObjSize(strlen(hash), hash);
  int n = sendCreateObjectSize(fdsocket, obj);

  Message * mack = receive_msg_server(fdsocket);

  if (mack != NULL)
  {
    if (mack->type == 2)
    {
      Error * err = (Error*) mack;
      fprintf(stderr, "[CLIENT] create_size_object Error %d\n", err->tError);
      free(err);
    }
    else if (mack->type == 12)
    {
      sizeAck * ack = (sizeAck*) mack;
      fprintf(stdout, "[CLIENT - SIZE_OBJECT]  Object %s, object_size: %ld, blocks: %d\n", hash, ack->size, ack->block);
      free(ack);
    }

  }

  if (obj != NULL)
  {
    free(obj->name);
    free(obj);
    obj = NULL;
  }

}

void create_object_trunc()
{
  char * hash = parse_file(fpath);
  objTrunc * obj = createObjTrunc(strlen(hash), hash, 15); // 12 - 15
  int n = sendObjectTrunc(fdsocket, obj);
  Message * reply = receive_msg_server(fdsocket);

  if (reply != NULL)
  {
    if (reply->type == 2)
    {
      Error * err = (Error*) reply;
      fprintf(stderr, "[CLIENT] create_object_trunc Error %d\n", err->tError);
      free(err);
    }
    else if (reply->type == 1)
    {
      fprintf(stdout, "[CLIENT - REMOVE_OBJECT] Object %s trunc'ed successfully\n", hash);
      free(reply);
    }

  }

  if (obj != NULL)
  {
    free(obj->name);
    free(obj);
    obj = NULL;
  }
}


int main (int argc, int ** argv)
{
  struct sockaddr_in server;

  fdsocket = socket(AF_INET , SOCK_STREAM , 0);
  if (fdsocket < 0)
      perror("Could not create socket");

  puts("Socket created");
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons(7777);

  if (connect(fdsocket , (struct sockaddr*)&server, sizeof(server)) < 0)
  {
    perror("error connect");
    close(fdsocket);
    return -1;
  }

  Message * r;
  sendConnect(fdsocket);
  r = receive_msg_server(fdsocket);
  if (r != NULL && r->type == 1)
    puts("<-------------- Main Connected ! ---------------------->");
  free(r);

  fpath = "/home/valy/Dropbox/4/DEISO/fuse-tutorial-2018-02-04/src/test";
  path = "/test";


  create_object();
  create_remove_object();
  create_object();
  create_object_write();
  create_object_read();
  create_size_object();
  create_object_trunc();


  sendDisconnect(fdsocket);
  r = receive_msg_server(fdsocket);
  if (r != NULL && r->type == 1)
    puts("<-------------- Main Disconnected ! ---------------------->");
  free(r);
close(fdsocket);
  return 0;
}
