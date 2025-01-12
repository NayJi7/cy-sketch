@echo off
setlocal enabledelayedexpansion
echo This setup will install:
echo 1. SDL2 and MSYS2 (a PowerShell window will open)
echo 2. Python dependencies
echo.
echo In 3 seconds, the PowerShell window will open.
timeout /t 3 /nobreak > nul
echo.

REM Run PowerShell setup script
powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process PowerShell -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File \"%~dp0setup.ps1\"' -Verb RunAs"
REM Wait for PowerShell script to complete
echo Press any key to continue to the next step...
pause >nul

REM Change to the script's directory
cd /d "%~dp0"

echo.
echo Installing Python dependencies...

REM Check if Python is installed and version >= 3.6
python -c "import sys; assert sys.version_info >= (3,6)" > nul 2>&1
if errorlevel 1 (
    echo Error: Python 3.6 or higher is required!
    echo Current Python version:
    python --version
    echo.
    echo Please install Python 3.6 or higher from https://www.python.org/downloads/
    pause
    exit /b 1
)

REM Install/Upgrade pip
python -m pip install --upgrade pip

REM Install required Python packages
echo Installing Python packages from requirements.txt...
python -m pip install -r "%~dp0requirements.txt"

REM Function: Include Resources
echo.
echo Checking and including resources...

REM Define the base directories
set BASE_DIR=%~dp0..\..
set IDE_DIR=%BASE_DIR%\Draw++\IDE
set SDL_DIR=%BASE_DIR%\Draw++\SDL
set COMPILATOR_DIR=%BASE_DIR%\Draw++\COMPILATOR

REM Initialize PyInstaller arguments
set "PYINSTALLER_ARGS=--clean --onefile --windowed --name=DrawStudioCode"

REM Create a temporary file to store resource paths
del /f /q temp_resources.txt 2>nul
type nul > temp_resources.txt

REM Process each resource type separately
for %%F in ("%BASE_DIR%\Draw++\Makefile") do (
    if exist "%%~F" (
        echo Adding Makefile resource...
        echo %%~F;. >> temp_resources.txt
    )
)

for %%F in ("%BASE_DIR%\Draw++\interpreter.py") do (
    if exist "%%~F" (
        echo Adding interpreter resource...
        echo %%~F;. >> temp_resources.txt
    )
)

for %%F in ("%IDE_DIR%\*.ico" "%IDE_DIR%\*.svg" "%IDE_DIR%\*.png" "%IDE_DIR%\*.bmp") do (
    if exist "%%~F" (
        echo Adding IDE resource: %%~nxF
        set "rel_path=%%~dpF"
        set "rel_path=!rel_path:%BASE_DIR%\Draw++\=!"
        echo %%~F;!rel_path! >> temp_resources.txt
    )
)

for %%F in ("%SDL_DIR%\fonts\*.ttf" "%SDL_DIR%\files.h\*.h" "%SDL_DIR%\src\*.c") do (
    if exist "%%~F" (
        echo Adding SDL resource: %%~nxF
        set "rel_path=%%~dpF"
        set "rel_path=!rel_path:%BASE_DIR%\Draw++\=!"
        echo %%~F;!rel_path! >> temp_resources.txt
    )
)

for %%F in ("%COMPILATOR_DIR%\src\*.py") do (
    if exist "%%~F" (
        echo Adding Compilator resource: %%~nxF
        set "rel_path=%%~dpF"
        set "rel_path=!rel_path:%BASE_DIR%\Draw++\=!"
        echo %%~F;!rel_path! >> temp_resources.txt
    )
)

echo.

REM Build PyInstaller arguments from temp file
for /f "usebackq delims=" %%L in ("temp_resources.txt") do (
    for /f "tokens=1,2 delims=;" %%A in ("%%L") do (
        REM Handle special cases for Makefile and interpreter.py
        if /i "%%~nA"=="Makefile" (
            set "DEST=."
        ) else if /i "%%~nA"=="interpreter" (
            set "DEST=."
        ) else (
            REM Find and extract path after IDE, SDL or COMPILATOR
            for %%K in (IDE SDL COMPILATOR) do (
                echo %%B | findstr /i "%%K" >nul && (
                    for /f "tokens=* delims=" %%P in ('echo %%B ^| findstr /r ".*%%K.*"') do (
                        set "FULLPATH=%%P"
                        set "DEST=!FULLPATH:*%%K=%%K!"
                        set "DEST=!DEST:~0,-3!"
                    )
                )
            )
        )
        echo Transfering ressource: %%A to !DEST!
        set "PYINSTALLER_ARGS=!PYINSTALLER_ARGS! --add-data "%%A;!DEST!""
    )
)

del /f /q temp_resources.txt

echo Resources have been successfully included.

echo.
echo Building executable with PyInstaller...
cd /d "%BASE_DIR%"
echo Running PyInstaller with arguments: !PYINSTALLER_ARGS!
python -m PyInstaller !PYINSTALLER_ARGS! --icon=Draw++/IDE/Dpp_circle.ico Draw++/ide.py

echo.
echo Moving build files to Draw++/IDE...
if not exist "Draw++\IDE\build" mkdir Draw++\IDE\build
if exist build\* xcopy /E /I /Y build\* Draw++\IDE\build\
if exist DrawStudioCode.spec move DrawStudioCode.spec Draw++\IDE\
if exist build rmdir /S /Q build

echo Moving executable up one level...
if exist dist\DrawStudioCode.exe move dist\DrawStudioCode.exe DrawStudioCode.exe
if exist dist rmdir /S /Q dist

echo.
echo Setup completed successfully!
echo Executable DrawStudioCode.exe created 
echo.
echo Press any key to exit...
pause >nul
