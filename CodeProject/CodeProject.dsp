# Microsoft Developer Studio Project File - Name="CodeProject" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CodeProject - Win32 ICL Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CodeProject.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CodeProject.mak" CFG="CodeProject - Win32 ICL Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CodeProject - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CodeProject - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CodeProject - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CodeProject - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CodeProject - Win32 ICL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CodeProject - Win32 ICL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CodeProject - Win32 ICL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CodeProject - Win32 ICL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CodeProject - Win32 Release"

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

!ELSEIF  "$(CFG)" == "CodeProject - Win32 Debug"

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

!ELSEIF  "$(CFG)" == "CodeProject - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CodeProject___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "CodeProject___Win32_Unicode_Release"
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

!ELSEIF  "$(CFG)" == "CodeProject - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CodeProject___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "CodeProject___Win32_Unicode_Debug"
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

!ELSEIF  "$(CFG)" == "CodeProject - Win32 ICL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CodeProject___Win32_ICL_Release"
# PROP BASE Intermediate_Dir "CodeProject___Win32_ICL_Release"
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

!ELSEIF  "$(CFG)" == "CodeProject - Win32 ICL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CodeProject___Win32_ICL_Debug"
# PROP BASE Intermediate_Dir "CodeProject___Win32_ICL_Debug"
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

!ELSEIF  "$(CFG)" == "CodeProject - Win32 ICL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CodeProject___Win32_ICL_Unicode_Release"
# PROP BASE Intermediate_Dir "CodeProject___Win32_ICL_Unicode_Release"
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

!ELSEIF  "$(CFG)" == "CodeProject - Win32 ICL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CodeProject___Win32_ICL_Unicode_Debug"
# PROP BASE Intermediate_Dir "CodeProject___Win32_ICL_Unicode_Debug"
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

# Name "CodeProject - Win32 Release"
# Name "CodeProject - Win32 Debug"
# Name "CodeProject - Win32 Unicode Release"
# Name "CodeProject - Win32 Unicode Debug"
# Name "CodeProject - Win32 ICL Release"
# Name "CodeProject - Win32 ICL Debug"
# Name "CodeProject - Win32 ICL Unicode Release"
# Name "CodeProject - Win32 ICL Unicode Debug"
# Begin Group "stdafx"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\Source\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\stdafx.h
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\Source\ColourPicker.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ColourPicker.h
# End Source File
# Begin Source File

SOURCE=.\Source\ColourPopup.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ColourPopup.h
# End Source File
# Begin Source File

SOURCE=.\Source\HistoryCombo.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\HistoryCombo.h
# End Source File
# Begin Source File

SOURCE=.\Source\HyperLink.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\Source\ReportCtrl.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ReportCtrl.h
# End Source File
# End Group
# Begin Group "CPPTooltip"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\Source\PPDrawManager.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\PPDrawManager.h
# End Source File
# Begin Source File

SOURCE=.\Source\PPHtmlDrawer.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\PPHtmlDrawer.h
# End Source File
# Begin Source File

SOURCE=.\Source\PPTooltip.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\PPTooltip.h
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\Source\ETSLayout.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\ETSLayout.h
# End Source File
# Begin Source File

SOURCE=.\Source\FilteredFolderDlg.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\FilteredFolderDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\FolderDlg.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\FolderDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\IconDlg.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\IconDlg.h
# End Source File
# End Group
# Begin Group "Graphics"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\Source\CeXDib.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\CeXDib.h
# End Source File
# End Group
# Begin Group "Path"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\Source\FileSpec.cpp
# ADD CPP /Yu"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Source\FileSpec.h
# End Source File
# End Group
# Begin Group "XML"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=.\Source\PugXML.h
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "chm"
# Begin Source File

SOURCE=.\Help\CFileSpec.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\CFolderDialog.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\CIconDialog.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\ColourPicker.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\CPPTooltip.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\CReportCtrl.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\ETSLayout.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\HistoryComboBox.chm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Help\PugXML.chm
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
