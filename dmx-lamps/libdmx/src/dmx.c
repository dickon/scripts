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

#include "project.h"

struct dmx
{
  struct ftdi_device *ftdi;
  uint8_t universe[513];
};


EXTERNAL struct dmx *
dmx_open (int n)
{
  struct dmx *dmx = malloc (sizeof (struct dmx));

  dmx->ftdi = ftdi_usb_open (n);
  if (!dmx->ftdi)
    {
      free (dmx);
      return NULL;
    }

  ftdi_usb_setup (dmx->ftdi);

  memset (dmx->universe, 0, sizeof (dmx->universe));

  return dmx;
}

EXTERNAL void
dmx_close (struct dmx *dmx)
{
  if (!dmx)
    return;
  ftdi_usb_close (dmx->ftdi);
  free (dmx);
}

EXTERNAL void
dmx_update (struct dmx *dmx)
{
  while (!ftdi_usb_temt (dmx->ftdi));

  ftdi_usb_set_break (dmx->ftdi, 1);
  ftdi_usb_set_break (dmx->ftdi, 0);

  ftdi_usb_write (dmx->ftdi, dmx->universe, sizeof (dmx->universe));
}

EXTERNAL void
dmx_set (struct dmx *dmx, int i, int v)
{
  dmx->universe[i] = v;
}
