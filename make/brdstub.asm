; Stub for brender driver files
;
	.model	small
	
	.code
	mov	ax,cs
	mov	ds,ax

	mov	ah,40h
	xor	bx,bx
	mov	cx,message_end-message
	mov	dx,offset message
	int	21h
	
	mov	ax,4c00h
	int	21h

message		db	'This is a BRender driver',13,10
message_end	label byte

	end
