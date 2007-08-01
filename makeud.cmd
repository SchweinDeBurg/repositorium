@echo off

echo.
echo Building libraries - "Win32 Unicode Debug"...
echo.

devenv.com Repository71.sln /rebuild "Unicode Debug" /project AfxGadgets /nologo
echo.
devenv.com Repository71.sln /rebuild "Unicode Debug" /project CodeProject /nologo
echo.
devenv.com Repository71.sln /rebuild "Unicode Debug" /project Naughter /nologo
echo.
devenv.com Repository71.sln /rebuild "Unicode Debug" /project ZipArchive /nologo
echo.
