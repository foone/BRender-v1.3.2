set DIST_DIR=%1

set SRC_REL=e
set DST_REL=r

set SRC_DRV=bed

mkdir %DIST_DIR%\lib
mkdir %DIST_DIR%\inc
mkdir %DIST_DIR%\dat
mkdir %DIST_DIR%\dosio

rem Core Libraries
rem

copy lib\brdbbf%SRC_REL%.lib     %DIST_DIR%\lib\brdbbf%DST_REL%.lib 
copy lib\brdbbx%SRC_REL%.lib     %DIST_DIR%\lib\brdbbx%DST_REL%.lib 
copy lib\brdbmf%SRC_REL%.lib     %DIST_DIR%\lib\brdbmf%DST_REL%.lib 
copy lib\brdbmx%SRC_REL%.lib     %DIST_DIR%\lib\brdbmx%DST_REL%.lib 
copy lib\brdbwf%SRC_REL%r.lib    %DIST_DIR%\lib\brdbwf%DST_REL%r.lib
copy lib\brdbwf%SRC_REL%s.lib    %DIST_DIR%\lib\brdbwf%DST_REL%s.lib
copy lib\brdbwx%SRC_REL%r.lib    %DIST_DIR%\lib\brdbwx%DST_REL%r.lib
copy lib\brdbwx%SRC_REL%s.lib    %DIST_DIR%\lib\brdbwx%DST_REL%s.lib

copy lib\brfmbf%SRC_REL%.lib     %DIST_DIR%\lib\brfmbf%DST_REL%.lib 
copy lib\brfmbx%SRC_REL%.lib     %DIST_DIR%\lib\brfmbx%DST_REL%.lib 
copy lib\brfmmf%SRC_REL%.lib     %DIST_DIR%\lib\brfmmf%DST_REL%.lib 
copy lib\brfmmx%SRC_REL%.lib     %DIST_DIR%\lib\brfmmx%DST_REL%.lib 
copy lib\brfmwf%SRC_REL%r.lib    %DIST_DIR%\lib\brfmwf%DST_REL%r.lib
copy lib\brfmwf%SRC_REL%s.lib    %DIST_DIR%\lib\brfmwf%DST_REL%s.lib
copy lib\brfmwx%SRC_REL%r.lib    %DIST_DIR%\lib\brfmwx%DST_REL%r.lib
copy lib\brfmwx%SRC_REL%s.lib    %DIST_DIR%\lib\brfmwx%DST_REL%s.lib

copy lib\brmtbf%SRC_REL%.lib     %DIST_DIR%\lib\brmtbf%DST_REL%.lib 
copy lib\brmtbx%SRC_REL%.lib     %DIST_DIR%\lib\brmtbx%DST_REL%.lib 
copy lib\brmtmf%SRC_REL%.lib     %DIST_DIR%\lib\brmtmf%DST_REL%.lib 
copy lib\brmtmx%SRC_REL%.lib     %DIST_DIR%\lib\brmtmx%DST_REL%.lib 
copy lib\brmtwf%SRC_REL%r.lib    %DIST_DIR%\lib\brmtwf%DST_REL%r.lib
copy lib\brmtwf%SRC_REL%s.lib    %DIST_DIR%\lib\brmtwf%DST_REL%s.lib
copy lib\brmtwx%SRC_REL%r.lib    %DIST_DIR%\lib\brmtwx%DST_REL%r.lib
copy lib\brmtwx%SRC_REL%s.lib    %DIST_DIR%\lib\brmtwx%DST_REL%s.lib

copy lib\brfwb%SRC_REL%.lib      %DIST_DIR%\lib\brfwb%DST_REL%.lib 
copy lib\brfwb%SRC_REL%.lib      %DIST_DIR%\lib\brfwbf%DST_REL%.lib 
copy lib\brfwb%SRC_REL%.lib      %DIST_DIR%\lib\brfwbx%DST_REL%.lib 

copy lib\brfwm%SRC_REL%.lib      %DIST_DIR%\lib\brfwm%DST_REL%.lib 
copy lib\brfwm%SRC_REL%.lib      %DIST_DIR%\lib\brfwmf%DST_REL%.lib 
copy lib\brfwm%SRC_REL%.lib      %DIST_DIR%\lib\brfwmx%DST_REL%.lib 

copy lib\brfww%SRC_REL%r.lib     %DIST_DIR%\lib\brfww%DST_REL%r.lib
copy lib\brfww%SRC_REL%r.lib     %DIST_DIR%\lib\brfwwf%DST_REL%r.lib
copy lib\brfww%SRC_REL%r.lib     %DIST_DIR%\lib\brfwwx%DST_REL%r.lib

copy lib\brfww%SRC_REL%s.lib     %DIST_DIR%\lib\brfww%DST_REL%s.lib
copy lib\brfww%SRC_REL%s.lib     %DIST_DIR%\lib\brfwwf%DST_REL%s.lib
copy lib\brfww%SRC_REL%s.lib     %DIST_DIR%\lib\brfwwx%DST_REL%s.lib

