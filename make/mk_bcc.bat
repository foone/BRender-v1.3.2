@echo off
rem Build Borland C++ 4.5 Libraries

setlocal
cdd %BR_SOURCE_DIR%\core

rem Build core libraries
rem
set BR_BASE_TYPE=FIXED
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_BASE_TYPE=FLOAT
make %1 %2 %3 %4 %5 %6 %7 %8 %9

rem Build different extender versions of host
rem

cdd %BR_SOURCE_DIR%\core\host

set BR_EXTENDER=PHARLAP
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_EXTENDER=POWERPACK
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_EXTENDER=WIN_32
make %1 %2 %3 %4 %5 %6 %7 %8 %9

cd ..
endlocal

