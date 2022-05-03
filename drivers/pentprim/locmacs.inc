; Copyright (c) 1992,1993-1996 Argonaut Technologies Limited. All rights reserved.
;
; $Id: locmacs.inc 1.1 1997/12/10 16:47:09 jon Exp $
; $Locker: $
;
; Some rahter cheesy macros for generating stack frames without using EBP
;

LOCALS_BEGIN	macro
_local_size = 0
_local_offset = 0
_param_size = 0
_param_offset = 4
				endm

PARAM_DWORD		macro	label
			local this_offset
this_offset equ _param_size
label		equ		dword ptr (this_offset+_param_offset)[esp]
_param_size = _param_size + 4
				endm

PARAM_DWORD_ALIAS		macro	label
			local this_offset
this_offset equ _param_size
label		equ		dword ptr (this_offset+_param_offset)[esp]
				endm

LOCAL_DWORD		macro	label
			local this_offset
this_offset equ _local_size
label		equ		dword ptr (this_offset+_local_offset)[esp]
_local_size = _local_size + 4
				endm

LOCAL_DWORD_ALIAS		macro	label
			local this_offset
this_offset equ _local_size
label		equ		dword ptr (this_offset+_local_offset)[esp]
				endm

LOCAL_QWORD		macro	label
			local this_offset
this_offset equ _local_size
label		equ		qword ptr (this_offset+_local_offset)[esp]
_local_size = _local_size + 8
				endm

LOCAL_QWORD_ALIAS		macro	label
			local this_offset
this_offset equ _local_size
label		equ		qword ptr (this_offset+_local_offset)[esp]
				endm

LOCALS_END		macro
				endm


LOCALS_CREATE	macro
			sub	esp,_local_size
_param_offset = _param_offset + _local_size
				endm

LOCALS_DESTROY	macro
			add	esp,_local_size
				endm

LADJUST			macro c
_local_offset = _local_offset + c
_param_offset = _param_offset + c
				endm

LPUSH			macro reg
				push	reg
				LADJUST	4
				endm

LPOP			macro reg
				pop		reg
				LADJUST	4
				endm

