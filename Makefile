all: bbfs_client bbfs_server
test: t bbfs_server

t:
	gcc -g -ggdb -D_FILE_OFFSET_BITS=64 -pthread obj_func.c obj_func.h msg_manager.c msg_manager.h ct.c log.c log.h object_types.c object_types.h params.h -o ct.o `pkg-config fuse --cflags --libs`
bbfs_client:
	gcc -g -ggdb -D_FILE_OFFSET_BITS=64 -pthread bbfs_client.c obj_func.c obj_func.h msg_manager.c msg_manager.h log.c log.h object_types.c object_types.h params.h -o client.o `pkg-config fuse --cflags --libs`
bbfs_server:
	gcc -g -ggdb -D_FILE_OFFSET_BITS=64 -pthread log.h log.c bbfs_server.c obj_func.c obj_func.h msg_manager.c msg_manager.h object_types.c object_types.h -o server.o `pkg-config fuse --cflags --libs`
clean:
	rm *.o
