@echo off

:: Variable
SET CMD=%0
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
echo %QMAKE% 4_config.pro
call %QMAKE% 4_config.pro
echo.

:: 3rd Party
cd 0_cppnetlib
call %CMD% %ARCH% %TYPE%
echo.
cd ..

cd 0_cryptopp
call %CMD% %ARCH% %TYPE%
echo.
cd ..

cd 0_msgpack
call %CMD% %ARCH% %TYPE%
echo.
cd ..

:: Library
cd 4_core
call %CMD% %ARCH% %TYPE%
echo.
cd ..

cd 4_core_gui
call %CMD% %ARCH% %TYPE%
echo.
cd ..

cd 4_core_client
call %CMD% %ARCH% %TYPE%
echo.
cd ..

:: Application
cd 4_client_cli
call %CMD% %ARCH% %TYPE%
echo.
cd ..

cd 4_router_cli
call %CMD% %ARCH% %TYPE%
echo.
cd ..

cd 4_client_desktop
call %CMD% %ARCH% %TYPE%
::echo.
cd ..
