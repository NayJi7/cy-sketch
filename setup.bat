@echo off
echo This setup will install:
echo 1. SDL2 and MSYS2 (if not already installed)
echo 2. Python dependencies
echo.

REM Run PowerShell setup script
powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process PowerShell -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File \"%~dp0setup.ps1\"' -Verb RunAs"
REM Wait a moment for PowerShell script to complete
timeout /t 2 /nobreak > nul

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
echo Setup completed successfully!
pause 