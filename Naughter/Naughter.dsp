# Microsoft Developer Studio Project File - Name="Naughter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Naughter - Win32 ICL Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Naughter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Naughter.mak" CFG="Naughter - Win32 ICL Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Naughter - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Naughter - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Naughter - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Naughter - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Naughter - Win32 ICL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Naughter - Win32 ICL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Naughter - Win32 ICL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Naughter - Win32 ICL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Naughter - Win32 Release"

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
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Naughter - Win32 Debug"

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
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Naughter - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Naughter___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "Naughter___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ReleaseW"
# PROP Intermediate_Dir ".\ReleaseW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "_AFXDLL" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Naughter - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Naughter___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "Naughter___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\DebugW"
# PROP Intermediate_Dir ".\DebugW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "_AFXDLL" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_NON_INTEL_COMPILER" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Naughter - Win32 ICL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Naughter___Win32_ICL_Release"
# PROP BASE Intermediate_Dir "Naughter___Win32_ICL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ICL_ReleaseA"
# PROP Intermediate_Dir ".\ICL_ReleaseA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Naughter - Win32 ICL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Naughter___Win32_ICL_Debug"
# PROP BASE Intermediate_Dir "Naughter___Win32_ICL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\ICL_DebugA"
# PROP Intermediate_Dir ".\ICL_DebugA"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "_MBCS" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Naughter - Win32 ICL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Naughter___Win32_ICL_Unicode_Release"
# PROP BASE Intermediate_Dir "Naughter___Win32_ICL_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\ICL_ReleaseW"
# PROP Intermediate_Dir ".\ICL_ReleaseW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp4 /MD /W4 /GR /GX /O1 /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Naughter - Win32 ICL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Naughter___Win32_ICL_Unicode_Debug"
# PROP BASE Intermediate_Dir "Naughter___Win32_ICL_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\ICL_DebugW"
# PROP Intermediate_Dir ".\ICL_DebugW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /Zp4 /MDd /W4 /GR /GX /Zi /Od /Ob1 /Gf /Gy /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_WINDOWS" /D "_AFXDLL" /D "WIN32" /D "_LIB" /D "_USE_INTEL_COMPILER" /FD /GZ /c
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

# Name "Naughter - Win32 Release"
# Name "Naughter - Win32 Debug"
# Name "Naughter - Win32 Unicode Release"
# Name "Naughter - Win32 Unicode Debug"
# Name "Naughter - Win32 ICL Release"
# Name "Naughter - Win32 ICL Debug"
# Name "Naughter - Win32 ICL Unicode Release"
# Name "Naughter - Win32 ICL Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\Base64Coder.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\md5.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\Pop3.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\Smtp.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\Base64Coder.h
# End Source File
# Begin Source File

SOURCE=".\Source\glob-md5.h"
# End Source File
# Begin Source File

SOURCE=.\Source\md5.h
# End Source File
# Begin Source File

SOURCE=.\Source\Pop3.h
# End Source File
# Begin Source File

SOURCE=.\Source\Smtp.h
# End Source File
# Begin Source File

SOURCE=.\Source\stdafx.h
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "htm;gif"
# Begin Source File

SOURCE=.\Help\POP3.GIF
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\POP3.HTM
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\SMTP.GIF
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\Smtp.htm
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
