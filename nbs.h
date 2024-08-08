#ifndef NBS_H
#define NBS_H
#include <stdarg.h>
#include <stdio.h>

typedef struct
{
  char *cmd;
  char **args;
  int numArgs;
} cmd;

typedef struct
{
  char *path;
  char *compiledPath;
  cmd *compileCmd;
} sourceFile;

typedef struct
{
  sourceFile *sourceFiles;
  int numSourceFiles;
  char *targetPath;
  char *targetName;
  cmd *linkCmd;
} target;

const cmd *createCmd(const char cmdName, const char first, ...);
void showCmd(const cmd cmd);
const cmd *addArgs(const cmd cmd, const char args, ...);
void freeCmd(const cmd *cmd);

const char *getFIleName(const char *path);

const sourceFile *createSourceFile(const char *path,
                                   const char *compiledPath,
                                   const cmd *compileCmd);
void freeSourceFile(const sourceFile *sourceFile);

bool needsCompiling(const sourceFile sourceFile);

const target *createTarget(const char *targetPath, const char *targetName);
const target *addSourceFile(const target target, const sourceFile *sourceFile);
void showTarget(const target target);
void freeTarget(const target *target);

void compile(target target);
void link(target target);

void VLOG(FILE *strean, char *tag, char *fmt, va_list args);
void INFO(char *fmt, ...);
void WARN(char *fmt, ...);
void ERRO(char *fmt, ...);
void PANIC(char *fmt, ...);
#endif // NBS_H

#ifdef NBS_IMPLEMENTATION

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
