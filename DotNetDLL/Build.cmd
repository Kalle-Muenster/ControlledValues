@if not "%ECHO_STATE%"=="" ( @echo %ECHO_STATE% ) else ( @echo off ) 
:: %1 is Architecture
:: %2 is Configuration
:: %3 override toolset version. (if not set, version of compiler which currently is in the PATH environment will be used)
:: %4 (or even %3) can be OutputPath

if "%_sourceroot_%"=="" goto DONE
echo.

::set _march=
::set _config=
::set _output=
::set _compilator=
::set _BuildAll_=
::set _march_path_=
::set _here=


if "%_next_%"=="ALL" (
	set _next_=DotNetDll
	set _BuildAll_=QuallenTest
) else (
	set _BuildAll_=DONE
)

set _config=Release
if not "%~2"=="" (
	set _config=%~2
) else (
	if not "%~2"=="clean" (
		echo will build target: "%_next_%" into folder^: "%_output%"
	) else (
		set _next_=clean
	)
)

MsBuild>Build.log
if "%ERRORLEVEL%"=="1" (
	set _compilator=MsBuild
) else (
	echo ERROR: MsBuild tool not in PATH!
	goto RETURN
)

if "%_toolversi%"=="" (set _toolversi=v140 )
if "%VisualStudioVersion%"=="16.0" (set _toolversi=v142 )
if "%VisualStudioVersion%"=="15.0" (set _toolversi=v141 )
if "%VisualStudioVersion%"=="14.0" (set _toolversi=v140 )
if "%VisualStudioVersion%"=="12.0" (set _toolversi=v120 )
if "%3"=="v142" (goto SET_TOOLVERSION )
if "%3"=="v141" (goto SET_TOOLVERSION )
if "%3"=="v140" (goto SET_TOOLVERSION )
if "%3"=="v120" (goto SET_TOOLVERSION )
goto BUILD_DOTNET_DLL

:SET_TOOLVERSION
set _toolversi=%3
set THERE_ARE_FOUR_PARAMETER=true
echo THERE_ARE_FOUR_PARAMETER=%THERE_ARE_FOUR_PARAMETER%

:BUILD_DOTNET_DLL
if not "%_next_%"=="DotNetDll" goto NEXT

set _output="%~3"
if "%THERE_ARE_FOUR_PARAMETER%"=="true" set _output="%~4"
if "%_output%"=="" (
	cd %_sourceroot_%
	md bin
	set _output=%_sourceroot_%\bin
)

echo building target: %_next_%
echo using toolversion %_toolversi%
echo into directory: %_output%

cd /d %_sourceroot_%
cd /d %_output%

md DotNetDLL
cd DotNetDLL
if "%~1"=="x64" (
    set _march=x64
    set _march_path_=x64
) else (
    set _march=Win32
    set _march_path_=x86
)
md %_march_path_%
cd %_march_path_%

set _config=Release
if not "%~2"=="" set _config=%~2
md %_config%
cd %_config%

set _buildHere=%CD%
pushd %_sourceroot_%\DotNetDLL\ControlledValues\ControlledValues
set _source=%_sourceroot_%\DotNetDLL\ControlledValues\ControlledValues\ControlledValues.vcxproj
popd

set _buildo_=/m /p
set _buildo_=%_buildo_%:OutDir=%_buildHere%\
set _buildo_=%_buildo_%;Configuration=%_config%
set _buildo_=%_buildo_%;Platform=%_march%
if not "%WindowsTargetPlatformVersion%"=="" set _buildo_=%_buildo_%;WindowsTargetPlatformVersion=%WindowsTargetPlatformVersion%
set _buildo_=%_buildo_% %_source%
set _buildo_=%_compilator% %_buildo_%
echo build command: %_buildo_%>%_buildHere%\build.log
echo tool version: %_toolversi%>>%_buildHere%\build.log
set _buildo_=%_buildo_% /fl /flp:LogFile=%_buildHere%\build.log;Append
call %_buildo_%
cd %_sourceroot_%\DotNetDLL
set _next_=%_BuildAll_%
if not "%_BuildAll_%"=="QuallenTest" set _BuildAll_=RETURN


:NEXT
if not "%_next_%"=="QuallenTest" goto RETURN
set _BuildAll_=DONE
echo building target: %_next_%
echo using toolversion %_toolversi%
set _output="%~3"
if "%THERE_ARE_FOUR_PARAMETER%"=="true" set _output="%~4"
if "%_output%"=="" (
	cd %_sourceroot_%
	md bin
	set _output=%_sourceroot_%\bin
)
cd /d %_output%
md DotNetDLL
cd DotNetDLL

if "%~1"=="x64" (
    set _march=x64
) else (
    set _march=x86
)
md %_march%
cd %_march%

md Test
cd Test
md %_config%
cd %_config%
set _buildHere=%CD%

cd /d %_sourceroot_%\DotNetDLL\Tests\Q2wpf2
set _source=%CD%\Qualle.sln
cd Q2wpf2
call copylinkage %_output% "%_sourceroot_%\DotNetDLL\Tests\Q2wpf2\Q2wpf2\linkage" "%_march%" "%_config%"

cd %_buildHere%
set _buildo_=/m /p
set _buildo_=%_buildo_%:OutDir=%_buildHere%\
set _buildo_=%_buildo_%;Configuration=%_config%
if not "%WindowsTargetPlatformVersion%"=="" set _buildo_=%_buildo_%;WindowsTargetPlatformVersion=%WindowsTargetPlatformVersion%
set _buildo_=%_buildo_%;Platform=%_march%
set _buildo_=%_buildo_% %_source%
set _buildo_=%_compilator% %_buildo_%
echo build command: %_buildo_%>%_buildHere%\build.log
echo tool version: %_toolversi%>>%_buildHere%\build.log
set _buildo_=%_buildo_% /fl /flp:LogFile=%_buildHere%\build.log;Append
call %_buildo_%
cd %_sourceroot_%
::set _next_=RETURN
set _next_=clean
goto %_next_%

:clean
if not "%_next_%"=="clean" goto RETURN
cd %_sourceroot_%\DotNetDll\ControlledValues
cd ControlledValues
del Win32 /f /s /q
rd Win32 /s /q
del x64 /f /s /q
rd x64 /s /q
del obj /f /s /q
rd obj /s /q
cd %_sourceroot_%\DotNetDll\Tests\Q2wpf2
cd Q2wpf2
cd Q2wpf2
del linkage /f /s /q
del obj /f /s /q
rd obj /s /q
del bin /f /s /q
rd bin /s /q
cd %_sourceroot_%\DotNetDll\Tests
del obj /f /s /q
rd obj /s /q
del bin /f /s /q
rd bin /s /q
cd %_sourceroot_%
cd %_buildHere%
set _next_=%_BuildAll_%
set _BuildAll_=DONE

:RETURN
if "%_next_%"=="clean" goto clean
:DONE
@echo off
if "%ECHO_STATE%"=="on" echo ...leaving dll-buildscript
set _march=
set _config=
set _output=
set _compilator=
set _BuildAll_=
set _march_path_=
set _here=
set THERE_ARE_FOUR_PARAMETER=
echo.
if "%ECHO_STATE%"=="on" @echo on