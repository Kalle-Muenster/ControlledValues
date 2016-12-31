@echo off
md bin
cd bin
cd..
if _%1_==_ALL_ goto %1
if _%1_==_dll_ goto dotnetdll
if _%1_==_cpptests_ goto %1
if _%1_==_clean_ goto %1
echo.
echo usage:
echo build ^<Target^> [x86^|x64] [Debug^|Release]
echo.
echo Target: can be "dll", "cpptest" or "ALL".
echo.
echo run at best from within a MSBuild console 
echo (needs msbuild.exe and cl.exe in PATH)   
echo.
goto end

:ALL
set _all=Yes

:dotnetdll
::call checkCompiler  <- should set _compilator variable.. later...

set _compilator=MsBuild
md bin
pushd %CD%\bin
md DotNetDLL
cd DotNetDLL
if "%2"=="x64" (
    set _march=x64
) else (
    set _march=x86
)
md %_march%
cd %_march%
set _march=Platform=%_march%
if "%3"=="" (
    set _config=Release
) else (
    set _config=%3
)
md %_config%
cd %_config%
set _buildHere=%CD%
set _config=Configuration=%_config%
popd
pushd %CD%\DotNetDLL\ControlledValues\ControlledValues
set _source=%CD%\ControlledValues.vcxproj 
cd %_buildHere%
set _buildo_=/m /p
set _buildo_=%_buildo_%:OutDir=%_buildHere%
set _buildo_=%_buildo_%;%_config%
set _buildo_=%_buildo_%;%_march%
set _buildo_=%_buildo_% %_source%
set _buildo_=%_compilator% %_buildo_%
echo build command: %_buildo_%>build.log
echo.>>build.log
echo.
echo now building cli dll ...
echo.  
%_buildo_%>>build.log
echo ...
echo ..
echo ...Done!   -^>  see build.log
echo.
type build.log
popd
if _%1_==_ALL_ (
    goto cpptests
)
goto end

:cpptests
::call checkCompiler  <- should set _compilator variable.. later...

set _compilator=MsBuild
md bin
pushd %CD%\bin
if "%2"=="x64" (
    set _march=x64
) else (
    set _march=Win32
)
if "%3"=="Release" (
    set _config=Release
) else (
    set _config=Debug
)
set _config=Configuration=%_config%
set _march=Platform=%_march%
set _buildHere=%CD%
popd
pushd %CD%\tst
set _source=%CD%\Tests.sln
cd %_buildHere%
set _buildo_=/m /p
set _buildo_=%_buildo_%:OutDir=%_buildHere%
set _buildo_=%_buildo_%;%_config%
set _buildo_=%_buildo_%;%_march%
set _buildo_=%_buildo_% %_source%
set _buildo_=%_compilator% %_buildo_%
echo buildlog: cpptests>build_%1.log
echo build command: %_buildo_%>>build_%1.log
echo.>>build_%1.log
echo.
echo now building native cpp tests ...
echo.  
%_buildo_%>>build_%1.log
echo ...
echo ..
type build_%1.log
echo ...
echo ..
echo ...Done! -^>  type: "tests" to start testrun...
echo.
goto end

:clean
popd
rd bin /s /q
pushd %CD%\tst
rd Release /s /q
rd Debug /s /q
rd x64 /s /q
cd..
cd DotNetDLL
cd ControlledValues
cd ControlledValues
rd Release /s /q
rd Debug /s /q
rd x64 /s /q
cd..
cd..
cd Tests
cd Tests
rd bin /s /q
rd obj /s /q
popd

:end
set _all=
set _config=
set _compilator=
set _source=
set _buildHere=
set _march=
set _buildo_=
