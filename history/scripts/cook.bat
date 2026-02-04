@echo off
setlocal

REM Define paths
set SCRIPT_DIR=%~dp0
set BASE_DIR=%SCRIPT_DIR%..
set RELEASE_DIR=%BASE_DIR%\sln\Bo\x64\Release
set TXT_DIR=%BASE_DIR%
set OUTPUT_ZIP=%SCRIPT_DIR%Generator.zip
set SEVENZIP=C:\Program Files\NVIDIA Corporation\NVIDIA App\7z.exe

REM Remove old zip if exists
if exist "%OUTPUT_ZIP%" del "%OUTPUT_ZIP%"

REM Use 7z to add files with flat structure
"%SEVENZIP%" a -tzip "%OUTPUT_ZIP%" "%RELEASE_DIR%\*.exe" "%TXT_DIR%\sta*.txt" -r -y

echo Created: %OUTPUT_ZIP%
endlocal
