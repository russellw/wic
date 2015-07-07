#ifdef _MSC_VER
#pragma warning(disable : 4530)
#endif

#include <stdio.h>
#include <windows.h>

#include <regex>
using std::regex;
using std::regex_match;

#include <string>
using std::string;

static void err(char *s) {
  // Retrieve the system error message for the last-error code
  auto error = GetLastError();
  char *msg;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (char *)&msg, 0, 0);

  // Display the error message and exit the process
  printf("%s failed with error %d: %s\n", s, error, msg);
  exit(error);
}

int main(int argc, char **argv) {
  char buf[MAX_PATH];
  GetModuleFileName(NULL, buf, MAX_PATH);
  auto name = strrchr(buf, '\\') + 1;

  if (!strcmp(name, "wic.exe")) {
    if (argc != 2) {
      printf("usage: wic command\n"
             "\n"
             "install    install to Visual Studio directory\n"
             "uninstall  put Visual Studio back the way it was\n");
      return 1;
    }
    auto command = argv[1];

    auto path = getenv("path");
    if (!path) {
      puts("path not found");
      return 1;
    }
    path = strdup(path);
    auto s = strtok(path, ";");
    auto e = regex(".*Microsoft Visual Studio.*BIN");
    while (s) {
      if (regex_match(s, e))
        break;
      s = strtok(0, ";");
    }
    if (!s) {
      puts("Visual Studio not found in path");
      return 1;
    }
    string bin = s;
    auto cl = bin + "\\cl.exe";
    auto real_cl = bin + "\\real-cl.exe";

    if (!strcmp(command, "install")) {
      if (GetFileAttributes(real_cl.c_str()) == INVALID_FILE_ATTRIBUTES &&
          GetLastError() == ERROR_FILE_NOT_FOUND)
        if (!MoveFile(cl.c_str(), real_cl.c_str()))
          err("MoveFile");
      return 0;
    }

    if (!strcmp(command, "uninstall")) {
      if (GetFileAttributes(real_cl.c_str()) == INVALID_FILE_ATTRIBUTES &&
          GetLastError() == ERROR_FILE_NOT_FOUND) {
        puts("wic is not installed");
        return 1;
      }
      if (!DeleteFile(cl.c_str()))
        err("DeleteFile");
      if (!MoveFile(real_cl.c_str(), cl.c_str()))
        err("MoveFile");
      return 0;
    }

    printf("%s: unknown command\n", command);
    return 1;
  }

  if (!strcmp(name, "cl.exe")) {
  }

  puts("unknown program name");
  return 1;
}
