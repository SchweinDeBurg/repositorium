# Microsoft Developer Studio Project File - Name="zlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zlib - Win32 ICL Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 ICL Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 ICL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 ICL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 ICL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 ICL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ReleaseA"
# PROP Intermediate_Dir ".\ReleaseA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\DebugA"
# PROP Intermediate_Dir ".\DebugA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "_MBCS" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zlib___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "zlib___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ReleaseW"
# PROP Intermediate_Dir ".\ReleaseW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zlib___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "zlib___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\DebugW"
# PROP Intermediate_Dir ".\DebugW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 ICL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zlib___Win32_ICL_Release"
# PROP BASE Intermediate_Dir "zlib___Win32_ICL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ICL_ReleaseA"
# PROP Intermediate_Dir ".\ICL_ReleaseA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 ICL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zlib___Win32_ICL_Debug"
# PROP BASE Intermediate_Dir "zlib___Win32_ICL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\ICL_DebugA"
# PROP Intermediate_Dir ".\ICL_DebugA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "_MBCS" /D "_WINDOWS" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "_MBCS" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 ICL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zlib___Win32_ICL_Unicode_Release"
# PROP BASE Intermediate_Dir "zlib___Win32_ICL_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ICL_ReleaseW"
# PROP Intermediate_Dir ".\ICL_ReleaseW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 ICL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zlib___Win32_ICL_Unicode_Debug"
# PROP BASE Intermediate_Dir "zlib___Win32_ICL_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\ICL_DebugW"
# PROP Intermediate_Dir ".\ICL_DebugW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "zlib - Win32 Release"
# Name "zlib - Win32 Debug"
# Name "zlib - Win32 Unicode Release"
# Name "zlib - Win32 Unicode Debug"
# Name "zlib - Win32 ICL Release"
# Name "zlib - Win32 ICL Debug"
# Name "zlib - Win32 ICL Unicode Release"
# Name "zlib - Win32 ICL Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\adler32.c
# End Source File
# Begin Source File

SOURCE=.\Source\compress.c
# End Source File
# Begin Source File

SOURCE=.\Source\crc32.c
# End Source File
# Begin Source File

SOURCE=.\Source\deflate.c
# End Source File
# Begin Source File

SOURCE=.\Source\gzio.c
# End Source File
# Begin Source File

SOURCE=.\Source\infback.c
# End Source File
# Begin Source File

SOURCE=.\Source\inffast.c
# End Source File
# Begin Source File

SOURCE=.\Source\inflate.c
# End Source File
# Begin Source File

SOURCE=.\Source\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\Source\trees.c
# End Source File
# Begin Source File

SOURCE=.\Source\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\Source\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\crc32.h
# End Source File
# Begin Source File

SOURCE=.\Source\deflate.h
# End Source File
# Begin Source File

SOURCE=.\Source\inffast.h
# End Source File
# Begin Source File

SOURCE=.\Source\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\Source\inflate.h
# End Source File
# Begin Source File

SOURCE=.\Source\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\Source\trees.h
# End Source File
# Begin Source File

SOURCE=.\Source\zconf.h
# End Source File
# Begin Source File

SOURCE=.\Source\zlib.h
# End Source File
# Begin Source File

SOURCE=.\Source\zutil.h
# End Source File
# End Group
# End Target
# End Project