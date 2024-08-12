#ifndef NBS_H
#define NBS_H
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  char *command;
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

cmd *createCmd(const char *cmdName, const char *first, ...);
void showCmd(const cmd cmd);
void addArgs(cmd *cmd, const char *first, ...);
void freeCmd(cmd *cmd);

const char *getFIleName(const char *path);

sourceFile *createSourceFile(const char *path,
                             const char *compiledPath,
                             const cmd *compileCmd);
void freeSourceFile(const sourceFile *sourceFile);

bool needsCompiling(const sourceFile sourceFile);

target *createTarget(const char *targetPath, const char *targetName);
target *addSourceFile(const target target, const sourceFile *sourceFile);
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
#define NBS_IMPLEMENTATION
#ifdef NBS_IMPLEMENTATION

cmd *
createCmd(const char *cmdName, const char *first, ...)
{
  if (cmdName == NULL)
  {
    return NULL;
  }
  cmd *cmd = malloc(sizeof(cmd));

  cmd->command = (char *)malloc(strlen(cmdName) + 1);
  strcpy(cmd->command, cmdName);

  va_list args;
  va_start(args, first);
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    cmd->numArgs++;
  }
  va_end(args);

  cmd->args = (char **)malloc(sizeof(char *) * cmd->numArgs);
  if (cmd->args == NULL)
  {
    PANIC("could not allocate memory : %s", strerror(errno));
  }

  cmd->numArgs = 0;
  cmd->args[cmd->numArgs++] = first;

  va_start(args, first);
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    cmd->args[cmd->numArgs++] = next;
  }
  va_end(args);

  return cmd;
}

void
showCmd(const cmd cmd)
{
  printf("cmd: %s\n", cmd.command);
  for (int i = 0; i < cmd.numArgs; i++)
  {
    printf("arg[%d]: %s\n", i, cmd.args[i]);
  }
}

void
addArgs(cmd *cmd, const char *first, ...)
{
  if (cmd == NULL || first == NULL)
  {
    return;
  }

  int numArgs = cmd->numArgs;

  va_list args;
  va_start(args, first);
  cmd->numArgs++;
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    cmd->numArgs++;
  }
  va_end(args);

  cmd->args = (char **)realloc(cmd->args, sizeof(char *) * cmd->numArgs);
  if (cmd->args == NULL)
  {
    PANIC("could not allocate memory : %s", strerror(errno));
  }

  cmd->args[numArgs++] = first;

  va_start(args, first);
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    cmd->args[numArgs++] = next;
  }
  va_end(args);
}

void
freeCmd(cmd *cmd)
{
  free(cmd->command);
  free(cmd->args);
  free(cmd);
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
