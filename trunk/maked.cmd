@echo off

echo.
echo Building libraries - "Win32 MBCS Debug"...
echo.

devenv.com Repository71.sln /rebuild "MBCS Debug" /project AfxGadgets /nologo
echo.
devenv.com Repository71.sln /rebuild "MBCS Debug" /project CodeProject /nologo
echo.
devenv.com Repository71.sln /rebuild "MBCS Debug" /project Naughter /nologo
echo.
devenv.com Repository71.sln /rebuild "MBCS Debug" /project ZipArchive /nologo
echo.
