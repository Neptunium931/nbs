#include <stdio.h>
#define NBS_IMPLEMENTATION
#include "nbs.h"

int
main(int argc, char *argv[])
{

  cmd *cmdTest = createCmd("echo", "hello", "world", NULL);
  showCmd(*cmdTest);
  addArgs(cmdTest, "h", "w", NULL);
  showCmd(*cmdTest);
  freeCmd(cmdTest);

  const char *path = "hello/world";
  printf("path: %s\n", path);
  printf("file: %s\n", getFIleName(path));

  cmdTest = createCmd("touch", "hello/world", NULL);
  sourceFile *sourceFile = createSourceFile("nbs.c", "nbs.h", cmdTest);
  showSourceFile(*sourceFile);

  if (needsCompiling(*sourceFile))
  {
    printf("needsCompiling: true\n");
  }
  else
  {
    printf("needsCompiling: false\n");
  }

  cmd *linkCmd = createCmd("gcc", "nbs.c", "-o", "nbs", NULL);
  target *target = createTarget("build", "nbs");
  addSourceFile(target, sourceFile);
  addLinkCmd(target, linkCmd);
  puts("");
  showTarget(*target);
  freeTarget(target);

  INFO("%s", "test INFO");
  WARN("%s", "test WARN");
  ERRO("%s", "test ERRO");
  PANIC("%s", "test PANIC");
  return 0;
}
