@echo off
set _link_=%~dp0linkage
set _arch_=%~1
set _conf_=%~2

set INT24BITTYPES=C:\WORKSPACE\PROJECTS\GITSPACE\Int24Types\bin\cote5\%_arch_%\%_conf_%
set MOTORSPORTDLL=c:\WORKSPACE\PROJECTS\GITSPACE\MotorSports\bin\4.8\%_arch_%\%_conf_%

del /s /f /q "%_link_%\*.*"

copy "%MOTORSPORTDLL%\*.dll" "%_link_%"
copy "%MOTORSPORTDLL%\*.pdb" "%_link_%"

copy "%INT24BITTYPES%\*.dll" "%_link_%"
copy "%INT24BITTYPES%\*.pdb" "%_link_%"

set _link_=
set _arch_=
set _conf_=
