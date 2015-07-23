Wic intercepts calls to the Microsoft C++ compiler and records, modifies or redirects them according to the setting of environment variables. It was written for the purpose of redirecting calls to allow Clang to be used as a drop-in replacement, but can be used for other purposes.

A variety of build systems are used on Windows, most of which assume they have to call the Microsoft compiler, `cl.exe`. The only general way to intercept these calls is to interpose a program under the name of this executable. Thus, wic renames `cl.exe` to `real-cl.exe` and inserts itself as `cl.exe` where it can in turn call either `real-cl.exe` or some other program.

To use wic, bring up a command window that has Visual Studio in the path and type `wic install`. This requires administrator privilege in order to modify the contents of `Program Files (x86)`; an easy way to arrange this is to bring up a new command window by right-clicking on the icon and selecting 'Run as administrator'. Subsequent operation does not require this privilege.

To put things back the way they were, type `wic uninstall`.

The operation of wic is controlled by the following environment variables.

###wic-args

Prepend extra command line arguments before calling the compiler. For example, to set a necessary Clang flag for compatibility with the Microsoft preprocessor:

```
set wic-args=-Wno-invalid-token-paste
```

###wic-exe

By default, wic calls `real-cl.exe` for normal operation of Visual C++. To tell it to start calling another compiler such as Clang, set `wic-exe` to the full path of the replacement program e.g.

```
set wic-exe=C:\llvm\build\Release\bin\clang-cl.exe
```

To switch back, unset the environment variable:

```
set wic-exe=
```

###wic-save

Some build procedures create temporary files after the build starts and delete them before it ends; modifying or debugging the procedure may require some way to capture a copy of these files. If `wic-save` is set, wic will try to save a copy of every file named on the compiler command line to that directory e.g.

```
set wic-save=C:\temp-files
```
