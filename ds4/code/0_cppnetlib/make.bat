@echo off

SET COMPILER=C:\Qt\Tools\mingw530_32\bin\mingw32-make.exe

IF [%1] == [] (
	echo %COMPILER%
	call %COMPILER%
) ELSE (
	echo %COMPILER% %1 %2
	call %COMPILER% %1 %2
)
