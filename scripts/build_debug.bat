@echo off
rem set up MSVC env in this same cmd session
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

rem build the solution (path is relative from scripts\ folder -> ../sln/Bo/Bo.sln)
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "%~dp0\..\sln\Bo\Bo.sln" /p:Configuration=Debug /p:Platform=x64
