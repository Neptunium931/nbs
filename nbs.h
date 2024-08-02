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
#include <unistd.h>

#define PATH_SEP "/"
#define JOIN(sep, ...) array_join(sep, array_make(__VA_ARGS__, NULL))
#define CONCAT(...) JOIN("", __VA_ARGS__)
#define PATH(...) JOIN(PATH_SEP, __VA_ARGS__)

typedef struct
{
  const char **elems;
  size_t count;
} str_array;

str_array array_make(const char *first, ...);
const char *array_join(const char *sep, str_array array);

void mkdirPath(str_array path);
#define MKDIR(...)                                                            \
  {                                                                           \
    str_array path = array_make(__VA_ARGS__, NULL);                           \
    INFO("MKDIRS: %s", array_join(PATH_SEP, path));                           \
    mkdirPath(path);                                                          \
  }

void rmPath(const char *path);
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

int path_is_dir(const char *path);
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
  va_end(args);
  result.elems = malloc(sizeof(result.elems[0]) * result.count);
  if (result.elems == NULL)
  {
    PANIC("could not allocate memory: %s", strerror(errno));
  }
  result.count = 0;
  result.elems[result.count++] = first;

  va_start(args, first);
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    result.elems[result.count++] = next;
  }
  va_end(args);

  return result;
}

const char *
array_join(const char *sep, str_array array)
{
  if (array.count == 0)
  {
    return "";
  }

  const size_t sep_len = strlen(sep);
  size_t len = 0;
  for (size_t i = 0; i < array.count; ++i)
  {
    len += strlen(array.elems[i]);
  }

  const size_t result_len = (array.count - 1) * sep_len + len + 1;
  char *result = malloc(sizeof(char) * result_len);
  if (result == NULL)
  {
    PANIC("could not allocate memory: %s", strerror(errno));
  }

  len = 0;
  for (size_t i = 0; i < array.count; ++i)
  {
    if (i > 0)
    {
      memcpy(result + len, sep, sep_len);
      len += sep_len;
    }

    size_t elem_len = strlen(array.elems[i]);
    memcpy(result + len, array.elems[i], elem_len);
    len += elem_len;
  }
  result[len] = '\0';

  return result;
}

void
mkdirPath(str_array path)
{
  if (path.count == 0)
  {
    return;
  }
  size_t len = 0;
  for (size_t i = 0; i < path.count; ++i)
  {
    len += strlen(path.elems[i]);
  }

  size_t sepsCount = path.count - 1;
  const size_t sepLen = strlen(PATH_SEP);

  char *result = malloc(sizeof(char) * (len + sepsCount * sepLen + 1));
  if (result == NULL)
  {
    PANIC("could not allocate memory: %s", strerror(errno));
  }
  len = 0;
  for (size_t i = 0; i < path.count; ++i)
  {
    size_t n = strlen(path.elems[i]);
    memcpy(result + len, path.elems[i], n);
    len += n;

    if (sepsCount > 0)
    {
      memcpy(result + len, PATH_SEP, sepLen);
      len += sepLen;
      sepsCount--;
    }
    result[len] = '\0';
    if (mkdir(result, 0755) < 0)
    {
      if (errno == EEXIST)
      {
        errno = 0;
        WARN("directory %s already exists", result);
      }
      else
      {
        PANIC("can not create directory %s: %s", result, strerror(errno));
      }
    }
  }
}

int
path_is_dir(const char *path)
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
rmPath(const char *path)
{
  if (IS_DIR(path))
  {
    FOREACH_FILE_DIR(file, path, {
      if (strcmp(file, ".") != 0 && strcmp(file, "..") != 0)
      {
        rmPath(PATH(path, file));
      }
    });

    if (rmdir(path) < 0)
    {
      if (errno == ENOENT)
      {
        WARN("directory %s does not exist", path);
      }
      else
      {
        PANIC("can not remove directory %s: %s", path, strerror(errno));
      }
    }
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
