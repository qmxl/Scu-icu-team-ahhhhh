@echo off
chcp 65001 >nul
echo Building campus trade system backend...
echo.

cd /d "%~dp0"

if exist build (
    echo Cleaning old build directory...
    rmdir /s /q build
)

echo Creating build directory...
mkdir build
cd build

echo Generating build configuration...
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build succeeded!
echo Executable: %~dp0build\Debug\campus_trade.exe
pause