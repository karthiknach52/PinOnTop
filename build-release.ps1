windres res.rc -O coff -o res.res
Write-Output "Resources Generated"

g++ -mwindows -O3 -o PinOnTop.exe main.cpp res.res
Write-Output "Compiled"

strip PinOnTop.exe
Write-Output "Stripped"
