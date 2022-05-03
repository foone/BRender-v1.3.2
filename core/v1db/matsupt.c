/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: matsupt.c 1.2 1998/03/05 20:24:12 jon Exp $
 * $Locker: $
 *
 * Material support routines 
 */
#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"
#include "datafile.h"

BR_RCS_ID("$Id: matsupt.c 1.2 1998/03/05 20:24:12 jon Exp $")

/*
 * Allocate a material
 */
br_material * BR_PUBLIC_ENTRY BrMaterialAllocate(char *name)
{
	br_material *m;

	m = BrResAllocate(v1db.res,sizeof(*m),BR_MEMORY_MATERIAL);

	*m = *v1db.default_material;
	m->stored = NULL;

	if(name)
		m->identifier = BrResStrDup(m, name);
	else
		m->identifier = NULL;

	return m;
}

/*
 * Free a material
 */
void BR_PUBLIC_ENTRY BrMaterialFree(br_material *m)
{
	UASSERT(m != NULL);

	BrResFree(m);
}


