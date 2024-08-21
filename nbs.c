// Copyright (c) 2024, Tymothé BILLEREY <tymothe_billerey@fastmail.fr>
// See end of file for extended copyright information.
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

  puts("");
  puts("");
  CMD("touch", "hello/world");
  CMD_ASYNC("touch", "hello/world2");

  INFO("%s", "test INFO");
  WARN("%s", "test WARN");
  ERRO("%s", "test ERRO");
  PANIC("%s", "test PANIC");
  return 0;
}
// This file is part of nbs.
//
// BSD 3-Clause License
//
// Copyright (c) 2024, Tymothé BILLEREY <tymothe_billerey@fastmail.fr>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
