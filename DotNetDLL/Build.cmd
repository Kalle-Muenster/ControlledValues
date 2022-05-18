@echo off

:: Set Version
set ControlledVersionNumber=00000003
set ControlledVersionString=0.0.0.3
set DotNetVersionString=dot48

:: Set Dependency locations
if "ConsolaBinRoot"=="" (
set ConsolaBinRoot=C:\WORKSPACE\PROJECTS\GITSPACE\Consola\bin\%DotNetVersionString%
)
if "Int24TypesRoot"=="" (
set Int24TypesRoot=C:\WORKSPACE\PROJECTS\GITSPACE\Int24Types\bin\%DotNetVersionString%
)

set _call_=%CD%
cd %~dp0
set _root_=%CD%
cd..

set _build_=%CD%\bin\%DotNetVersionString%
set _clean_=

:CHECK_ARCH
if "%~1"=="x86" set _arch_=x86
if "%~2"=="x86" set _arch_=x86
if "%~3"=="x86" set _arch_=x86
if "%~4"=="x86" set _arch_=x86
if "%_arch_%"=="" set _arch_=x64

:CHECK_CONF
if "%~1"=="Debug" set _conf_=Debug
if "%~2"=="Debug" set _conf_=Debug
if "%~3"=="Debug" set _conf_=Debug
if "%~4"=="Debug" set _conf_=Debug
if "%_conf_%"=="" set _conf_=Release

:CHECK_TEST
if "%~1"=="Test" set _target_=QuallenTest.sln
if "%~2"=="Test" set _target_=QuallenTest.sln
if "%~3"=="Test" set _target_=QuallenTest.sln
if "%~4"=="Test" set _target_=QuallenTest.sln
if "%_target_%"=="" set _target_=ControlledValues.sln

:CHECK_CLEAN
if "%~1"=="Clean" set _clean_= /t:Clean
if "%~2"=="Clean" set _clean_= /t:Clean
if "%~3"=="Clean" set _clean_= /t:Clean
if "%~4"=="Clean" set _clean_= /t:Clean


if not "%_call_%"=="%_root_%" (
  if not "%_call_%"=="%CD%" (
    set _build_=%_call_%
  )
)


if "%_clean_%"=="" set ControlledValuesRoot=%_build_%
set _build_=%_build_%\%_arch_%\%_conf_%
set _args_=/p:Configuration=%_conf_%;Platform=%_arch_%;OutDir=%_build_%%_clean_%

cd %_root_%
call MsBuild %_target_% %_args_%
cd %_call_%

set _build_=
set _target_=
set _root_=
set _call_=
set _clean_=
set _arch_=
set _conf_=
set _args_=
set DotNetVersionString=
