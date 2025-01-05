@echo off
echo This setup will install:
echo 1. SDL2 and MSYS2 (a PowerShell window will open)
echo 2. Python dependencies
echo.
echo in 3 seconds, the PowerShell window will open.
timeout /t 3 /nobreak > nul
echo.


REM Run PowerShell setup script
powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process PowerShell -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File \"%~dp0setup.ps1\"' -Verb RunAs"
REM Wait a moment for PowerShell script to complete
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

echo.
echo Building executable with PyInstaller...
cd ..
cd ..
python -m PyInstaller --clean --onefile --windowed --icon=Draw++/IDE/Dpp_circle.ico --name="DrawStudioCode" Draw++/ide.py

echo.
echo Moving build files to Draw++/IDE...
if not exist "Draw++\IDE\build" mkdir Draw++\IDE\build
xcopy /E /I /Y build\* Draw++\IDE\build\
move DrawStudioCode.spec Draw++\IDE\
rmdir /S /Q build

echo Moving executable up one level...
move dist\DrawStudioCode.exe
rmdir /S /Q dist

echo.
echo Setup completed successfully!
echo Executable DrawStudioCode.exe created 
echo.
echo Press any key to continue to exit...
pause >nul