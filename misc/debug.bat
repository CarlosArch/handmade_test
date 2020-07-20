@echo off

mkdir debug
pushd debug
cl /Zi ..\code\win32_handmade.c user32.lib
popd