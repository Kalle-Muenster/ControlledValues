@echo off
if "%1"=="" (
	echo parameter 1 must be build target
	echo parameter 2 can override toolversion
	echo parameter 3 can be location where to build...
	goto END
) 
if "%SOURCE_BASEDIR%"=="" set SOURCE_BASEDIR=%CD%
set _TARGET_=%1
:ALL_CONFIGURATION
cd %SOURCE_BASEDIR%
call build %_TARGET_% x86 Debug "%~2" "%~3"
cd %SOURCE_BASEDIR%
call build %_TARGET_% x86 Release "%~2" "%~3"
cd %SOURCE_BASEDIR%
call build %_TARGET_% x64 Debug "%~2" "%~3"
cd %SOURCE_BASEDIR%
call build %_TARGET_% x64 Release "%~2" "%~3"
cd %SOURCE_BASEDIR%
:END
set SOURCE_BASEDIR=
set _TARGET_=

