set version=3.8.0

del llvm-%version%.src.tar
7z x llvm-%version%.src.tar.xz
if errorlevel 1 goto :eof
7z x llvm-%version%.src.tar
if errorlevel 1 goto :eof

del cfe-%version%.src.tar
7z x cfe-%version%.src.tar.xz
if errorlevel 1 goto :eof
7z x cfe-%version%.src.tar
if errorlevel 1 goto :eof

rd /q /s \llvm.bak
move \llvm \llvm.bak

move llvm-%version%.src \llvm
if errorlevel 1 goto :eof
move cfe-%version%.src \llvm\tools\clang
if errorlevel 1 goto :eof
cd \llvm
if errorlevel 1 goto :eof
md build
if errorlevel 1 goto :eof
cd build
if errorlevel 1 goto :eof

\CMake\bin\cmake.exe -DLLVM_ENABLE_TIMESTAMPS=OFF -DLLVM_USE_CRT_DEBUG=MTd -DLLVM_USE_CRT_RELEASE=MT -G "Visual Studio 14 Win64" ..
if errorlevel 1 goto :eof

if "%VCINSTALLDIR%"=="" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64
if errorlevel 1 goto :eof

msbuild ALL_BUILD.vcxproj
if errorlevel 1 goto :eof
msbuild /p:Configuration=Release ALL_BUILD.vcxproj
if errorlevel 1 goto :eof
