@if not "%ECHO_STATE%"=="" ( @echo %ECHO_STATE% ) else ( @echo off ) 

set _dst_=%~dp0linkage
set _src_=C:\WORKSPACE\PROJECTS\GITSPACE\ControlledValues\bin\DotNetDll\%~2\%~3
set _consola_=C:\WORKSPACE\PROJECTS\GITSPACE\Consola\bin\v4.8\%~2\%~3\Consola.dll

echo will copy ControlledValues dll from^:
echo %_src_%
echo To linkagefolder: %_dst_%

cd "%_src_%"
copy /Y /B *.* "%_dst_%"
if not "%ERRORLEVEL%"=="0" (
    del /f /s /q "%_dst_%\ControlledValues_old.dll"
    rename "%_dst_%\ControlledValues.dll" "%_dst_%\ControlledValues_old.dll"
    copy /Y /B "%_src_%\ControlledValues.dll" "%_dst_%\ControlledValues.dll"
)
del /f /s /q "%_dst_%\Consola_old.dll"
rename "%_dst_%\Consola.dll" "%_dst_%\Consola_old.dll"
echo copy /Y /B "%_consola_%" "%_dst_%\Consola.dll"
copy /Y /B "%_consola_%" "%_dst_%\Consola.dll"
set _dst_=
set _src_=
set _consola_=
