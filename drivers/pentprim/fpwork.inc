; Layout of workspace for floating point Pentium optimized setup for rasterisers
;

@tag_0		UNION
l		DWORD		?
s		SWORD		2t DUP (?)
@tag_0		ENDS

_workspace struc 8
;qwords start here
	xm		DWORD		?
	d_xm		DWORD		?
	x1		DWORD		?
	d_x1		DWORD		?
	x2		DWORD		?
	d_x2		DWORD		?

	s_z dword ?
	d_z_y_1 dword ?
	d_z_x dword ?
	d_z_y_0 dword ?

	s_i dword ?
	d_i_y_1 dword ?
	d_i_x dword ?
	d_i_y_0 dword ?

	s_u dword ?
	d_u_y_1 dword ?
	d_u_x dword ?
	d_u_y_0 dword ?

	s_v dword ?
	d_v_y_1 dword ?
	d_v_x dword ?
	d_v_y_0 dword ?

        s_s dword ?
        d_s_y_1 dword ?
        d_s_x dword ?
        d_s_y_0 dword ?

	scanAddress dword ?
	scanAddressTrashed dword ?
	depthAddress dword ?
	depthAddressTrashed dword ?

;qwords end here


;input to setup code
	v0 dword ?
	v1 dword ?
	v2 dword ?

;these are only used within the setup code
	iarea dword ?
	dx1_a dword ?
	dx2_a dword ?
	dy1_a dword ?
	dy2_a dword ?
	xstep_1 dword ?
	xstep_0 dword ?
	t_dx dword ?
	t_dy dword ?

;results from the setup code
	t_y dword ?
	flip dword ?

;used in some rasterisers
	c_z dword ?
	c_u dword ?
	c_v dword ?
	c_i dword ?

;tsbHeader stuff aka h	
	counts		@tag_0		<>


; Fixed setup only
	v0_x dword ? ;converted vertex coordinates - maps to three overlapping
	v1_x dword ? ; 'converted_vertex' structures
	v2_x dword ?
	v0_y dword ?
	v1_y dword ?
	v2_y dword ?
	top_vertex dword ?


;my stuff, currently in here for my convenience -JohnG
	xm_f dword ?
	d_xm_f dword ?

	topCount dword ?
	bottomCount dword ?

align 8
	colour dword ?
	colourExtension dword ?
	shadeTable dword ?
align 8
	scratch0 dword ?
	scratch1 dword ?
	scratch2 dword ?
	scratch3 dword ?
	scratch4 dword ?
	scratch5 dword ?
	scratch6 dword ?
	scratch7 dword ?
	scratch8 dword ?
	scratch9 dword ?
	scratch10 dword ?
	scratch11 dword ?
	scratch12 dword ?
	scratch13 dword ?
	scratch14 dword ?
	scratch15 dword ?
_workspace ends

	m_y	equ [workspace.t_dy]
	sort_value equ [workspace.top_vertex]

ArbitraryWidthWorkspace struc 8
    su dword ?
	dword ?

	dux dword ?
    dword ?

	duy1 dword ?
    dword ?

	duy0 dword ?
	dword ?
    
	sv dword ?
    dword ?

    dvy1 dword ?
	dword ?
	dvy1c dword ?
	dword ?
    dvy0 dword ?
	dword ?
	dvy0c dword ?

	dvxc dword ?
	dword ?
	dvx dword ?
	dword ?

	svf dword ?
	dvxf dword ?
    dvy1f dword ?
    dvy0f dword ?

	uUpperBound dword ?
	vUpperBound dword ?

	flags dword ?
	retAddress dword ?
ArbitraryWidthWorkspace ends



externdef SYSCALL workspace:_workspace
externdef SYSCALL workspaceA:ArbitraryWidthWorkspace
