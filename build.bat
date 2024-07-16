@echo off

set RootDir=%cd%
set NSIGHT_GPU_DEBUGGER=1

set MSVCFlags= -O2 /std:c++17 -diagnostics:column -Zi -nologo /EHsc -W4 -wd4018 -wd4706 -wd4702 -wd4026 -wd4305 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4244 -FC  -I%RootDir%\include -MD
set LinkerFlags= -nologo /link -incremental:no -opt:ref /LIBPATH:%RootDir%\lib  user32.lib gdi32.lib winmm.lib opengl32.lib kernel32.lib shell32.lib 

set buildDirectory=%RootDir%\build

if not exist "%buildDirectory%\" (
    mkdir "%buildDirectory%"
)

pushd %buildDirectory%

if exist *.pdb (
    del *.pdb
)

if not exist precompiled.obj (
    cl /c %RootDir%\code\precompiled.cpp -O2 -nologo -I%RootDir%\include -MD
)

cl %RootDir%\code\win32_main.cpp precompiled.obj %MSVCFlags% %LinkerFlags%

popd
