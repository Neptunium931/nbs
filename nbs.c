#define NBS_IMPLEMENTATION
#include "nbs.h"

int
main(int argc, char *argv[])
{
  RM("./build");

  INFO("%s", "test INFO");
  WARN("%s", "test WARN");
  ERRO("%s", "test ERRO");
  PANIC("%s", "test PANIC");
  return 0;
}
