#ifdef _MSC_VER
#pragma warning(disable: 4530)
#endif

#include <stdio.h>
#include <windows.h>

#include <regex>
using std::regex;
using std::regex_match;

int main(int argc, char **argv) {
  char buf[MAX_PATH];
  GetModuleFileName(NULL, buf, MAX_PATH);
  auto name = strrchr(buf, '\\') + 1;

  if (!strcmp(name, "wic.exe")) {
    if (argc >= 2) {
      auto command = argv[1];

      auto path = getenv("path");
      if (!path) {
        puts("path not found");
        return 1;
      }
      path = strdup(path);
      auto bin = strtok(path, ";");
      auto e = regex(".*Microsoft Visual Studio.*BIN");
      while (bin) {
        if (regex_match(bin, e))
          break;
        bin = strtok(0, ";");
      }
      if (!bin) {
        puts("Visual Studio not found in path");
        return 1;
      }

      if (!strcmp(command, "install")) {
        return 0;
      }

      printf("%s: unknown command\n", command);
      return 1;
    }
    printf("usage: wic command\n"
           "\n"
           "install    install to Visual Studio directory\n"
           "uninstall  put Visual Studio back the way it was\n");
    return 1;
  }

  if (!strcmp(name, "cl.exe")) {
  }

  puts("unknown program name");
  return 1;
}
