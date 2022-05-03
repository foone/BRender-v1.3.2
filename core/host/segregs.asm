;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: segregs.asm 1.1 1997/12/10 16:41:15 jon Exp $
;; $Locker: $
;;
	.386p
	.model flat

if ADD_RCS_ID
	.data
		db '$Id: segregs.asm 1.1 1997/12/10 16:41:15 jon Exp $',0
		align 4
endif
	.code

; br_uint_16 ASM_CALL HostDSQuery(void);
;
		public _HostDSQuery
_HostDSQuery proc
		mov	ax,ds
		ret
_HostDSQuery endp

; br_uint_16 ASM_CALL HostESQuery(void);
;
		public _HostESQuery
_HostESQuery proc
		mov	ax,ds
		ret
_HostESQuery endp

; br_uint_16 ASM_CALL HostCSQuery(void);
;
		public _HostCSQuery
_HostCSQuery proc
		mov	ax,cs
		ret
_HostCSQuery endp

; br_uint_16 ASM_CALL HostSSQuery(void);
;
		public _HostSSQuery
_HostSSQuery proc
		mov	ax,ss
		ret
_HostSSQuery endp

	end

