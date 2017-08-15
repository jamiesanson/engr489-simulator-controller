@echo off
python --version >nul 2>nul | find /i "Python 3.6" 
if not errorlevel 1 (
    WHERE pyinstaller >nul 2>nul
    IF %ERRORLEVEL% NEQ 0 pip install pyinstaller
    pyinstaller --onefile solar.py %*
    rmdir /s /q build
    del solar.spec
    copy dist\solar.exe 
    rmdir /s /q dist
) else (
    echo PyInstaller does not yet support Python 3.6.*
    echo Consider switching to a lower version of python, or compile on a different system
)