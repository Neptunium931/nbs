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

  INFO("%s", "test INFO");
  WARN("%s", "test WARN");
  ERRO("%s", "test ERRO");
  PANIC("%s", "test PANIC");
  return 0;
}
