#ifndef __OBJ_FUNC_H__
#define __OJB_FUNC_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "object_types.h"
#include <inttypes.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define CONNECTION 3

#define EEEXIST (-1)   // El objeto ya exist
#define EENOOBJ (-2)   // El objeto no existe
#define EEFULL (-3)    // Almacenamiento lleno
#define EEOFFSET (-4)  // sici ́on de lectura/escritura dentro del objeto incorrecta
#define EEIO (-5)      // Error de entrada/salida
#define EEPARAM (-6)   // Par ́ametros incorrectos
#define EEBUSY (-7)    // Recursos no disponibles
#define EEINVAL (-8)   // Petici ́on/operaci ́on inv ́alida

uint64_t get_object_size(objSize * obj);
int open_object(objCreate * _obj);
char * read_object(objRead * obj);
int write_object(objWrite * obj);
int remove_object(objDelete * obj);
int truncate_object(ObjTrunc * obj);
int get_error();


#endif
