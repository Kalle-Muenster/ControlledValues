@echo off
cd "%~dp0"
echo.
goto BEGIN

:USAGE
echo Usage:
echo.
echo StartServer [options] ^<color^> ^<port^>
echo.
echo Required parameters:
echo.
echo    either a RUBY or a PYTHON interpreter need to be executable via PATH environment variable.
echo.
echo      ^<color^>       :   Sellect color of which quallen will be served by the QuallenServer.
echo      ^<port^>        :   Sellect a port number where QuallenServer will listen for requests.
echo.
echo Avialible options:
echo.
echo    --autostart     :   Adds a registry-entry to let begin serving quallen on System-startup.
echo    --no-autostart  :   Remove Autostart-Entry (priviously set by --autostart) from the Registry.
echo    --help/-h       :   Displays this message.
echo.
echo Supported colors:
echo.
echo      Colors which can be given as the ^<color^> parameter are: cyan, pink, gelb
echo.
goto END

:BEGIN
if "%~1"=="--autostart" (
    set _farb_=%~2
    set _port_=%~3
) else if "%~1"=="--no-autostart" (
    set _farb_=%~2
    set _port_=%~3
) else (
    set _farb_=%~1
    set _port_=%~2
)

if "%~2"=="" (
	set _port_=5678
) 
if "%~1"=="" (
	set _farb_=cyan
) 
 
if "%1"=="--help" ( goto USAGE )
    if "%1"=="-h" ( goto USAGE )
if NOT "%_farb_%"=="cyan" if NOT "%_farb_%"=="pink" if NOT "%_farb_%"=="gelb" goto USAGE
if not exist %~dp0StartServer.cmd goto USAGE

if "%~1"=="--autostart" (
	REG ADD HKCU\Software\Microsoft\Windows\CurrentVersion\Run /v QuallenServer_%_farb_% /t REG_SZ /d "%~dp0StartServer.cmd %_farb_% %_port_%"
	echo Added QuallenServer_%_farb_% on port %_port_% to autostart!
	goto END
)
if "%~1"=="--no-autostart" (
	REG DELETE HKCU\Software\Microsoft\Windows\CurrentVersion\Run /v QuallenServer_%_farb_%
	echo Removed QuallenServer_%_farb_% from autostart!
	goto END
)

	set _ntrprtr_=python.exe
	set _scrptex_=py
	set _hostadr_=
	set _srvroot_= %~dp0root_%_farb_%
	goto LOS
	
set NEXT=NICH
:TRY
ruby -v
if "%errorlevel%"=="0" (
	set _ntrprtr_=ruby.exe
	set _scrptex_=rb
	set _hostadr_= %COMPUTERNAME%
	set _srvroot_= %~dp0root_%_farb_%
	goto LOS
) 
:: python -v
:: if "%errorlevel%"=="0" (

:: )
if %NEXT%==PYTI set NEXT=PECH
if %NEXT%==RUBY set NEXT=PYTI
if %NEXT%==NICH set NEXT=RUBY
goto %NEXT%

:RUBY
call use -RB
goto TRY
:PYTI
call use -PY
goto TRY

:PECH
goto USAGE

:LOS
set _command_=%_ntrprtr_% %~dp0http.%_scrptex_%%_hostadr_% %_port_%%_srvroot_%
echo %_command_%
cd %_srvroot_%
start "" %_command_%
cd..
cd..

:END
set _ntrprtr_=
set _scrptex_=
set _hostadr_=
set _srvroot_=
set _command_=
set _farb_=
set _port_=
set NEXT=
echo.