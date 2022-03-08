@if not "%ECHO_STATE%"=="" ( @echo %ECHO_STATE% ) else ( @echo off ) 

set _dst_=%~2
set _src_=%~1\DotNetDll\%~3\%~4
set _consola_=C:\WORKSPACE\PROJECTS\ConsolaStreams\bin\v4.8\%~3\%~4\StdStreams.dll

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
del /f /s /q "%_dst_%\StdStreams_old.dll"
rename "%_dst_%\StdStreams.dll" "%_dst_%\StdStreams_old.dll"
echo copy /Y /B "%_consola_%" "%_dst_%\StdStreams.dll"
copy /Y /B "%_consola_%" "%_dst_%\StdStreams.dll"
set _dst_=
set _src_=
set _consola_=
