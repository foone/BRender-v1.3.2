;; Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: vertex.inc 1.1 1997/12/10 16:53:14 jon Exp $
;; $Locker: $
;;
;; Vertex structure sizes
;;

BRP_VERTEX_SHIFT	equ 6

if EVAL
FMT_VERTEX_SHIFT	equ 11
else
FMT_VERTEX_SHIFT	equ 5
endif

BRP_VERTEX_SIZE		equ 1 shl BRP_VERTEX_SHIFT
FMT_VERTEX_SIZE 	equ 1 shl FMT_VERTEX_SHIFT

SHOW		macro x,y
		%out x = y
		endm


	if (sizeof brp_vertex) NE BRP_VERTEX_SIZE
	.err <sizeof(brp_vertex) is assumed to be 2^BRP_VERTEX_SHIFT bytes>
	SHOW	<Size of brp_vertex     >,<%(sizeof brp_vertex)>
	endif

	if (sizeof fmt_vertex) NE FMT_VERTEX_SIZE
	.err <sizeof(fmt_vertex) is assumed to be 2^FMT_VERTEX_SHIFT bytes>
	SHOW	<Size of fmt_vertex     >,<%(sizeof fmt_vertex)>
	endif

