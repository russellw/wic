#ifdef _MSC_VER
#pragma warning(disable : 4530)
#pragma warning(disable : 4577)
#endif

#include <stdio.h>
#include <windows.h>

#include <string>
using std::string;

// utility functions

static bool endsWith(const string &s, char *suffix) {
  auto n = strlen(suffix);
  if (n > s.size())
    return 0;
  return !s.compare(s.size() - n, n, suffix);
}

static string filename(const char *path) {
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  _splitpath(path, drive, dir, fname, ext);
  return string(fname) + ext;
}

static void help() {
  printf("usage: wic command\n"
         "\n"
         "install    install to Visual Studio directory\n"
         "uninstall  put Visual Studio back the way it was\n");
  exit(0);
}

static bool isSeparator(char c) {
  switch (c) {
  case '/':
  case ':':
  case '\\':
    return 1;
  }
  return 0;
}

// system functions

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

// main

int main(int argc, char **argv) {
  char me[MAX_PATH];
  GetModuleFileName(0, me, MAX_PATH);
  auto myName = strrchr(me, '\\') + 1;

  if (!stricmp(myName, "wic.exe")) {
    if (argc != 2)
      help();
    auto s = argv[1];
    if (*s == '-' || *s == '/') {
      while (*s == '-' || *s == '/')
        ++s;
      switch (*s) {
      case '?':
      case 'h':
        help();
      case 'V':
      case 'v':
        puts("wic version 2");
        return 0;
      default:
        printf("%s: unknown option\n", argv[1]);
        return 1;
      }
    }
    auto command = s;

    auto path = getenv("path");
    if (!path) {
      puts("path not found");
      return 1;
    }
    path = strdup(path);
    s = strtok(path, ";");
    while (s) {
      string dir = s;
      auto cl = dir + "\\cl.exe";
      if (GetFileAttributes(cl.c_str()) != INVALID_FILE_ATTRIBUTES)
        break;
      s = strtok(0, ";");
    }
    if (!s) {
      puts("Visual Studio not found in path");
      return 1;
    }
    string dir = s;
    auto cl = dir + "\\cl.exe";
    auto realCl = dir + "\\real-cl.exe";

    if (!strcmp(command, "install")) {
      if (GetFileAttributes(realCl.c_str()) == INVALID_FILE_ATTRIBUTES &&
          GetLastError() == ERROR_FILE_NOT_FOUND)
        move(cl.c_str(), realCl.c_str());
      copy(me, cl.c_str());
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
      return 0;
    }

    printf("%s: unknown command\n", command);
    return 1;
  }

  if (!stricmp(myName, "cl.exe")) {
    auto wicSave = getenv("wic-save");
    if (wicSave) {
      auto dir = string(wicSave) + '\\';
      for (auto i = argv + 1; i != argv + argc; ++i) {
        auto s = *i;
        switch (*s) {
        case '-':
        case '/':
          continue;
        case '@':
          ++s;
          break;
        }
        auto file = dir + filename(s);
        CopyFile(s, file.c_str(), 0);
      }
    }

    string dir(me, strrchr(me, '\\') + 1);
    auto exe = dir + "real-cl.exe";
    auto wicExe = getenv("wic-exe");
    if (wicExe)
      exe = wicExe;

    string args;
    auto wicArgs = getenv("wic-args");
    if (wicArgs)
      args = string(wicArgs) + ' ';
    auto s = GetCommandLine();
    s = strchr(s + 1, *s == '"' ? '"' : ' ');
    if (s)
      args += s + 1;

    auto command = string("\"") + exe + "\" " + args;

    STARTUPINFO si;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof pi);

    if (!CreateProcess(exe.c_str(), (char *)command.c_str(), 0, 0, 1, 0, 0, 0,
                       &si, &pi))
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
