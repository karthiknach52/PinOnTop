windres res.rc -O coff -o res.res
Write-Output "Resources Generated"

g++ -g -ggdb3 -o PinOnTop.exe main.cpp res.res
Write-Output "Compiled"
