@echo off

set assignment=%1
set assignment_number=%assignment:~-1%

if "%~1"=="" goto :error

set test=%2

set proj_name=%assignment%

if "%assignment%" NEQ "tech_proj" if "%assignment%" NEQ "dirt_proj" set proj_name=assignment%assignment_number%

cmake --build ./build --config Release --target %proj_name%
if not %ERRORLEVEL% == 0 goto :error

cd assignments\%assignment%
..\..\build\assignments\%assignment%\Release\%proj_name%.exe %test%
cd ..\..\
if not %ERRORLEVEL% == 0 goto :error



goto :endofscript

:error
echo =========================
echo There was an error!
echo =========================

:endofscript
