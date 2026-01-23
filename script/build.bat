@echo off
REM GenericScope Build Script

echo ========================================
echo Building GenericScope
echo ========================================

REM 设置环境变量
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
cd /d %PROJECT_ROOT%

REM 检查 Qt 环境
where qmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: qmake not found in PATH
    echo Please run this script from Qt Command Prompt
    pause
    exit /b 1
)

REM 检查 MSVC 环境
where cl >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: MSVC compiler not found in PATH
    echo Please run vcvarsall.bat first
    pause
    exit /b 1
)

REM 清理旧的构建文件
echo.
echo Cleaning old build files...
if exist Makefile del /Q Makefile
if exist Makefile.Debug del /Q Makefile.Debug
if exist Makefile.Release del /Q Makefile.Release

REM 运行 qmake
echo.
echo Running qmake...
qmake GenericScope.pro -spec win32-msvc
if %ERRORLEVEL% NEQ 0 (
    echo Error: qmake failed
    pause
    exit /b 1
)

REM 运行 qmake_all
echo.
echo Running qmake_all...
jom qmake_all
if %ERRORLEVEL% NEQ 0 (
    echo Error: qmake_all failed
    pause
    exit /b 1
)

REM 编译 Release 版本
echo.
echo Building Release version...
jom.exe -f Makefile.Release
if %ERRORLEVEL% NEQ 0 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo Output: Bin\x64\Release\GenericScope.exe
echo ========================================
pause
