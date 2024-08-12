#include <stdio.h>
#define NBS_IMPLEMENTATION
#include "nbs.h"

int
main(int argc, char *argv[])
{

  cmd *cmd = createCmd("echo", "hello", "world", NULL);
  showCmd(*cmd);
  addArgs(cmd, "h", "w", NULL);
  showCmd(*cmd);
  freeCmd(cmd);

  const char *path = "hello/world";
  printf("path: %s\n", path);
  printf("file: %s\n", getFIleName(path));

  cmd = createCmd("touch", "hello/world", NULL);
  sourceFile *sourceFile = createSourceFile(path, "hello/world", cmd);
  showSourceFile(*sourceFile);
  freeSourceFile(sourceFile);

  INFO("%s", "test INFO");
  WARN("%s", "test WARN");
  ERRO("%s", "test ERRO");
  PANIC("%s", "test PANIC");
  return 0;
}
