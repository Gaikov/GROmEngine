# Microsoft Developer Studio Project File - Name="raster_d3d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=raster_d3d - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "raster_d3d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "raster_d3d.mak" CFG="raster_d3d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "raster_d3d - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "raster_d3d - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "raster_d3d"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "raster_d3d - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../build_temp/raster_d3d_Release"
# PROP Intermediate_Dir "../build_temp/raster_d3d_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RASTER_D3D_EXPORTS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RASTER_D3D_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d8.lib d3dx8.lib dxerr8.lib /nologo /dll /machine:I386 /out:"../bin/raster/raster_d3d.dll"

!ELSEIF  "$(CFG)" == "raster_d3d - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../build_temp/raster_d3d_Debug"
# PROP Intermediate_Dir "../build_temp/raster_d3d_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RASTER_D3D_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RASTER_D3D_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d8.lib d3dx8.lib dxerr8.lib /nologo /dll /debug /machine:I386 /out:"../bin/raster/raster_d3d_d.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "raster_d3d - Win32 Release"
# Name "raster_d3d - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;h;hpp;"
# Begin Source File

SOURCE=.\ConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\D3d_enum.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_enum.h
# End Source File
# Begin Source File

SOURCE=..\Common\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\Draw2d.cpp
# End Source File
# Begin Source File

SOURCE=.\draw2d.h
# End Source File
# Begin Source File

SOURCE=.\Draw3d.cpp
# End Source File
# Begin Source File

SOURCE=.\draw3d.h
# End Source File
# Begin Source File

SOURCE=.\DynamicObj.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicObj.h
# End Source File
# Begin Source File

SOURCE=.\local.h
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MdlManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MdlManager.h
# End Source File
# Begin Source File

SOURCE=.\RenderState.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderState.h
# End Source File
# Begin Source File

SOURCE=.\States.cpp
# End Source File
# Begin Source File

SOURCE=.\States.h
# End Source File
# Begin Source File

SOURCE=.\TexManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TexManager.h
# End Source File
# Begin Source File

SOURCE=.\VertexBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexBuffer.h
# End Source File
# Begin Source File

SOURCE=.\VertexBufferManager.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexBufferManager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\raster_d3d.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\Comm_def.cpp
# End Source File
# Begin Source File

SOURCE=..\common\comm_def.h
# End Source File
# Begin Source File

SOURCE=..\common\comm_raster.h
# End Source File
# Begin Source File

SOURCE=..\common\comm_types.h
# End Source File
# Begin Source File

SOURCE=..\common\mymath.h
# End Source File
# Begin Source File

SOURCE=..\common\parser.cpp
# End Source File
# Begin Source File

SOURCE=..\common\parser.h
# End Source File
# Begin Source File

SOURCE=..\common\ParserUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ParserUtils.h
# End Source File
# End Group
# End Target
# End Project
