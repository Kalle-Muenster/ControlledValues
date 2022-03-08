@echo off
cd "%~dp0DotNetDLL\Tests\QuallenServer"
call "%~dp0DotNetDLL\Tests\QuallenServer\StartServer.cmd" %~1 %~2 %~3
cd "%~dp0"
echo.
echo started Quallenserver! 
echo.