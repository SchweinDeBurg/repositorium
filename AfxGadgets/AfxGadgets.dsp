# Microsoft Developer Studio Project File - Name="AfxGadgets" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AfxGadgets - Win32 ICL Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AfxGadgets.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AfxGadgets.mak" CFG="AfxGadgets - Win32 ICL Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AfxGadgets - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AfxGadgets - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AfxGadgets - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AfxGadgets - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AfxGadgets - Win32 ICL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AfxGadgets - Win32 ICL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AfxGadgets - Win32 ICL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AfxGadgets - Win32 ICL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AfxGadgets - Win32 Release"

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
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AfxGadgets - Win32 Debug"

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
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "AfxGadgets - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AfxGadgets___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "AfxGadgets___Win32_Unicode_Release"
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

!ELSEIF  "$(CFG)" == "AfxGadgets - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AfxGadgets___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "AfxGadgets___Win32_Unicode_Debug"
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

!ELSEIF  "$(CFG)" == "AfxGadgets - Win32 ICL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AfxGadgets___Win32_ICL_Release"
# PROP BASE Intermediate_Dir "AfxGadgets___Win32_ICL_Release"
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

!ELSEIF  "$(CFG)" == "AfxGadgets - Win32 ICL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AfxGadgets___Win32_ICL_Debug"
# PROP BASE Intermediate_Dir "AfxGadgets___Win32_ICL_Debug"
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

!ELSEIF  "$(CFG)" == "AfxGadgets - Win32 ICL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AfxGadgets___Win32_ICL_Unicode_Release"
# PROP BASE Intermediate_Dir "AfxGadgets___Win32_ICL_Unicode_Release"
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

!ELSEIF  "$(CFG)" == "AfxGadgets - Win32 ICL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AfxGadgets___Win32_ICL_Unicode_Debug"
# PROP BASE Intermediate_Dir "AfxGadgets___Win32_ICL_Unicode_Debug"
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

# Name "AfxGadgets - Win32 Release"
# Name "AfxGadgets - Win32 Debug"
# Name "AfxGadgets - Win32 Unicode Release"
# Name "AfxGadgets - Win32 Unicode Debug"
# Name "AfxGadgets - Win32 ICL Release"
# Name "AfxGadgets - Win32 ICL Debug"
# Name "AfxGadgets - Win32 ICL Unicode Release"
# Name "AfxGadgets - Win32 ICL Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\DialogXML.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\HotKeysXML.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\MemMapFile.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\MemoryDC.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\MenuXML.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ProcessPrivileges.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\SeException.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\SortingListCtrl.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\Win32Error.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\WinCrypto.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\DialogXML.h
# End Source File
# Begin Source File

SOURCE=.\Source\HotKeysXML.h
# End Source File
# Begin Source File

SOURCE=.\Source\MemMapFile.h
# End Source File
# Begin Source File

SOURCE=.\Source\MemoryDC.h
# End Source File
# Begin Source File

SOURCE=.\Source\MenuXML.h
# End Source File
# Begin Source File

SOURCE=.\Source\ProcessPrivileges.h
# End Source File
# Begin Source File

SOURCE=.\Source\ProcessPrivileges.inl
# End Source File
# Begin Source File

SOURCE=.\Source\PugXMLplus.h
# End Source File
# Begin Source File

SOURCE=.\Source\SeException.h
# End Source File
# Begin Source File

SOURCE=.\Source\SortingListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Source\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Source\StringConv.h
# End Source File
# Begin Source File

SOURCE=.\Source\Win32Error.h
# End Source File
# Begin Source File

SOURCE=.\Source\WinCrypto.h
# End Source File
# Begin Source File

SOURCE=.\Source\XMLproxy.h
# End Source File
# End Group
# End Target
# End Project
