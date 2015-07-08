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

static void help() {
  printf("usage: wic command\n"
         "\n"
         "install    install to Visual Studio directory\n"
         "uninstall  put Visual Studio back the way it was\n");
  exit(0);
}

static void err(char *s) {
  auto error = GetLastError();
  char *msg;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (char *)&msg, 0, 0);
  printf("%s failed with error %d: %s", s, error, msg);
  exit(error);
}

static void copy(const char *oldFile, const char *newFile) {
  printf("copy %s -> %s\n", oldFile, newFile);
  if (!CopyFile(oldFile, newFile, 0))
    err("CopyFile");
}

static void del(const char *file) {
  printf("delete %s\n", file);
  if (!DeleteFile(file))
    err("DeleteFile");
}

static void move(const char *oldFile, const char *newFile) {
  printf("move %s -> %s\n", oldFile, newFile);
  if (!MoveFile(oldFile, newFile))
    err("MoveFile");
}

int main(int argc, char **argv) {
  char myFile[MAX_PATH];
  GetModuleFileName(0, myFile, MAX_PATH);
  auto separator = strrchr(myFile, '\\');
  auto myName = separator + 1;

  if (!strcmp(myName, "wic.exe")) {
    if (argc != 2)
      help();
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
    auto cl64 = bin + "\\x86_amd64\\cl.exe";
    auto realCl = bin + "\\real-cl.exe";
    auto realCl64 = bin + "\\x86_amd64\\real-cl.exe";

    if (!strcmp(command, "install")) {
      if (GetFileAttributes(realCl.c_str()) == INVALID_FILE_ATTRIBUTES &&
          GetLastError() == ERROR_FILE_NOT_FOUND) {
        move(cl.c_str(), realCl.c_str());
        move(cl64.c_str(), realCl64.c_str());
      }
      copy(myFile, cl.c_str());
      copy(myFile, cl64.c_str());
      return 0;
    }

    if (!strcmp(command, "uninstall")) {
      if (GetFileAttributes(realCl.c_str()) == INVALID_FILE_ATTRIBUTES &&
          GetLastError() == ERROR_FILE_NOT_FOUND) {
        puts("wic is not installed");
        return 1;
      }
      del(cl.c_str());
      move(realCl.c_str(), cl.c_str());
      del(cl64.c_str());
      move(realCl64.c_str(), cl64.c_str());
      return 0;
    }

    printf("%s: unknown command\n", command);
    return 1;
  }

  if (!strcmp(myName, "cl.exe")) {
    separator[1] = 0;
    auto program = string(myFile) + "real-cl.exe";

    string command = "cl.exe";
    for (auto i = argv + 1; i != argv + argc; ++i) {
      command += ' ';
      auto q = strchr(*i, ' ');
      if (q)
        command += '"';
      command += *i;
      if (q)
        command += '"';
    }

    STARTUPINFO si;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof pi);

    if (!CreateProcess(program.c_str(), (char *)command.c_str(), 0, 0, 1, 0, 0,
                       0, &si, &pi))
      err("CreateProcess");
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode))
      err("GetExitCodeProcess");
    return exitCode;
  }

  puts("unknown program name");
  return 1;
}
