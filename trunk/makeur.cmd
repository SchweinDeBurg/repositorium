@echo off

echo.
echo Building libraries - "Win32 Unicode Release"...
echo.

devenv.com Repository71.sln /rebuild "Unicode Release" /project AfxGadgets /nologo
echo.
devenv.com Repository71.sln /rebuild "Unicode Release" /project CodeProject /nologo
echo.
devenv.com Repository71.sln /rebuild "Unicode Release" /project Naughter /nologo
echo.
devenv.com Repository71.sln /rebuild "Unicode Release" /project ZipArchive /nologo
echo.
