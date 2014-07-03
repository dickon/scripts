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

static int
funge (uint8_t * val, int a, int len,int fade)
{
  static int dmx[513] = { 0 };
  int d;
  int ret = 0;

  while (len--)
    {
      if (*val > dmx[a])
        {
          if (fade) dmx[a] += fade;
          else dmx[a]=*val;
          ret = 1;
        }
      if (*val < dmx[a])
        {
          if (fade) dmx[a] -= fade;
          else dmx[a]=*val;
          ret = 1;
        }

      d = *val;

      d -= dmx[a];
      if (d < 0)
        d = -d;

      if (d < fade)
        dmx[a] = *val;

      *val = (int) dmx[a];

      a++;
      val++;
    }

  return ret;

}



static int
do_pixel_grey (XImage * img, struct dmx *dmx, int x, int y, int dmx_addr,int fade)
{
  unsigned long v = XGetPixel (img, x, y);
  uint8_t values[1];
  int ret;

  values[0] = (v & img->green_mask) >> get_shift (img->green_mask);

  ret = funge (values, dmx_addr, 1,fade);

  dmx_set (dmx, dmx_addr, values[0]);
  return ret;
}





static int
do_pixel_argb (XImage * img, struct dmx *dmx, int x, int y, int dmx_addr,int fade)
{
  unsigned long v = XGetPixel (img, x, y);
  uint8_t values[4];
  int ret;

  values[0] = 0xff;
  values[1] = (v & img->red_mask) >> get_shift (img->red_mask);
  values[2] = (v & img->green_mask) >> get_shift (img->green_mask);
  values[3] = (v & img->blue_mask) >> get_shift (img->blue_mask);

  ret = funge (values, dmx_addr, 4,fade);

  dmx_set (dmx, dmx_addr, values[0]);
  dmx_set (dmx, dmx_addr + 1, values[1]);
  dmx_set (dmx, dmx_addr + 2, values[2]);
  dmx_set (dmx, dmx_addr + 3, values[3]);

  return ret;
}


static int
do_pixel_rgb (XImage * img, struct dmx *dmx, int x, int y, int dmx_addr,int fade)
{
  unsigned long v = XGetPixel (img, x, y);
  uint8_t values[3];
  int ret;

  values[0] = (v & img->red_mask) >> get_shift (img->red_mask);
  values[1] = (v & img->green_mask) >> get_shift (img->green_mask);
  values[2] = (v & img->blue_mask) >> get_shift (img->blue_mask);

  ret = funge (values, dmx_addr, 3,fade);

  //printf("%3d: %3d %3d %3d\n",dmx_addr,values[0],values[1],values[2]);

  dmx_set (dmx, dmx_addr, values[0]);
  dmx_set (dmx, dmx_addr + 1, values[1]);
  dmx_set (dmx, dmx_addr + 2, values[2]);

  return ret;
}



#define N	6

int
main (int argc, char *argv)
{
  Screen *s;
  Window r[N];
  XImage *img;
  Display *disp[N];
  int i;
  struct dmx *dmx;

  dmx = dmx_open (0);

  for (i = 0; i < N; ++i)
    {
      char fish[32];
      sprintf (fish, ":%d", i);
      disp[i] = XOpenDisplay (fish);
      if (disp[i])
        r[i] = DefaultRootWindow (disp[i]);
      else {
	sleep(10);
	exit(0);
	}

    }

  while (1)
    {
      struct timeval tv = { 0, 0 };
      int busy = 0;
      for (i = 0; i < N; ++i)
        {
          if (!disp[i])
            continue;
          img = XGetImage (disp[i], r[i], 0, 0, 1, 1, ~0, ZPixmap);


          if (i != 4)
            {
              busy += do_pixel_rgb (img, dmx, 0, 0, 1 + (i * 30),(i==3) ? 2 : 0);
	        busy += do_pixel_argb (img, dmx, 0, 0, 256 + (i * 30),(i==3) ? 2 : 0);
            }
          else
            {
              busy += do_pixel_grey (img, dmx, 0, 0, 94,4);
            }

          XDestroyImage (img);
        }

      if (busy)
        {
          tv.tv_usec = 20000;
        }
      else
        {
          tv.tv_sec = 1;
        }

      select (0, NULL, NULL, NULL, &tv);

      dmx_update (dmx);
    }

}
