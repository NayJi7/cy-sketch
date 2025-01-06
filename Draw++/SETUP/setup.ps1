# Windows Setup Script
$ErrorActionPreference = "Stop"
$msys2Path = "C:\msys64"

# Function to check MSYS2 and SDL2 installation
function Test-MSYS2Installation {
    # Check for critical MSYS2 components
    $requiredPaths = @(
        "$msys2Path\usr\bin\bash.exe",
        "$msys2Path\mingw64\bin\gcc.exe",
        "$msys2Path\mingw64\bin\SDL2.dll",
        "$msys2Path\mingw64\bin\mingw32-make.exe"
    )
    
    foreach ($path in $requiredPaths) {
        if (-not (Test-Path $path)) {
            return $false
        }
    }
    return $true
}

# Check if running as Administrator
if (-not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "Please run this script as Administrator" -ForegroundColor Red
    pause
    exit 1
}

Write-Host "Choose an option:" -ForegroundColor Yellow
Write-Host "1. Fresh installation of SDL2 and MSYS2" -ForegroundColor Cyan
Write-Host "2. Check for updates and update existing installation" -ForegroundColor Cyan
Write-Host "3. Skip SDL2/MSYS2 setup" -ForegroundColor Cyan
$choice = Read-Host "Enter your choice (1-3)"

if ($choice -eq "3") {
    Write-Host "Skipping SDL2 and MSYS2 setup..." -ForegroundColor Green
    exit 0
}

if ($choice -eq "2") {
    if (-not (Test-MSYS2Installation)) {
        Write-Host "`nMSYS2 or SDL2 components are missing or not properly installed." -ForegroundColor Red
        Write-Host "Please choose option 1 for a fresh installation to ensure all components are properly set up." -ForegroundColor Yellow
        pause
        exit 1
    }
    
    Write-Host "`nChecking for updates..." -ForegroundColor Yellow
    Write-Host "When the terminal opens, please run these commands:" -ForegroundColor Cyan
    Write-Host "1. pacman -Syu --noconfirm" -ForegroundColor Green
    Write-Host "2. pacman -Su --noconfirm" -ForegroundColor Green
    Write-Host "3. pacman -S --needed --overwrite '*' mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_gfx mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-python mingw-w64-x86_64-make --noconfirm" -ForegroundColor Green
    Write-Host "4. Type 'exit' when done" -ForegroundColor Green
    pause

    # Open MSYS2 terminal for updates
    $env:MSYSTEM = "MINGW64"
    Start-Process -FilePath "$msys2Path\usr\bin\bash.exe" -ArgumentList '--login' -Wait
    
    Write-Host "`nUpdate complete!" -ForegroundColor Green
    exit 0
}

# Fresh Installation (Option 1)
# Install MSYS2 if not present
if (-not (Test-Path "$msys2Path\usr\bin\bash.exe")) {
    Write-Host "Installing MSYS2..." -ForegroundColor Yellow
    
    # Download MSYS2
    $installer = "msys2-x86_64-latest.exe"
    $url = "https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe"
    
    Write-Host "Downloading MSYS2..." -ForegroundColor Yellow
    (New-Object System.Net.WebClient).DownloadFile($url, "$env:TEMP\$installer")
    
    # Run installer
    Write-Host "`nStarting MSYS2 installer..." -ForegroundColor Yellow
    Write-Host "1. Click Next" -ForegroundColor Cyan
    Write-Host "2. Keep the default path (C:\msys64)" -ForegroundColor Cyan
    Write-Host "3. Wait for installation to complete" -ForegroundColor Cyan
    pause
    Start-Process -FilePath "$env:TEMP\$installer" -Wait
    Remove-Item "$env:TEMP\$installer" -ErrorAction SilentlyContinue
}

# Setup MSYS2 packages
Write-Host "`nSetting up MSYS2 packages..." -ForegroundColor Yellow
Write-Host "When you will press Enter a terminal will open. Please run these commands: (Select and right click to copy paste)" -ForegroundColor Cyan
Write-Host "If everything closes without finishing, just restart the bat script again" -ForegroundColor Cyan
Write-Host "1. pacman -Scc --noconfirm" -ForegroundColor Green
Write-Host "2. pacman -Syyu --noconfirm" -ForegroundColor Green
Write-Host "3. pacman -S --needed --overwrite '*' mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_gfx mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-python mingw-w64-x86_64-make --noconfirm" -ForegroundColor Green
Write-Host "4. Type 'exit' when done" -ForegroundColor Green
Write-Host "`nNote: After installation, use 'mingw32-make' instead of 'make' in your commands" -ForegroundColor Yellow
pause

# Open MSYS2 terminal
$env:MSYSTEM = "MINGW64"
Start-Process -FilePath "$msys2Path\usr\bin\bash.exe" -ArgumentList '--login' -Wait

# Add to PATH
$pathsToAdd = @("$msys2Path\mingw64\bin", "$msys2Path\usr\bin")
$currentPath = [Environment]::GetEnvironmentVariable("Path", "Machine")
foreach ($path in $pathsToAdd) {
    if ($currentPath -notlike "*$path*") {
        $currentPath = "$currentPath;$path"
        [Environment]::SetEnvironmentVariable("Path", $currentPath, "Machine")
    }
}

# Create make alias
$aliasPath = "$env:USERPROFILE\make.bat"
@"
@echo off
mingw32-make %*
"@ | Out-File -FilePath $aliasPath -Encoding ASCII

# Add alias directory to PATH if not already there
$aliasDir = "$env:USERPROFILE"
if ($currentPath -notlike "*$aliasDir*") {
    $currentPath = "$currentPath;$aliasDir"
    [Environment]::SetEnvironmentVariable("Path", "Machine", $currentPath)
}

Write-Host "`nSetup complete!" -ForegroundColor Green
Write-Host "Please restart your terminal/IDE to use the new environment" -ForegroundColor Yellow
Write-Host "You can now use either 'make' or 'mingw32-make' to build your project" -ForegroundColor Green
pause 