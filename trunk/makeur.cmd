@echo off
echo.
echo Building libraries - "Win32 Unicode Release"...
echo.
msdev.com Repository.dsw /MAKE "AfxGadgets - Win32 Unicode Release" /REBUILD
echo.
msdev.com Repository.dsw /MAKE "atlmfc71 - Win32 Unicode Release" /REBUILD
echo.
msdev.com Repository.dsw /MAKE "CodeProject - Win32 Unicode Release" /REBUILD
echo.
msdev.com Repository.dsw /MAKE "Naughter - Win32 Unicode Release" /REBUILD
echo.
msdev.com Repository.dsw /MAKE "ZipArchive - Win32 Unicode Release" /REBUILD
echo.