copy lib\brpmb%SRC_REL%.lib      %DIST_DIR%\lib\brpmb%DST_REL%.lib  
copy lib\brpmm%SRC_REL%.lib      %DIST_DIR%\lib\brpmm%DST_REL%.lib  
copy lib\brpmw%SRC_REL%r.lib     %DIST_DIR%\lib\brpmw%DST_REL%r.lib 
copy lib\brpmw%SRC_REL%s.lib     %DIST_DIR%\lib\brpmw%DST_REL%s.lib 

copy lib\brstb%SRC_REL%.lib      %DIST_DIR%\lib\brstb%DST_REL%.lib  
copy lib\brstm%SRC_REL%.lib      %DIST_DIR%\lib\brstm%DST_REL%.lib  
copy lib\brstw%SRC_REL%r.lib     %DIST_DIR%\lib\brstw%DST_REL%r.lib 
copy lib\brstw%SRC_REL%s.lib     %DIST_DIR%\lib\brstw%DST_REL%s.lib 

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
copy inc\brv1db.h        %DIST_DIR%\inc
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
copy inc\dmyifix.h       %DIST_DIR%\inc\mscifix.h
copy inc\dmyifix.h       %DIST_DIR%\inc\proifix.h
copy inc\dmyifix.h       %DIST_DIR%\inc\g386ifix.h

rem Drivers
rem

copy dll\mcga.bdd        %DIST_DIR%\dat
copy dll\vesa.bdd        %DIST_DIR%\dat

copy dll\softprim.bdd    %DIST_DIR%\dat
copy dll\softh.bdd       %DIST_DIR%\dat
copy dll\softhz.bdd      %DIST_DIR%\dat
copy dll\softi.bdd       %DIST_DIR%\dat
copy dll\softiz.bdd      %DIST_DIR%\dat
copy dll\softt.bdd       %DIST_DIR%\dat
copy dll\softtz.bdd      %DIST_DIR%\dat
copy dll\softb.bdd       %DIST_DIR%\dat

copy dll\softrndx.%SRC_DRV% %DIST_DIR%\dat
copy dll\softrndf.%SRC_DRV% %DIST_DIR%\dat

rem DOSIO
rem
copy dosio\dosio.h		%DIST_DIR%\dosio
copy dosio\eventq.h		%DIST_DIR%\dosio
copy dosio\keyboard.h	%DIST_DIR%\dosio
copy dosio\dos_p.h 		%DIST_DIR%\dosio
copy dosio\dos_x.h		%DIST_DIR%\dosio

copy lib\dosiob%SRC_REL%.lib     %DIST_DIR%\lib\dosiob%DST_REL%.lib 
copy lib\dosiom%SRC_REL%.lib     %DIST_DIR%\lib\dosiom%DST_REL%.lib 
copy lib\dosiow%SRC_REL%r.lib    %DIST_DIR%\lib\dosiow%DST_REL%r.lib
copy lib\dosiow%SRC_REL%s.lib    %DIST_DIR%\lib\dosiow%DST_REL%s.lib

rem HOST
rem
rem copy host\host.h		%DIST_DIR%\host
rem copy host\host_p.h		%DIST_DIR%\host

copy lib\hst4gw%SRC_REL%r.lib    %DIST_DIR%\lib\hst4gw%DST_REL%r.lib
copy lib\hst4gw%SRC_REL%s.lib    %DIST_DIR%\lib\hst4gw%DST_REL%s.lib
copy lib\hstplb%SRC_REL%.lib     %DIST_DIR%\lib\hstplb%DST_REL%.lib 
copy lib\hstplm%SRC_REL%.lib     %DIST_DIR%\lib\hstplm%DST_REL%.lib 
copy lib\hstplw%SRC_REL%r.lib    %DIST_DIR%\lib\hstplw%DST_REL%r.lib
copy lib\hstplw%SRC_REL%s.lib    %DIST_DIR%\lib\hstplw%DST_REL%s.lib
copy lib\hstppb%SRC_REL%.lib     %DIST_DIR%\lib\hstppb%DST_REL%.lib 
copy lib\hstx3w%SRC_REL%r.lib    %DIST_DIR%\lib\hstx3w%DST_REL%r.lib
copy lib\hstx3w%SRC_REL%s.lib    %DIST_DIR%\lib\hstx3w%DST_REL%s.lib
copy lib\hstwnb%SRC_REL%.lib     %DIST_DIR%\lib\hstwnb%DST_REL%.lib 
copy lib\hstwnm%SRC_REL%.lib     %DIST_DIR%\lib\hstwnm%DST_REL%.lib 
copy lib\hstwnw%SRC_REL%r.lib    %DIST_DIR%\lib\hstwnw%DST_REL%r.lib
copy lib\hstwnw%SRC_REL%s.lib    %DIST_DIR%\lib\hstwnw%DST_REL%s.lib

