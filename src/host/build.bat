@echo off
pyinstaller --onefile solar.py %*
rmdir /s /q build
del solar.spec
copy dist\solar.exe 
rmdir /s /q dist