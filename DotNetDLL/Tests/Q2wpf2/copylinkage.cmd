@echo off

set _src_="%~1"
set _dst_="%~2"

echo will copy ControlledValues dll from^:
echo %_src_%
echo To sourcefolder: %_dst_%

cd "%_src_%"
copy /Y /B *.* "%_dst_%"
if not "%ERRORLEVEL%"=="0" (
    rename "%_dst_%\ControlledValues.dll" "%_dst_%\ControlledValues_old.dll"
    copy /Y /B ControlledValues.dll "%_dst_%\ControlledValues.dll"
)

set _dst_=
set _src_=
