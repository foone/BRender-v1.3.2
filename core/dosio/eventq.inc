;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: eventq.inc 1.1 1997/12/10 16:40:59 jon Exp $
;; $Locker: $
;;
;; Structures and equates coresponding to eventq.h
;;

; Structure corresponding to dosio_event
;
event	struc
etype		word	?	
qualifiers	word	?	
value_1		dword	?	
value_2		dword	?	
event	ends

; Structure corresponding to dosio_event_queue
;
eventq	struc
total		word	?	
count		word	?	
head		word	?	
tail		word	?
slots		word	?	
eventq	ends

DOSIO_QUAL_SHIFT		equ	1
DOSIO_QUAL_CONTROL		equ	2
DOSIO_QUAL_ALT			equ	4

DOSIO_EVENT_KEY_DOWN		equ	0
DOSIO_EVENT_KEY_UP		equ	1
DOSIO_EVENT_POINTER1_DOWN	equ	2
DOSIO_EVENT_POINTER1_UP		equ	3
DOSIO_EVENT_POINTER2_DOWN	equ	4
DOSIO_EVENT_POINTER2_UP		equ	5
DOSIO_EVENT_POINTER3_DOWN	equ	6
DOSIO_EVENT_POINTER3_UP		equ	7
DOSIO_EVENT_POINTER_MOVE	equ	8

