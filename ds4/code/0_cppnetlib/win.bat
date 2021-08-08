@echo off

:: Variable
SET ARCH=%1
SET TYPE=%2

:: Default
IF [%1] == [] SET ARCH=64
IF [%2] == [] SET TYPE=static

:: Variable
IF "%TYPE%" == "static" (
    SET QMAKE=C:\Qt\%QT_VERSION%\msvc2017_%ARCH%_static\bin\qmake
) ELSE IF "%TYPE%" == "ltcg" (
	SET QMAKE=C:\Qt\%QT_VERSION%\msvc2017_%ARCH%_ltcg\bin\qmake
) ELSE (
	IF "%ARCH%" == "32" (
		SET QMAKE=C:\Qt\%QT_VERSION%\msvc2017\bin\qmake
	) ELSE (
		SET QMAKE=C:\Qt\%QT_VERSION%\msvc2017_%ARCH%\bin\qmake
	)
)

:start
echo %QMAKE%
call %QMAKE%
