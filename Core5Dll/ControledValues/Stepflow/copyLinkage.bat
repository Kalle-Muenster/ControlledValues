@echo off
set _link_=%~dp0linkage
set _arch_=%~1
set _conf_=%~2

set INT24BITTYPES=C:\WORKSPACE\PROJECTS\Int24Types\bin\core5\%_arch_%\%_conf_%

copy "%INT24BITTYPES%\*.dll" "%_link_%"
copy "%INT24BITTYPES%\*.pdb" "%_link_%"

set _link_=
set _arch_=
set _conf_=
