#include "obj_func.h"

int iderr = 0;

uint64_t get_object_size(objSize * obj)
{
  if (obj->size < 1 || obj->size != strlen(obj->name)){
    return EEPARAM;
  }

  struct stat statbuf;
  stat(obj->name, &statbuf);
  uint64_t size = (uint64_t)statbuf.st_size;
  return size;
}

int open_object(objCreate * _obj)
{
  if (_obj->size < 1 || _obj->size != strlen(_obj->name))
    return EEPARAM;
  //printf("obj %s, size: %d, strlen: %ld\n", _obj->name, _obj->size, strlen(_obj->name));

  int obj = open(_obj->name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP
                | S_IWGRP | S_IROTH);

  // File exists
  if (access(_obj->name, F_OK) != -1)
  {
    if (errno == ENOSPC)
    {
      close(obj);
      return EEFULL;
    }
    else if (errno == EEXIST)
    {
      close(obj);
      return EEEXIST;
    }

  }

  close(obj);
  return obj;
}

int remove_object(objDelete * obj)
{
  if (obj->size < 1 || obj->size != strlen(obj->name)){
    return EEPARAM;
  }

  if (unlink(obj->name) < 0){
    if (errno == ENOENT)
      return EENOOBJ;
    return errno;
  }
  return 0;
}

/* uint64_t for high offsets */
void * read_file(int fd, uint64_t n)
{
  void * buffer = malloc(n);
  uint64_t readed = 0;
  uint64_t off = 0;

  while ((readed = read(fd, buffer + off, n)) > 0 && n > 0)
  {
    n -= readed;
    off += readed;
  }

  return buffer;
}

char * read_object(objRead * obj)
{

  char * data = (char*)malloc(obj->size_read);

  if (obj->size < 1 || obj->size != strlen(obj->name) || obj->offset < 0 || obj->size_read < 1)
  {
    iderr = EEPARAM;
    return NULL;
  }



  int file = open(obj->name, O_RDONLY);

  if (errno == ENOENT)
  {
    iderr = EENOOBJ;
    close(file);
    return NULL;
  }

  data = read_file(file, obj->size_read);

  if (data == NULL)
  {
    if (errno == EINVAL)
    {
      iderr = EEOFFSET;
      close(file);
      return NULL;
    }
  }

  close(file);
  return data;
}

int write_object(objWrite * obj)
{
  if (obj->size < 1 || obj->size != strlen(obj->name) || obj->offset < 0 || obj->size_write < 1)
    return EEPARAM;

  errno = 0;
  int file = open(obj->name, O_WRONLY);
  if (errno == ENOENT)
  {
    close(file);
    return EENOOBJ;
  }

  if (lseek(file, obj->offset, SEEK_SET) < 0)
  {
    if (errno == EINVAL)
    {
      close(file);
      return EEOFFSET;
    }
  }

  uint64_t wrote = (uint64_t)write(file, obj->data, obj->size_write);
  close(file);

  if (wrote < obj->size_write)
  {
    if (errno == EIO)
      return EEIO;
    else if (errno == ENOSPC)
      return EEFULL;

  }
  return wrote;
}

int truncate_object(ObjTrunc * obj)
{
  if (obj->size < 1 || obj->size != strlen(obj->name) || obj->tsize < 0)
    return EEPARAM;

  int file = open(obj->name, O_WRONLY);

  if (errno == ENOENT)
  {
    close(file);
    return EENOOBJ;
  }
  else if (errno == EIO)
  {
    close(file);
    return EEIO;
  }

  if (ftruncate(file, obj->tsize) < 0)
  {
    close(file);
    return errno;
  }
  else{
    close(file);
    return 0;
  }
}

int get_error()
{
  return iderr;
}
