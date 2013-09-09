@echo off
rem CLEAN-UP batch file
CHOICE /C YN /N /M "Do you realy want to clean-up ALL DATA [Y\N]?"
IF ERRORLEVEL 2 GOTO dn

cls
echo "IntelliSense clean-up..."
del /F /S /Q *.ncb

pause
cls
echo "Precompilet object code clean-up..."
del /F /S /Q *.obj

pause
cls
echo "Precompilet headers clean-up..."
del /F /S /Q *.pch

pause
cls
echo "Debug servise info clean-up..."
del /F /S /Q *.pdb
del /F /S /Q *.tlog
rmdir /S /Q ipch

pause
cls
echo "Incremental linker clean-up..."
del /F /S /Q *.ilk
del /F /S /Q *.idb

:dn
echo "DONE!"
pause