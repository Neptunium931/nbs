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
  sourceFile *sourceFile1 = createSourceFile("nbs.c", "nbs.h", cmdTest);
  showSourceFile(*sourceFile1);

  if (needsCompiling(*sourceFile1))
  {
    printf("needsCompiling: true\n");
  }
  else
  {
    printf("needsCompiling: false\n");
  }

  cmd *linkCmd = createCmd("gcc", "nbs.c", "-o", "nbs", NULL);
  cmd *compileCmd =
    createCmd("gcc", "hello/world.c", "-o", "hello/world", NULL);
  sourceFile *sourceFile2 = createSourceFile("nbs.c", "nbs.h", compileCmd);
  target *target = createTarget("build", "nbs");
  addSourceFile(target, sourceFile1);
  addSourceFile(target, sourceFile2);
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
