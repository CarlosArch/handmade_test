@echo off

IF "%~1"=="final" ( GOTO FINAL )
IF "%~1"=="debug" ( GOTO DEBUG )
                    GOTO BADOP

SET libs=

:FINAL
    mkdir build
    pushd build
    cl ..\code\win32_handmade.c user32.lib gdi32.lib
    popd
    GOTO END

:DEBUG
    mkdir debug
    pushd debug
    cl /Zi ..\code\win32_handmade.c user32.lib gdi32.lib
    popd
    GOTO END

:BADOP
    echo Option "%~1" not one of (final, debug)
    GOTO END

:END
