@echo off
make
if errorlevel 1 exit /b %errorlevel%
test.exe bin/test.hex
