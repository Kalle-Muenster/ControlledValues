@echo off

set _name_=ControlledValues
set _call_=%CD%
cd %~dp0
set _here_=%CD%
cd..
set _root_=%CD%

:: Set Version
set ControlledVersionNumber=00000003
set ControlledVersionString=0.0.0.3
set DotNetVersionString=dot48

:: Set Dependency locations
if "%ConsolaBinRoot%"=="" (
set ConsolaBinRoot=C:\WORKSPACE\PROJECTS\GITSPACE\Consola\bin\%DotNetVersionString%
)
if "%Int24TypesBinRoot%"=="" (
set Int24TypesBinRoot=C:\WORKSPACE\PROJECTS\GITSPACE\Int24Types\bin\%DotNetVersionString%
)

:: Set parameters and solution files
call %_root_%\Args "%~1" "%~2" "%~3" "%~4" ControlledValues.sln QuallenTest.sln

:: Do the build
cd %_here_%
call MsBuild %_target_% %_args_%
cd %_call_%

:: Cleanup Environment
call %_root_%\Args ParameterCleanUp

