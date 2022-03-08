@if not "%ECHO_STATE%"=="" ( @echo %ECHO_STATE% ) else ( @echo off )
pushd "%~dp0"
set _sourceroot_=%CD%
popd
set _all=
if _%1_==_ALL_ goto %1
if _%1_==_dot47_ goto DotNetDll
if _%1_==_test_ goto DotNetDll
if _%1_==_cpptests_ goto %1
if _%1_==_clean_ goto %1

if "%_next_%"=="DONE" goto END
set _next_=USAGE

:USAGE
echo.
echo usage:
echo build ^<Target^> [x86^|x64] [Debug^|Release] [toolversion] [OutputFolder]
echo.
echo Target: can be "dot47", "cpplib", "test", "cpptests".
echo.
echo run at best from within a MSBuild console 
echo (needs msbuild.exe and cl.exe in PATH) or  
echo even from within a build console with gcc
echo compiler in PATH (just cpptests). 
goto END

:: Initialization function (checks for compiler)
:INIT
if %_init%_==_init_ goto END
set _init=_init
if "%_sourceroot_%"=="%CD%" (
 md bin
 cd bin
) 
set _buildroot_=%CD%

MsBuild>checkforcompiler.log
if "%ERRORLEVEL%"=="1" (
	set _compilator=MsBuild
    	goto check_for_compilerversion
)
gcc --version>>checkforcompiler.log
if "%ERRORLEVEL%"=="0" (
	set _compilator=mingw32-make
    	goto %_next_%
) else (
    	goto USAGE
)
:check_for_compilerversion
if "%4"=="v142" (set _toolversi=v142)
if "%4"=="v141" (set _toolversi=v141)
if "%4"=="v140" (set _toolversi=v140)
if "%4"=="v120" (set _toolversi=v120)
if "%4"=="v120_xp" (set _toolversi=%4)
if "%_toolversi%"=="" (
if "%VisualStudioVersion%"=="16.0" (set _toolversi=v142)
if "%VisualStudioVersion%"=="15.0" (set _toolversi=v141)
if "%VisualStudioVersion%"=="14.0" (set _toolversi=v140)
if "%VisualStudioVersion%"=="12.0" (set _toolversi=v120)
)
echo using tool version^: %_toolversi%
pushd /d %_sourceroot_%
goto %_next_%

:: Targets:
:ALL
set _all=ALL

:DotNetDll
if "%~1"=="test" (
	set _all=ALL
	set _next_=ALL
) else (
	set _next_=DotNetDll
)
if not %_init%_==_init_ call :INIT %*
cd %_sourceroot_%\DotNetDll
if "%_toolversi%"=="%~4" (
        if "%~5"=="" (
		set _buildHere=%_buildroot_%
	) else (
		set _buildHere=%~5
	)
) else (
        if "%~4"=="" (
		set _buildHere=%_buildroot_%
	) else (
		set _buildHere=%~4
	)
)
call %_sourceroot_%\DotNetDll\Build.cmd "%~2" "%~3" %_toolversi% "%_buildHere%"
set _next_=
if _%1_==_ALL_ (
    popd
    goto cpptests
)
goto END

:cpptests
set _next_=cpptests_
if not %_init%_==_init_ call :INIT %*
:cpptests_
cd bin
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
set _buildHere=%_sourceroot_%\bin
::cd..
cd %_sourceroot_%\tst
:MSVC
echo current directory: %CD%
echo copy /Y Tests_%_toolversi%.vcxproj Tests.vcxproj
copy /Y Tests_%_toolversi%.vcxproj Tests.vcxproj
if not "%_compilator%"=="MsBuild" goto MINGW
set _config=Configuration=%_config%
set _march=Platform=%_march%
set _source=%_sourceroot_%\tst\Tests.sln
cd %_buildHere%
set _buildo_=/m /p
set _buildo_=%_buildo_%:OutDir=%_buildHere%
set _buildo_=%_buildo_%;%_config%
set _buildo_=%_buildo_%;%_march%
if not "%WindowsTargetPlatformVersion%"=="" set _buildo_=%_buildo_%;WindowsTargetPlatformVersion=%WindowsTargetPlatformVersion%
set _buildo_=%_buildo_% %_source%
goto MAKE
:MINGW
if not "%_compilator%"=="mingw32-make" goto USAGE
set _source=%_sourceroot_%\tst\Tests.mke
cd %_buildHere%
set _buildo_=%_source% %_config%_%_march%
:MAKE
set _buildo_=%_compilator% %_buildo_%
echo buildlog: cpptests>build_%1.log
echo build tool: %_toolversi%>>build_%1.log
echo build command: %_buildo_%>>build_%1.log
call %_buildo_%
echo ...Done! -^>  type: "tests" to start testrun...
echo.
set _next_=RunTest
goto END

:clean
popd
pushd %_sourceroot_%\tst
call MsBuild Tests.sln /t:Clean
del Release /f /s /q
rd Release /s /q
del Debug /f /s /q
rd Debug /s /q
del x64 /f /s /q
rd x64 /s /q
popd
pushd %_sourceroot_%\DotNetDLL
set _next_=clean
call Build %_next_% %_next_% %CD%
popd

:END
@echo off
if "%ECHO_STATE%"=="on" echo cleaning up script environment...
cd %_buildHere%
set _all=
set _config=
set _compilator=
set _source=
set _march=
set _buildo_=
set _init=
if "%_next_%"=="RunTest" (
	set _next_=
	Tests.exe -v
	call CreateSpectrumdataImages.bat
	call CompareWaveforms.bat
)
set _buildHere=
set _toolversi=
if "%_next_%"=="DONE" (	set _next_= && set _sourceroot_= && set _buildroot_= )
set _next_=
if "%ECHO_STATE%"=="on" @echo on
:RETURN
