@echo off
setlocal enabledelayedexpansion


::	-----------------------------------------------------------------
::	This generate script will auto search and generate resource files
::	-----------------------------------------------------------------


:: Output Resource Header File
set RESOURCE_HEADER=resources.h

::Bin to CPP Application
set BIN2CPP=bin2cpp.exe


:: Search For File Extentions

set EXT[0]=*.ogg
set EXT[1]=*.png
:: set EXT[2]=*.ext
:: set EXT[3]=*.ext
:: set EXT[4]=*.ext





:: ------------------------
::		Program Start
:: ------------------------

if NOT EXIST %BIN2CPP% (
	echo Could not find the %BIN2CPP% binary
	goto end
)

:: LF Hack
(
set NLM=^
%=EMPTY=%
)
set NL=^^^%NLM%%NLM%^%NLM%


echo Export Resource Files...

for /F "tokens=2 delims==" %%s in ('set EXT') do (
	for %%F in (%%s) do (
		if not exist %%~nF.cpp (
			echo Exporting %%~nF.cpp
			start /B /WAIT "%%~nF" %BIN2CPP% %%F %%~nF %%~nF
		)
	)
)

del /S /Q %RESOURCE_HEADER%>nul

(
set "files=#pragma once!NL!"
for /f "delims=" %%A in ('dir /b /a-d "*.h" ') do set "files=!files! !NL! #include "%%A""
)

echo %files%>%RESOURCE_HEADER%

:end