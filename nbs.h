#ifndef NBS_H
#define NBS_H

#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef BUILD_DIR
#define BUILD_DIR "./build"
#endif // !BUILD_DIR
#ifndef CC
#define CC "cc"
#endif // !CC

#define PATH_SEP "/"
#define JOIN(sep, ...) array_join(sep, array_make(__VA_ARGS__, NULL))
#define CONCAT(...) JOIN("", __VA_ARGS__)
#define PATH(...) JOIN(PATH_SEP, __VA_ARGS__)

typedef struct
{
  char *elems;
  size_t count;
} str_array;

str_array array_make(const char *first, ...);
const char *array_join(const char sep, str_array array);

void rmPath(char *path);
#define RM(path)                                                              \
  {                                                                           \
    INFO("rm: %s", path);                                                     \
    rmPath(path);                                                             \
  }

#define FOREACH_FILE_DIR(file, path, body)                                    \
  {                                                                           \
    DIR *dir = opendir(path);                                                 \
    if (dir == NULL)                                                          \
    {                                                                         \
      PANIC("can not open directory %s: %s", path, strerror(errno));          \
    }                                                                         \
    errno = 0;                                                                \
    struct dirent *dp = NULL;                                                 \
    while ((dp = readdir(dir)))                                               \
    {                                                                         \
      const char *file = dp->d_name;                                          \
      body;                                                                   \
    }                                                                         \
    if (errno > 0)                                                            \
    {                                                                         \
      PANIC("can not read directory %s: %s", path, strerror(errno));          \
    }                                                                         \
    closedir(dir);                                                            \
  }

int path_is_dir(char *path);
#define IS_DIR(path) path_is_dir(path)

void VLOG(FILE *strean, char *tag, char *fmt, va_list args);
void INFO(char *fmt, ...);
void WARN(char *fmt, ...);
void ERRO(char *fmt, ...);
void PANIC(char *fmt, ...);
#endif // NBS_H

#ifdef NBS_IMPLEMENTATION
str_array
array_make(const char *first, ...)
{
  str_array result = { 0 };
  if (first == NULL)
  {
    return result;
  }
  result.count += 1;
  va_list args;
  va_start(args, first);
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    result.count++;
  }
}

const char *
array_join(const char sep, str_array array)
{
}

int
path_is_dir(char *path)
{
  struct stat statPath = { 0 };
  if (stat(path, &statPath) < 0)
  {
    if (errno == ENOENT)
    {
      errno = 0;
      return 0;
    }
    PANIC("can not get the stat of %s: %s", path, strerror(errno));
  }

  return S_ISDIR(statPath.st_mode);
}

void
rmPath(char *path)
{
  if (IS_DIR(path))
  {
    FOREACH_FILE_DIR(file, path, {
      if (strcmp(file, ".") != 0 && strcmp(file, "..") != 0)
      {
        path_rm(PATH(path, file));
      }
    });
  }
  else
  {
    if (unlink(path) < 0)
    {
      if (errno == ENOENT)
      {
        errno = 0;
        WARN("file %s does not exist", path);
      }
      else
      {
        PANIC("can not remove file %s: %s", path, strerror(errno));
      }
    }
  }
}

void
VLOG(FILE *stream, char *tag, char *fmt, va_list args)
{
  fprintf(stream, "[%s] ", tag);
  vfprintf(stream, fmt, args);
  fprintf(stream, "\n");
}

void
INFO(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  VLOG(stderr, "INFO", fmt, args);
  va_end(args);
}

void
WARN(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  VLOG(stderr, "WARN", fmt, args);
  va_end(args);
}

void
ERRO(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  VLOG(stderr, "ERRO", fmt, args);
  va_end(args);
}

void
PANIC(char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  VLOG(stderr, "PANIC", fmt, args);
  va_end(args);
  exit(-1);
}
#endif // NBS_IMPLEMENTATION
