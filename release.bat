@echo off
rmdir /S /Q bin
mkdir bin\x64

xcopy /s /q x64\Release\* bin\x64\