@echo off
cd C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build
call vcvarsall.bat amd64

cd /d %~dp0
nmake
pause