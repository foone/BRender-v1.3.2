set DIST_DIR=%1
rem Core Libraries - Release Version
rem

copy lib\brdbbfr.lib     %DIST_DIR%\lib
copy lib\brdbbxr.lib     %DIST_DIR%\lib
copy lib\brdbmfr.lib     %DIST_DIR%\lib
copy lib\brdbmxr.lib     %DIST_DIR%\lib
copy lib\brdbwfrr.lib    %DIST_DIR%\lib
copy lib\brdbwfrs.lib    %DIST_DIR%\lib
copy lib\brdbwxrr.lib    %DIST_DIR%\lib
copy lib\brdbwxrs.lib    %DIST_DIR%\lib

copy lib\brfmbfr.lib     %DIST_DIR%\lib
copy lib\brfmbxr.lib     %DIST_DIR%\lib
copy lib\brfmmfr.lib     %DIST_DIR%\lib
copy lib\brfmmxr.lib     %DIST_DIR%\lib
copy lib\brfmwfrr.lib    %DIST_DIR%\lib
copy lib\brfmwfrs.lib    %DIST_DIR%\lib
copy lib\brfmwxrr.lib    %DIST_DIR%\lib
copy lib\brfmwxrs.lib    %DIST_DIR%\lib

copy lib\brfwbfr.lib     %DIST_DIR%\lib
copy lib\brfwbxr.lib     %DIST_DIR%\lib
copy lib\brfwmfr.lib     %DIST_DIR%\lib
copy lib\brfwmxr.lib     %DIST_DIR%\lib
copy lib\brfwwfrr.lib    %DIST_DIR%\lib
copy lib\brfwwfrs.lib    %DIST_DIR%\lib
copy lib\brfwwxrr.lib    %DIST_DIR%\lib
copy lib\brfwwxrs.lib    %DIST_DIR%\lib

copy lib\brmtbfr.lib     %DIST_DIR%\lib
copy lib\brmtbxr.lib     %DIST_DIR%\lib
copy lib\brmtmfr.lib     %DIST_DIR%\lib
copy lib\brmtmxr.lib     %DIST_DIR%\lib
copy lib\brmtwfrr.lib    %DIST_DIR%\lib
copy lib\brmtwfrs.lib    %DIST_DIR%\lib
copy lib\brmtwxrr.lib    %DIST_DIR%\lib
copy lib\brmtwxrs.lib    %DIST_DIR%\lib

copy lib\brpmbr.lib      %DIST_DIR%\lib
copy lib\brpmmr.lib      %DIST_DIR%\lib
copy lib\brpmwrr.lib     %DIST_DIR%\lib
copy lib\brpmwrs.lib     %DIST_DIR%\lib

copy lib\brstbr.lib      %DIST_DIR%\lib
copy lib\brstmr.lib      %DIST_DIR%\lib
copy lib\brstwrr.lib     %DIST_DIR%\lib
copy lib\brstwrs.lib     %DIST_DIR%\lib

rem Core Headers
rem

