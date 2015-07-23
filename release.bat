set version=3

call build.bat
if errorlevel 1 goto :eof

md wic-%version%

copy *.exe wic-%version%
copy *.md wic-%version%
copy LICENSE wic-%version%

del *.zip
7z a wic-%version%.zip wic-%version% -tzip

rd /q /s wic-%version%
