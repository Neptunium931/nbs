#ifndef NBS_H
#define NBS_H
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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
void showCmd(const cmd cmdInput);
void addArgs(cmd *cmdInput, const char *first, ...);
void freeCmd(cmd *cmdInput);

const char *getFIleName(const char *path);

sourceFile *createSourceFile(const char *path,
                             const char *compiledPath,
                             const cmd *compileCmd);
void showSourceFile(const sourceFile sourceFileInput);
void freeSourceFile(sourceFile *sourceFileInput);

bool fileExists(const char *path);
bool needsCompiling(const sourceFile sourceFile);

target *createTarget(const char *targetPath, const char *targetName);
void addSourceFile(target *targetInput, sourceFile *sourceFile);
void addLinkCmd(target *targetInput, cmd *linkCmd);
void showTarget(const target targetInput);
void freeTarget(target *targetInput);

void compileSources(sourceFile sourceFiles);
void compileTarget(target target);
void linkTarget(target target);

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
  cmd *cmdCreate = (cmd *)malloc(sizeof(cmd));

  cmdCreate->command = (char *)malloc(strlen(cmdName) + 1);
  strcpy(cmdCreate->command, cmdName);

  va_list args;
  va_start(args, first);
  cmdCreate->numArgs = 1;
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    printf("arg: %s\n", next);
    cmdCreate->numArgs++;
    printf("arg: %s\n", next);
  }
  va_end(args);

  cmdCreate->args = (char **)malloc(sizeof(char *) * cmdCreate->numArgs);
  if (cmdCreate->args == NULL)
  {
    PANIC("could not allocate memory : %s", strerror(errno));
  }

  cmdCreate->numArgs = 0;
  cmdCreate->args[cmdCreate->numArgs++] = first;

  va_start(args, first);
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    cmdCreate->args[cmdCreate->numArgs++] = next;
  }
  va_end(args);

  return cmdCreate;
}

void
showCmd(const cmd cmdInput)
{
  printf("cmd: %s\n", cmdInput.command);
  for (int i = 0; i < cmdInput.numArgs; i++)
  {
    printf("arg[%d]: %s\n", i, cmdInput.args[i]);
  }
}

void
addArgs(cmd *cmdInput, const char *first, ...)
{
  if (cmdInput == NULL || first == NULL)
  {
    return;
  }

  int numArgs = cmdInput->numArgs;

  va_list args;
  va_start(args, first);
  cmdInput->numArgs++;
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    cmdInput->numArgs++;
  }
  va_end(args);

  cmdInput->args =
    (char **)realloc(cmdInput->args, sizeof(char *) * cmdInput->numArgs);
  if (cmdInput->args == NULL)
  {
    PANIC("could not allocate memory : %s", strerror(errno));
  }

  cmdInput->args[numArgs++] = first;

  va_start(args, first);
  for (const char *next = va_arg(args, const char *); next != NULL;
       next = va_arg(args, const char *))
  {
    cmdInput->args[numArgs++] = next;
  }
  va_end(args);
}

void
freeCmd(cmd *cmdInput)
{
  free(cmdInput->command);
  free(cmdInput->args);
  free(cmdInput);
}

const char *
getFIleName(const char *path)
{
  const char *lastSlash = strrchr(path, '/');
  if (lastSlash == NULL)
  {
    return path;
  }
  return lastSlash + 1;
}

sourceFile *
createSourceFile(const char *path,
                 const char *compiledPath,
                 const cmd *compileCmd)
{
  sourceFile *sourceFileCreate = (sourceFile *)malloc(sizeof(sourceFile));
  sourceFileCreate->path = strdup(path);
  sourceFileCreate->compiledPath = strdup(compiledPath);
  sourceFileCreate->compileCmd = compileCmd;
  return sourceFileCreate;
}

void
showSourceFile(const sourceFile sourceFileInput)
{
  printf("sourceFile: %s\n", sourceFileInput.path);
  printf("compiledPath: %s\n", sourceFileInput.compiledPath);
  showCmd(*sourceFileInput.compileCmd);
}

void
freeSourceFile(sourceFile *sourceFileInput)
{
  free(sourceFileInput->path);
  free(sourceFileInput->compiledPath);
  free(sourceFileInput->compileCmd);
  free(sourceFileInput);
}

bool
fileExists(const char *path)
{
  if (access(path, F_OK) == 0)
  {
    return true;
  }
  return false;
}

bool
needsCompiling(const sourceFile sourceFile)
{
  if (!fileExists(sourceFile.compiledPath))
  {
    return true;
  }
  struct stat sourceFileStat;
  if (stat(sourceFile.path, &sourceFileStat) != 0)
  {
    PANIC("could not stat %s : %s", sourceFile.path, strerror(errno));
  }
  struct stat compiledFileStat;
  if (stat(sourceFile.compiledPath, &compiledFileStat) != 0)
  {
    PANIC("could not stat %s : %s", sourceFile.compiledPath, strerror(errno));
  }
  return sourceFileStat.st_mtime > compiledFileStat.st_mtime;
}

target *
createTarget(const char *targetPath, const char *targetName)
{
  target *targetCreate = (target *)malloc(sizeof(target));
  targetCreate->targetPath = strdup(targetPath);
  targetCreate->targetName = strdup(targetName);
  return targetCreate;
}

void
addSourceFile(target *targetInput, sourceFile *sourceFileInput)
{
  if (targetInput->sourceFiles == NULL)
  {
    targetInput->sourceFiles = (sourceFile *)malloc(sizeof(sourceFile));
  }
  else
  {
    targetInput->sourceFiles = (sourceFile *)realloc(
      targetInput->sourceFiles,
      sizeof(sourceFile) * (targetInput->numSourceFiles + 1));
  }
  targetInput->sourceFiles[targetInput->numSourceFiles++] = *sourceFileInput;
}

void
addLinkCmd(target *targetInput, cmd *linkCmd)
{
  targetInput->linkCmd = linkCmd;
}

void
showTarget(const target targetInput)
{
  printf("target: %s\n", targetInput.targetName);
  puts("sourceFiles:");
  for (int i = 0; i < targetInput.numSourceFiles; i++)
  {
    showSourceFile(targetInput.sourceFiles[i]);
  }
  puts("linkCmd:");
  showCmd(*targetInput.linkCmd);
}

void
freeTarget(target *targetInput)
{
  free(targetInput->targetPath);
  free(targetInput->targetName);
  free(targetInput->linkCmd);
  for (int i = 0; i < targetInput->numSourceFiles; i++)
  {
    freeSourceFile(&targetInput->sourceFiles[i]);
  }
  free(targetInput->sourceFiles);
  free(targetInput);
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
