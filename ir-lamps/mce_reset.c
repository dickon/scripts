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
#include <usb.h>

#if 0

      if (usb_set_configuration (ret->dh, 1) < 0)
        break;

      if (usb_claim_interface (ret->dh, 0) < 0)
        break;

#endif




int mce_reset (int n)
{
  static int initted = 0;
  struct usb_bus *bus;
  struct usb_device *dev;
  struct usb_dev_handle *dh;


  if (!initted)
    {
      usb_init ();
      usb_find_busses ();
      usb_find_devices ();
    }



  for (bus = usb_get_busses (); bus; bus = bus->next)
    {
      for (dev = bus->devices; dev; dev = dev->next)
        {
          if (dev->descriptor.idVendor != 0x3ee)
            continue;
          if (dev->descriptor.idProduct != 0x2501)
            continue;
          if (n--)
            continue;

printf("Resetting\n");

     dh = usb_open (dev);

	usb_reset(dh);

	usb_close(dh);
        }
    }

  return NULL;
}

int main(int argc,char *argv[])
{
mce_reset(0);
}
