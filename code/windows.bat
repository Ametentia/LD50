@echo off
setlocal

IF NOT EXIST "../build" (mkdir "../build")

pushd "../build" > NUL

SET COMPILER_FLAGS=-nologo -W4 -wd4201 -wd4244 -wd4505 -I "..\base"
SET LINKER_FLAGS=user32.lib kernel32.lib gdi32.lib pathcch.lib shell32.lib shcore.lib ole32.lib

REM Build renderer
REM
call ..\base\renderer\windows_wgl.bat debug

REM Debug build
REM
cl %COMPILER_FLAGS% -Od -Zi "..\code\windows_ludum.cpp" -Fe"ludum_debug.exe" -link %LINKER_FLAGS%

popd > NUL

endlocal
