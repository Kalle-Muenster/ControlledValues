@if "%ECHO_STATE%"=="" (@echo off) else (@echo %ECHO_STATE%)

set ConsolaProject=%~dp0..\..\Consola\ConsolaDot48
set Int24TypesProject=%~dp0..\..\Int24Types\dot48
set Float16TypeProject=%~dp0..\..\Float16Type

set ARCH=%~1
set CONF=%~2
set CLEAN=%~3

pushd %ConsolaProject%
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
call Build.cmd "%ARCH%" "%CONF%" Test %CLEAN%
popd

pushd "%Float16TypeProject%"
call Pack.bat dot48
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
popd

pushd "%Int24TypesProject%"
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
popd

pushd "%~dp0"
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
call Build.cmd "%ARCH%" "%CONF%" Test %CLEAN%
popd

set ARCH=
set CONF=
set CLEAN=
