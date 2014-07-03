/*
 * dmx.h.in:
 *
 */

/*
 * Copyright (c) 2014 Citrix Systems, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*
 * $Id: dmx-head.h.in,v 1.3 2011/01/19 19:28:12 root Exp $
 */

/*
 * $Log: dmx-head.h.in,v $
 * Revision 1.3  2011/01/19 19:28:12  root
 * *** empty log message ***
 *
 * Revision 1.2  2011/01/13 17:44:27  root
 * *** empty log message ***
 *
 *
 */

/* MAKE ABSOLUTELY SURE THAT YOU ARE EDITING THE dmx-head.h.in */
/* FILE FROM WHICH THIS IS GENERATED - OTHERWISE YOUR EDITS */
/* WILL BE LOST */

#ifndef __DMX_H__
#define __DMX_H__

#ifdef __cplusplus
extern "C" { 
#endif

#include <stdio.h>

/*the integer constants here are set by configure*/

/*get uint32_t and friends defined */
#if 1
#include <stdint.h>
#elif 0
#include <sys/int_types.h>
#endif
#if 1
#include <unistd.h>
#endif

/* If the following is <> then configure failed to find where */
/* struct tm was defined - report it as a bug */

/*get struct tm defined*/
#include <time.h>

#include <usb.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

struct dmx;


/* libdmx.c */
/* version.c */
/* ftdi.c */
/* dmx.c */
struct dmx *dmx_open(int n);
void dmx_close(struct dmx *dmx);
void dmx_update(struct dmx *dmx);
void dmx_set(struct dmx *dmx, int i, int v);
#ifdef __cplusplus
}
#endif

#endif /* __DMX_H__ */
