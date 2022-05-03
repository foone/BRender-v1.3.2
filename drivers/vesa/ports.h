/*
 * Copyright (c) 1992,1993-1996 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: ports.h 1.1 1997/12/10 16:54:25 jon Exp $
 * $Locker: $
 */
#ifndef _PORTS_H_
#define _PORTS_H_

int BR_ASM_CALL inp(unsigned short);
unsigned short BR_ASM_CALL inpw(unsigned short);
unsigned long BR_ASM_CALL inpd(unsigned short);

int BR_ASM_CALL outp(unsigned short, int);
unsigned short BR_ASM_CALL outpw(unsigned short, unsigned short);
unsigned long BR_ASM_CALL outpd(unsigned short, unsigned long);

#endif
