@echo off

echo.
echo Building libraries - "Win32 MBCS Release"...
echo.

devenv.com Repository71.sln /rebuild "MBCS Release" /project AfxGadgets /nologo
echo.
devenv.com Repository71.sln /rebuild "MBCS Release" /project CodeProject /nologo
echo.
devenv.com Repository71.sln /rebuild "MBCS Release" /project Naughter /nologo
echo.
devenv.com Repository71.sln /rebuild "MBCS Release" /project ZipArchive /nologo
echo.
