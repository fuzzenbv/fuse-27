
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#include <pthread.h>
#include "msg_manager.h"


#define BACKLOG 3
#define CONNECTION 3


char * dir = 0;



void * thread_connection(void * socket_client)
{
  int socket = *(int*)socket_client;
  Message * msg = NULL;

  if ((msg = receive_connection_client(socket)) == NULL)
  {
    fprintf(stderr, "First message must be - CONNECT(3)\n");
    pthread_exit(NULL);
    close(socket);
  }

  msg = receive_msg_client(socket);
    while(1)
    {
      if (!msg)
        break;
      if (msg != NULL && msg->type == 4)
        break;
      if (msg != NULL)
        manage_msg(socket, msg);

      msg = receive_msg_client(socket);
    }

  free(msg);
  close(socket);
  pthread_exit(NULL);
}

int main(int argc , char *argv[])
{

  int socketfd, socket_client;
  int port, opt;
  int option = 1;
  struct sockaddr_in server, client;

  optind = 0;

  while (argv[argc])
    argc++;

  if (argc < 2)
  {
    puts("Set option -p or -d");
    return -1;
  }

  while ((opt = getopt(argc, argv, "p:d:")) != -1)
  {
    switch(opt)
    {
      case 'p':
        port = atoi(argv[optind - 1]);
        break;
      case 'd':
        dir = argv[optind - 1];
        break;
      default:
        puts("Invalid option");
    }
  }

  fprintf(stdout, "port: %d, dir: %s\n", port, dir);

  if (chdir(dir) < 0)
  {
    perror("chdir error");
  }

  // Creates the socket
  socketfd = socket(AF_INET , SOCK_STREAM , 0);

  if (setsockopt(socketfd, SOL_TCP, TCP_NODELAY, (char*)&option, sizeof(option)) < 0)
  {
    perror("setsockopt error");
    close(socketfd);
  }

  if (socketfd < 0)
    puts("Could not create the socket");

  puts("[SERVER] Socket created");

  // Socket struct sockaddr_in
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  // Bind the socket
  if (bind(socketfd, (struct sockaddr*)&server, sizeof(server)) < 0)
  {
    perror("bind error ");
    return -1;
  }

  puts("Socket binded");

  listen(socketfd, BACKLOG);

  puts("[SERVER] Waiting for connections ...");

  int sizesock = sizeof(struct sockaddr_in);

  while (socket_client = accept(socketfd, (struct sockaddr*)&client,
                          (socklen_t*)&sizesock))
  {
      puts("[SERVER] New connection !");

      pthread_t  tHandler;
      int * nsock = malloc(1);
      *nsock = socket_client;

      if (pthread_create(&tHandler, NULL, thread_connection, (void*)nsock) < 0)
      {
        perror("Thread creation error");
        return -1;
      }

      if (pthread_join(tHandler, NULL) < 0)
      {
        perror("Thread join error");
        return -1;
      }

      puts ("[SERVER] Thread associated to connection exited!");
  }

  if (socket_client < 0)
  {
    perror("Socket accept error");
    return -1;
  }

  return 0;

}
