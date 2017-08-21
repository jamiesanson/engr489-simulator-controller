@echo off
WHERE pyinstaller >nul 2>nul
IF %ERRORLEVEL% NEQ 0 pip install pyinstaller
pyinstaller --onefile solar.py %*
rmdir /s /q build
del solar.spec
copy dist\solar.exe 
rmdir /s /q dist