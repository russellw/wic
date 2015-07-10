# wic
Windows Interface for Clang

Clang on Windows provides `clang-cl.exe` which aims to be compatible with Microsoft C++. However, programs use a variety of build systems on Windows, most of which assume they have to call the Microsoft compiler, `cl.exe`. The only general way to compile existing programs with Clang is to catch the call.

Thus, wic renames `cl.exe` to `real-cl.exe` and inserts itself as `cl.exe` where it can in turn call either `real-cl.exe` or `clang-cl.exe` depending on the setting of environment variables.

To use wic, bring up a command window that has Visual Studio in the path and type `wic install`. This requires administrator privilege in order to modify the contents of `Program Files (x86)`; an easy way to arrange this is to bring up a new command window by right-clicking on the icon and selecting 'Run as administrator'. Subsequent operation does not require this privilege.

To put things back the way they were, type `wic uninstall`.

By default, wic calls `real-cl.exe` for normal operation of Visual Studio. To tell it to start calling Clang, set the `wic` environment variable with the location of the Clang binary folder e.g.

```
set wic=C:\llvm\buld\Release\bin
```

To switch back to Microsoft C++, unset the environment variable:

```
set wic=
```

In some cases it is necessary to select Clang only for 32-bit or only for 64-bit builds. To do this, use the environment variable `wic32` or `wic64`.

wic calls Clang with `-Wno-invalid-token-paste` which is necessary for compatibility with the Microsoft preprocessor. In addition, further arguments to Clang may be provided by setting the `wic-args` environment variable.
