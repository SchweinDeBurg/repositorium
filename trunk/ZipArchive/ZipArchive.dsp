# Microsoft Developer Studio Project File - Name="ZipArchive" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ZipArchive - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ZipArchive.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ZipArchive.mak" CFG="ZipArchive - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ZipArchive - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ZipArchive - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ZipArchive - Win32 Release"

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
# ADD BASE CPP /nologo /Zp4 /W4 /GR /GX /Od /Ob1 /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /Ob1 /Gf /Gy /I "..\zlib\Source" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ZipArchive - Win32 Debug"

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
# ADD BASE CPP /nologo /Zp4 /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /I "..\zlib\Source" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "_AFXDLL" /FD /GZ /c
# SUBTRACT CPP /YX
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

# Name "ZipArchive - Win32 Release"
# Name "ZipArchive - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipArchive.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipAutoBuffer.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipCentralDir.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipCompatibility.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipException.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipFile.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipFileHeader.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipMemFile.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipPathComponent.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipPlatform.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipPlatformComm.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipStorage.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ZipString.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipAbstractFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipArchive.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipAutoBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipBaseException.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipCentralDir.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipCollections.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipCompatibility.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipException.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipExport.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipFileHeader.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipFileMapping.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipMemFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipPathComponent.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipPlatform.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipStorage.h
# End Source File
# Begin Source File

SOURCE=.\Source\ZipString.h
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "chi;chm"
# Begin Source File

SOURCE=.\Help\ZipArchive.chi
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\ZipArchive.chm
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
