@if "%ECHO_STATE%"=="" (@echo off) else (@echo %ECHO_STATE%)

set _name_=ControlledValues
set _call_=%CD%
cd %~dp0
set _here_=%CD%
cd..
set _root_=%CD%

:: Set Version
set ControlledVersionNumber=00000003
set ControlledVersionString=0.0.0.3
set DotNetVersionString=core5

:: Set Dependency locations
if "%ConsolaBinRoot%"=="" (
set ConsolaBinRoot=%_root_%\..\Consola\bin\%DotNetVersionString%
)
if "%Int24TypesBinRoot%"=="" (
set Int24TypesBinRoot=%_root_%\..\Int24Types\bin\%DotNetVersionString%
)
if "%Float16TypeBinRoot%"=="" (
set Float16TypeBinRoot=%_root_%\..\Float16Type\bin\%DotNetVersionString%
)
:: Set parameters and solution files
call %_root_%\Args "%~1" "%~2" "%~3" "%~4" ControlledValues.sln QuallenTest.sln

:: Do the build
cd %_here_%
call MsBuild %_target_% %_args_%
cd %_call_%

:: Cleanup Environment
call %_root_%\Args ParameterCleanUp

