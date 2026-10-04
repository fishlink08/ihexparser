@echo off
make
if errorlevel 1 exit /b %errorlevel%
test.exe test.hex
