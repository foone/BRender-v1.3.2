@echo off
cd inc
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\fw
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\std
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\math
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\fmt
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\v1db
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\pixelmap
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\dosio
make %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

