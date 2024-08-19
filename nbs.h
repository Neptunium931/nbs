#ifndef NBS_H
#define NBS_H
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
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
void freeCmdChildren(cmd *cmdInput);
void freeCmd(cmd *cmdInput);

const char *getFIleName(const char *path);

sourceFile *createSourceFile(const char *path,
                             const char *compiledPath,
                             const cmd *compileCmd);
void showSourceFile(const sourceFile sourceFileInput);
void freeSourceFileChildren(sourceFile *sourceFileInput);
void freeSourceFile(sourceFile *sourceFileInput);

bool fileExists(const char *path);
bool needsCompiling(const sourceFile sourceFile);

target *createTarget(const char *targetPath, const char *targetName);
void addSourceFile(target *targetInput, sourceFile *sourceFile);
void addLinkCmd(target *targetInput, cmd *linkCmd);
void showTarget(const target targetInput);
void freeTarget(target *targetInput);

void compileSources(sourceFile sourceFiles);
void compileTarget(target targetInput);
void linkTarget(target targetInput);

char **createArgsArray(const char *first, ...);
pid_t runCommandAsync(char **args);
void waitForPid(pid_t pid);
void runCommandSync(char **args);

void VLOG(FILE *strean, char *tag, char *fmt, va_list args);
void INFO(char *fmt, ...);
void WARN(char *fmt, ...);
void ERRO(char *fmt, ...);
void PANIC(char *fmt, ...);
#define CMD(...)                                                              \
  {                                                                           \
    runCommandSync(createArgsArray(__VA_ARGS__));                             \
  }

#define CMD_ASYNC(...)                                                        \
  {                                                                           \
    runCommandAsync(createArgsArray(__VA_ARGS__));                            \
  }
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
freeCmdChildren(cmd *cmdInput)
{
  free(cmdInput->command);
  free(cmdInput->args);
}

void
freeCmd(cmd *cmdInput)
{
  freeCmdChildren(cmdInput);
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
freeSourceFileChildren(sourceFile *sourceFileInput)
{
  free(sourceFileInput->path);
  free(sourceFileInput->compiledPath);
  freeCmd(sourceFileInput->compileCmd);
}

void
freeSourceFile(sourceFile *sourceFileInput)
{
  freeSourceFileChildren(sourceFileInput);
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
  target *targetCreate = (target *)calloc(sizeof(target), 1);
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
  for (int i = 0; i < targetInput.numSourceFiles; i++)
  {
    printf("sourceFile[%d]:\n", i);
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
    freeSourceFileChildren(&targetInput->sourceFiles[i]);
  }
  free(targetInput);
}

char **
createArgsArray(const char *first, ...)
{
  va_list args;
  va_start(args, first);

  int numArgs = 0;
  for (const char *next = first; next != NULL;
       next = va_arg(args, const char *))
  {
    numArgs++;
  }
  va_end(args);

  char **argsArray = (char **)malloc(sizeof(char *) * (numArgs + 1));
  if (argsArray == NULL)
  {
    PANIC("could not allocate memory for argsArray: %s", strerror(errno));
  }

  va_start(args, first);
  argsArray[0] = (char *)first;
  for (int i = 1; i <= numArgs; i++)
  {
    argsArray[i] = va_arg(args, char *);
  }
  va_end(args);

  argsArray[numArgs] = NULL;

  return argsArray;
}

pid_t
runCommandAsync(char **args)
{
  pid_t pid = fork();
  if (pid < 0)
  {
    PANIC("could not fork : %s", strerror(errno));
  }
  if (pid == 0)
  {
    if (execvp((char *)args[0], (char *const *)args) < 0)
    {
      PANIC("could not exec %s : %s", (char *)args[0], strerror(errno));
    }
    free(args);
  }
  return pid;
}

void
waitForPid(pid_t pid)
{
  int status;
  if (waitpid(pid, &status, 0) < 0)
  {
    PANIC("could not waitpid : %s", strerror(errno));
  }
}

void
runCommandSync(char **args)
{
  waitForPid(runCommandAsync(args));
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
