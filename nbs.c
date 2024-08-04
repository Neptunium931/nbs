#include <stdio.h>
#define NBS_IMPLEMENTATION
#include "nbs.h"

static const char buildDir[] = "./build";

int
main(int argc, char *argv[])
{
  static target target;
  addSrcFile(target, "main.c");
  printf("target: %s\n", target.targets[0].in);
  printf("target: %s\n", target.targets[0].out);
  CMD("echo", "test", "test2");
  MKDIR("./build", "test", "test2");
  RM("./build");

  INFO("%s", "test INFO");
  WARN("%s", "test WARN");
  ERRO("%s", "test ERRO");
  PANIC("%s", "test PANIC");
  return 0;
}
