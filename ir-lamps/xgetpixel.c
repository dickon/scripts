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
#include "lirc.h"
#include <usb.h>

#include <errno.h>
#include <string.h>

void write_to_file (char *data, char *file)
{
  FILE *f;

  printf ("Writing %s to %s\n", data, file);

  f = fopen (file, "w");
  if (!f)
    {
      printf ("Failed to open %s: %s\n", file, strerror(errno));
      sleep(5);
      exit(1);
    }

  fputs(data, f);
  fclose(f);
}

void mce_reset (void)
{
  static int initted = 0;
  struct usb_bus *bus;
  struct usb_device *dev;
  struct usb_dev_handle *dh;
  int n = 0;


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
          if ((dev->descriptor.idVendor == 0x3ee && 
               dev->descriptor.idProduct == 0x2501) ||
              (dev->descriptor.idVendor == 0x471 &&
               dev->descriptor.idProduct == 0x815))
            {
              printf ("Resetting %x:%x\n",
                      dev->descriptor.idVendor,
                      dev->descriptor.idProduct);

              n++;

              dh = usb_open (dev);

              usb_reset(dh);

              usb_close(dh);
            }
        }
    }

  if (n != 2)
    {
      write_to_file ("0000:00:1d.0", "/sys/bus/pci/drivers/ehci_hcd/unbind");
      sleep (5);
      write_to_file ("0000:00:1d.0", "/sys/bus/pci/drivers/ehci_hcd/bind");
      sleep (5);
    }
}

#define NOTPULSE  LIRC_MODE2_SPACE
int
add_int (int v, lirc_t * ptr)
{
  int i, n = 0;

  for (i = 0x10; i; i >>= 1)
    {
      ptr[n++] = 500 | NOTPULSE;
      ptr[n++] = ((i & v) ? 2500 : 1950) | LIRC_MODE2_SPACE;
    }
  return n;
}

int
ir_send (int fd, int r, int g, int b)
{
  lirc_t buf[1024];
  int n = 0;

  r >>= 3;
  g >>= 3;
  b >>= 3;

  n += add_int (r, buf + n);
  n += add_int (g, buf + n);
  n += add_int (b, buf + n);

  n += add_int (r ^ 0x1f, buf + n);
  n += add_int (g ^ 0x1f, buf + n);
  n += add_int (b ^ 0x1f, buf + n);

  buf[n++] = 450 | NOTPULSE;
//buf[n++]=100000|LIRC_MODE2_SPACE;

  write (fd, buf, n * sizeof (lirc_t));

#if 0
  {
    int i;
    for (i = 0; i < n; i += 2)
      {
        write (fd, &buf[i], sizeof (lirc_t) * 2);
      }
  }
#endif

  usleep (50000);

}

void
send_rgb (char *path,int m, int r, int g, int b)
{
  int fd;
  unsigned int u;

  printf ("Detected change sending %d %d %d to %x on %s\n",  r, g, b,m,path);

  fd = open (path, O_RDWR);

  u = m;
  errno=0;
  printf ("set_mask(%x)=%d (%d)\n",u,ioctl (fd, LIRC_SET_TRANSMITTER_MASK, &u),errno);

  u = 40000;
  ioctl (fd, LIRC_SET_SEND_CARRIER, &u);
  ir_send (fd, r, g, b);
  usleep (50000);
  ir_send (fd, r, g, b);
  usleep (100000);
  ir_send (fd, r, g, b);
  close (fd);

  usleep(100000);

}


static int
get_shift (int i)
{
  int ret = 0;
  while (i)
    {
      if (i & 1)
        return ret;
      ret++;
      i >>= 1;
    }

  return 0;
}



#define N	4

int
main (int argc, char *argv)
{
  Screen *s;
  Window r[N];
  unsigned long values[N];
  XImage *img;
  Display *disp[N];
  int i;
  int ticks = 0;
  int last_reset=0;

  setlinebuf(stdout);
  setlinebuf(stderr);

  for (i = 0; i < N; ++i)
    {
      char fish[32];
      sprintf (fish, ":%d", i+1 );
      disp[i] = XOpenDisplay (fish);
      if (!disp[i]) {
		sleep(5);
		exit(1);
	}

      if (disp[i])
        r[i] = DefaultRootWindow (disp[i]);
    }

  while (1)
    {
      struct timeval tv = { 0, 0 };

      for (i = 0; i < N; ++i)
        {
          unsigned long v;

          if (!disp[i])
            continue;

          img = XGetImage (disp[i], r[i], 0, 0, 1, 1, ~0, ZPixmap);

          v = XGetPixel (img, 0, 0);

#if 0
          printf ("read %x have %x ticks=%d\n", v, values[i], ticks);
#endif

          if ((values[i] != v) || (!ticks))
            {
              int r, g, b;
              r = (v & img->red_mask) >> get_shift (img->red_mask);
              g = (v & img->green_mask) >> get_shift (img->green_mask);
              b = (v & img->blue_mask) >> get_shift (img->blue_mask);

	      if (last_reset>=100) {
			last_reset=0;
			mce_reset();
			sleep(1);
	      }
        last_reset++;

	switch (i) {
	case 0:
	case 1:
	case 2:
              send_rgb ("/dev/lirc_north",1 << i, r, g, b);
		break;
	case 3:
              send_rgb ("/dev/lirc_south",0x7, r, g, b);
		break;
		}
              values[i] = v;

            }


          XDestroyImage (img);
        }

      tv.tv_sec = 1;

      if (ticks++ >= 120)
        ticks = 0;


      select (0, NULL, NULL, NULL, &tv);

    }

}
