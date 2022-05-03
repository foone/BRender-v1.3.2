rem @echo off
setlocal

set BR_BUILD=RELEASE
set BR_BASE_TYPE=FIXED

rem Build Fixed point drivers with Watcom 10.6
rem
call cwat106.bat


cdd %BR_SOURCE_DIR%\drivers\mcga
make TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9

cdd %BR_SOURCE_DIR%\drivers\vesa
make TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9

cdd %BR_SOURCE_DIR%\drivers\softprim
make TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9
make -f softi.mak  TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9
make -f softiz.mak TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9
make -f softh.mak  TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9
make -f softhz.mak TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9
make -f softt.mak  TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9
make -f softtz.mak TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9
make -f softb.mak  TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9

cd %BR_SOURCE_DIR%\drivers\softrend
make TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9

rem Build floating point renderer with Intel Reference Compiler
rem
call cmvc4.bat

set BR_MAKEFILE=proton.mak
set BR_BASE_TYPE=FLOAT

make TARGET_TYPE=DRIVER %1 %2 %3 %4 %5 %6 %7 %8 %9

cd ..

endlocal

