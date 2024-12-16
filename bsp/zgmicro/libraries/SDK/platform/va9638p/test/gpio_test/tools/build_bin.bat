@echo off

:: Run after build
:: build_bin.bat $KARM\ARMCC\bin @L %L
:: build_bin.bat <fromelf bin locate dir> <target name> <Object output path> <target filename>


:: Command Help
:: % File name with extension (PROJECT1.UVPROJ) 
:: # File name with extension and complete path specification (C:\MYPROJECT\PROJECT1.UVPROJ) 
:: @ File name without extension or path specification (PROJECT1) 
:: $ Path name of a file. Path names get extended with a backslash. For example, $P could generate C:\MYPROJECT\. 
:: ! File name with extension and relative path specification to the current folder (.\SRC\TEST.C) 
:: ~ 1 Line number of the current cursor position 
:: ^ 1 Column number of the current cursor position 

:: $D Device name as selected from the Device Database. 
:: E   Editor file name currently in focus. 
:: F   Depending on the context, this File Code returns: 
::     The file selected in the window Project. 
::     The currently active editor file. 
::     The file that is currently translated by a build process. 
 
:: H   Application HEX file name (PROJECT1.H86). 
:: $J  Absolute compiler system include folder. Compiler base folders are listed in the field Project - Manage - Project Items - Folder/Extensions - ARMCC Folder. The include path depends on the compiler selected in Options for Target - Code Generation - ARM Compiler. 
:: K   Absolute root folder of the development toolchain, regardless of the Key Code used. 
:: L   Linker output file. Typically the executable file used for debugging (PROJECT1). 
:: M   CPU mask revision number. 
:: P   Current project file name. 
:: X   Vision executable program file (..\UV4\UV4.EXE). Works for For Key Code %, # @. 
:: $X  XTAL clock frequency in MHz as specified in Options for Target - Target - XTAL. 
:: ^X  XTAL clock frequency in kHz as specified in Options for Target - Target - XTAL. 


set CurDate=%date:~0,4%%date:~5,2%

:: Keil Project Paramters
set TargetName=%2
set TargetFileName=%4
set ObjPath=%3
set ProgramFromelf=%1\fromelf
set pluginPath="..\coreshare\tools\plugin.exe"
set targetPath="../../release/"

:: Output Env
set ReleasePath=%ObjPath%..\%TargetName%
set ReleaseBinPath=%ReleasePath%\binary
set ReleaseInfoPath=%ReleasePath%\debuginfo
set ReleaseCallGraph=%ReleasePath%\callgraph

::echo "%TargetName%"
::echo "%TargetFileName%"
::echo "%ObjPath%"

::echo "%ReleasePath%"
::echo "%ReleaseBinPath%"
::echo "%ReleaseInfoPath%"
::echo "%ReleaseCallGraph%"

if exist "%ReleasePath%" (del /F /S /Q "%ReleasePath%" & rd /S /Q "%ReleasePath%")

if not exist "%ReleasePath%" mkdir "%ReleasePath%"
if not exist "%ReleaseBinPath%" mkdir "%ReleaseBinPath%"
if not exist "%ReleaseInfoPath%" mkdir "%ReleaseInfoPath%"
if not exist "%ReleaseCallGraph%" mkdir "%ReleaseCallGraph%"

echo "Release to %ReleasePath%"

if exist "%ObjPath%%TargetName%.axf"   xcopy /Q /Y "%ObjPath%%TargetName%.axf" "%ReleaseInfoPath%"\
if exist "%ObjPath%%TargetName%.map"   xcopy /Q /Y "%ObjPath%%TargetName%.map" "%ReleaseInfoPath%"\
if exist "%ObjPath%%TargetName%.htm"   xcopy /Q /Y "%ObjPath%%TargetName%.htm" "%ReleaseCallGraph%"\
if exist "%ObjPath%%TargetName%.axf"   %ProgramFromelf% --bin --output %ReleaseBinPath%\%TargetName%.bin "%ObjPath%%TargetName%.axf"
if exist "%ObjPath%%TargetName%.axf"   %ProgramFromelf% --text -c -s --output "%ReleaseInfoPath%"\%TargetName%.txt "%ObjPath%%TargetName%.axf"

::%pluginPath% stack %ReleaseCallGraph%\%TargetName%.htm 128

%pluginPath% ver %ReleaseBinPath%\%TargetName%.bin %ReleaseInfoPath%\%TargetName%.map 0 "9648"
%pluginPath% copy %ReleasePath%  %targetPath%