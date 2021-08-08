@echo off

:: Variable
SET ARCH=%1
SET TYPE=%2
SET DISTRO=android

:: Default
IF [%1] == [] SET ARCH=armv7
IF [%2] == [] SET TYPE=shared

IF "%ARCH%" == "x86" (
	SET QMAKE=C:\qt\5.8\\android_x86\bin\qmake
) ELSE IF "%ARCH%" == "armv7" (
	SET QMAKE=C:\qt\5.8\\android_armv7\bin\qmake
) ELSE (
	goto usage
)

:start
echo %QMAKE% %cd%
call %QMAKE%

goto :eof

:usage
@echo Usage: %0 ^{x86^|armv7^}
exit /B 1
