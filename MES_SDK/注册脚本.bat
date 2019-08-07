@title 注册MES动态Dll

@echo off 
%1 mshta vbscript:CreateObject("Shell.Application").ShellExecute("cmd.exe","/c %~s0 ::","","runas",1)(window.close)

:Begin

:: 用户选择是注册还是卸载
@echo 请选择要进行的操作，然后按回车
@echo [0]注册
@echo [1]卸载
@echo.

::echo %SystemRoot%

@echo off
set /p var=请选择:
@echo.

IF "%var%"=="0" goto Install 
IF "%var%"=="1" goto Uninstall 
IF NOT "%var%"=="0" goto Begin
IF NOT "%var%"=="1" goto Begin

:Install
:: 执行注册
::regasm Kel.IFactory.Mqtt.Client.dll /codebase /tlb:Kel.IFactory.Mqtt.Client.tlb
@echo off 
%SystemRoot%/Microsoft.NET/Framework/v4.0.30319/RegAsm.exe Kel.IFactory.Mqtt.Client.dll /codebase /tlb:Kel.IFactory.Mqtt.Client.tlh
IF ERRORLEVEl 1 goto Failed
IF ERRORLEVEl 0 goto Success

:Uninstall
:: 执行卸载
::regasm Kel.IFactory.Mqtt.Client.dll /unregister
@echo off 
%SystemRoot%/Microsoft.NET/Framework/v4.0.30319/RegAsm.exe Kel.IFactory.Mqtt.Client.dll /unregister
IF ERRORLEVEl 1 goto Failed
IF ERRORLEVEl 0 goto Success

:Success
@echo 命令执行成功
goto End

:Failed
@echo 命令执行失败
@echo %errorlevel%
goto End

:End
pause