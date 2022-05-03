@echo off
rem Build Watcom Libraries
rem

setlocal

cdd %BR_SOURCE_DIR%\core

rem Build core libraries
rem

set BR_BASE_TYPE=FIXED

set BR_LIB_SUFFIX=r
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_LIB_SUFFIX=s
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_BASE_TYPE=FLOAT

set BR_LIB_SUFFIX=r
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_LIB_SUFFIX=s
make %1 %2 %3 %4 %5 %6 %7 %8 %9

rem Build different extender versions of host
rem

cdd %BR_SOURCE_DIR%\core\host

set BR_EXTENDER=DOS4GW

set BR_LIB_SUFFIX=r
make %1 %2 %3 %4 %5 %6 %7 %8 %9
set BR_LIB_SUFFIX=s
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_EXTENDER=X32VM

set BR_LIB_SUFFIX=r
make %1 %2 %3 %4 %5 %6 %7 %8 %9
set BR_LIB_SUFFIX=s
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_EXTENDER=PHARLAP

set BR_LIB_SUFFIX=r
make %1 %2 %3 %4 %5 %6 %7 %8 %9
set BR_LIB_SUFFIX=s
make %1 %2 %3 %4 %5 %6 %7 %8 %9

set BR_EXTENDER=WIN_32

set BR_LIB_SUFFIX=r
make %1 %2 %3 %4 %5 %6 %7 %8 %9
set BR_LIB_SUFFIX=s
make %1 %2 %3 %4 %5 %6 %7 %8 %9

endlocal