copy inc\actor.h         %DIST_DIR%\inc
copy inc\angles.h        %DIST_DIR%\inc
copy inc\brdiag.h        %DIST_DIR%\inc
copy inc\brender.h       %DIST_DIR%\inc
copy inc\brerror.h       %DIST_DIR%\inc
copy inc\brfile.h        %DIST_DIR%\inc
copy inc\brfont.h        %DIST_DIR%\inc
copy inc\brlimits.h      %DIST_DIR%\inc
copy inc\brmem.h         %DIST_DIR%\inc
copy inc\camera.h        %DIST_DIR%\inc
copy inc\colour.h        %DIST_DIR%\inc
copy inc\compend.h       %DIST_DIR%\inc
copy inc\compiler.h      %DIST_DIR%\inc
copy inc\custsupt.h      %DIST_DIR%\inc
copy inc\fixed.h         %DIST_DIR%\inc
copy inc\fmt.h           %DIST_DIR%\inc
copy inc\fmt_p.h         %DIST_DIR%\inc
copy inc\fw_p.h          %DIST_DIR%\inc
copy inc\light.h         %DIST_DIR%\inc
copy inc\material.h      %DIST_DIR%\inc
copy inc\math_p.h        %DIST_DIR%\inc
copy inc\math_x.h        %DIST_DIR%\inc
copy inc\matrix.h        %DIST_DIR%\inc
copy inc\model.h         %DIST_DIR%\inc
copy inc\mscifix.h       %DIST_DIR%\inc
copy inc\order.h         %DIST_DIR%\inc
copy inc\pixelmap.h      %DIST_DIR%\inc
copy inc\pm_p.h          %DIST_DIR%\inc
copy inc\pool.h          %DIST_DIR%\inc
copy inc\pretok.h        %DIST_DIR%\inc
copy inc\quat.h          %DIST_DIR%\inc
copy inc\render.h        %DIST_DIR%\inc
copy inc\scalar.h        %DIST_DIR%\inc
copy inc\token.h         %DIST_DIR%\inc
copy inc\transfrm.h      %DIST_DIR%\inc
copy inc\v1db_p.h        %DIST_DIR%\inc
copy inc\v1zb_p.h        %DIST_DIR%\inc
copy inc\vecfns.h        %DIST_DIR%\inc
copy inc\brvector.h      %DIST_DIR%\inc
copy inc\brvector.h      %DIST_DIR%\inc\vector.h
copy inc\wtcifix.h       %DIST_DIR%\inc

rem Drivers
rem

copy dll\mcga.bdd        %DIST_DIR%\dat
copy dll\softh.bdd       %DIST_DIR%\dat
copy dll\softhz.bdd      %DIST_DIR%\dat
copy dll\softi.bdd       %DIST_DIR%\dat
copy dll\softiz.bdd      %DIST_DIR%\dat
copy dll\softprim.bdd    %DIST_DIR%\dat
copy dll\softrndx.bdd    %DIST_DIR%\dat
copy dll\softrndf.bdd    %DIST_DIR%\dat
copy dll\softt.bdd       %DIST_DIR%\dat
copy dll\softtz.bdd      %DIST_DIR%\dat
copy dll\softb.bdd       %DIST_DIR%\dat
copy dll\vesa.bdd        %DIST_DIR%\dat

rem DOSIO
rem
copy dosio\dosio.h		%DIST_DIR%\dosio
copy dosio\eventq.h		%DIST_DIR%\dosio
copy dosio\keyboard.h	%DIST_DIR%\dosio
copy dosio\dos_p.h 		%DIST_DIR%\dosio
copy dosio\dos_x.h		%DIST_DIR%\dosio

copy lib\dosiobr.lib     %DIST_DIR%\lib
copy lib\dosiomr.lib     %DIST_DIR%\lib
copy lib\dosiowrr.lib    %DIST_DIR%\lib
copy lib\dosiowrs.lib    %DIST_DIR%\lib

rem HOST
rem
rem copy host\host.h		%DIST_DIR%\host
rem copy host\host_p.h		%DIST_DIR%\host

copy lib\hst4gwrr.lib    %DIST_DIR%\lib
copy lib\hst4gwrs.lib    %DIST_DIR%\lib
copy lib\hstplbr.lib     %DIST_DIR%\lib
copy lib\hstplmr.lib     %DIST_DIR%\lib
copy lib\hstplwrr.lib    %DIST_DIR%\lib
copy lib\hstplwrs.lib    %DIST_DIR%\lib
copy lib\hstppbr.lib     %DIST_DIR%\lib
copy lib\hstx3wrr.lib    %DIST_DIR%\lib
copy lib\hstx3wrs.lib    %DIST_DIR%\lib
copy lib\hstwnbr.lib     %DIST_DIR%\lib
copy lib\hstwnmr.lib     %DIST_DIR%\lib
copy lib\hstwnwrr.lib    %DIST_DIR%\lib
copy lib\hstwnwrs.lib    %DIST_DIR%\lib

