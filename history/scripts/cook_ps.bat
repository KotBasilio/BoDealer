@echo off
setlocal

REM Define paths
set SCRIPT_DIR=%~dp0
set BASE_DIR=%SCRIPT_DIR%..
set RELEASE_DIR=%BASE_DIR%\sln\Bo\x64\Release
set TXT_DIR=%BASE_DIR%
set OUTPUT_ZIP=%SCRIPT_DIR%\Generator.zip

REM Remove old zip if exists
if exist "%OUTPUT_ZIP%" del "%OUTPUT_ZIP%"

REM Copy files to a temp folder for flat structure
set TEMP_DIR=%SCRIPT_DIR%\temp_zip
if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%"
mkdir "%TEMP_DIR%"

copy "%RELEASE_DIR%\*.exe" "%TEMP_DIR%"
copy "%TXT_DIR%\sta*.txt" "%TEMP_DIR%"

REM Use PowerShell to zip
powershell -command "Compress-Archive -Path '%TEMP_DIR%\*' -DestinationPath '%OUTPUT_ZIP%'"

REM Clean up
rmdir /s /q "%TEMP_DIR%"

echo Created: %OUTPUT_ZIP%
endlocal
