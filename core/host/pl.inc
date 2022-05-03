;;
;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: pl.inc 1.1 1997/12/10 16:41:14 jon Exp $
;; $Locker: $
;;

; Structure used to pass register values to real mode interrupt
; via Pharlap

pl_registers	struc
interrupt	word	?
rds		word	?
res		word	?
rfs		word	?
rgs		word	?
reax		dword	?
redx		dword	?
pl_registers	ends

