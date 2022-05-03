ifndef __FPSETUP_INC
__FPSETUP_INC equ 1
    					   
    ; Z-BUFFERED
    
    externdef c TriangleSetup_ZTIS_ARBITRARY:proc
    externdef c TriangleSetup_ZTI_ARBITRARY:proc
    externdef c TriangleSetup_ZTIS_ARBITRARY_FLAT:proc
    externdef c TriangleSetup_ZTI_ARBITRARY_FLAT:proc
    externdef c TriangleSetup_ZTIS:proc
    externdef c TriangleSetup_ZTI:proc
    externdef c TriangleSetup_ZTIS_FLAT:proc
    externdef c TriangleSetup_ZTI_FLAT:proc
    externdef c TriangleSetup_ZT_FLAT_LIT:proc
    externdef c TriangleSetup_ZT_ARBITRARY:proc
    externdef c TriangleSetup_ZT:proc
    externdef c TriangleSetup_ZI:proc
    externdef c TriangleSetup_ZI_FLAT:proc
    externdef c TriangleSetup_Z_ShadeTable:proc
    externdef c TriangleSetup_Z:proc
    
    externdef c TriangleSetup_ZPTI:proc
    externdef c TriangleSetup_ZPTI_FLAT:proc
    externdef c TriangleSetup_ZPT:proc

    externdef c TriangleSetup_ZPTI_NOCHEAT:proc
    externdef c TriangleSetup_ZPTI_FLAT_NOCHEAT:proc
    externdef c TriangleSetup_ZPT_NOCHEAT:proc

    ; NON Z-BUFFERED
    
    externdef c TriangleSetup_TIS_ARBITRARY:proc
	externdef c TriangleSetup_TI_ARBITRARY:proc
    externdef c TriangleSetup_TIS_ARBITRARY_FLAT:proc
	externdef c TriangleSetup_TI_ARBITRARY_FLAT:proc
    externdef c TriangleSetup_TIS:proc
	externdef c TriangleSetup_TI:proc
    externdef c TriangleSetup_TIS_FLAT:proc
	externdef c TriangleSetup_TI_FLAT:proc
	externdef c TriangleSetup_T_ARBITRARY:proc
	externdef c TriangleSetup_T:proc
	externdef c TriangleSetup_I:proc
	externdef c TriangleSetup_I_FLAT:proc
	externdef c TriangleSetup_ShadeTable:proc
	externdef c TriangleSetup:proc

    externdef c TriangleSetup_PTI:proc
    externdef c TriangleSetup_PTI_FLAT:proc
    externdef c TriangleSetup_PT:proc

    externdef c TriangleSetup_PTI_NOCHEAT:proc
    externdef c TriangleSetup_PTI_FLAT_NOCHEAT:proc
    externdef c TriangleSetup_PT_NOCHEAT:proc

endif
