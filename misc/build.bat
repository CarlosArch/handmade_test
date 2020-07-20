@echo off

mkdir build
pushd build
cl ..\code\win32_handmade.c user32.lib
popd