@echo off
REM		Build Script

set OUTPUT=bin2cpp.exe

del %OUTPUT% 2>nul

g++ -std=c++20 -O3 bin2cpp.cpp -static -o %OUTPUT%

:finish
if exist .\%OUTPUT% (
	echo Build Success!
) else (
	echo Build Failed!
)