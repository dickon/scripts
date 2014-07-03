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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "dmx.h"


int
main (int argc, char *argv)
{
  struct dmx *dmx;
  int dmx_addr;
int i;
  dmx = dmx_open (0);



for (dmx_addr=60+256;(dmx_addr+4)<=512;dmx_addr+=32)  {

for (i=1;i<=512;++i) 
  dmx_set (dmx, i, 0);

printf("%d\n",dmx_addr);

  dmx_set (dmx, dmx_addr, 255);
  dmx_set (dmx, dmx_addr+1,0);
  dmx_set (dmx, dmx_addr+2, 255);
  dmx_set (dmx, dmx_addr+3,255);
for (;;) {
      dmx_update (dmx);
	usleep(500000);
}
getchar();
}

}
