@echo off
echo [Compiler] Setting up Visual Studio C++ build environment...
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" > nul 2>&1
if %errorlevel% neq 0 (
    echo [Error] Failed to load vcvars64.bat. Please verify Visual Studio installation path.
    exit /b %errorlevel%
)

echo [Compiler] Compiling C++ project with UTF-8 support...
cl.exe /EHsc /std:c++17 /utf-8 /Fe:TicketSystem.exe src\main.cpp

if %errorlevel% equ 0 (
    echo [Success] Compilation succeeded! Executable generated: TicketSystem.exe
) else (
    echo [Failure] Compilation failed!
)
