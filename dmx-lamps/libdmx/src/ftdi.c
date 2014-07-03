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
#include "ftdi.h"

struct ftdi_device
{
  struct usb_device *dev;
  struct usb_dev_handle *dh;
  int iep, oep;
};

#define TIMEOUT 10000


static uint32_t
ftdi_usb_baud_to_divisor (int baud)
{
  static const unsigned char divfrac[8] = { 0, 3, 2, 4, 1, 5, 6, 7 };
  uint32_t divisor;
  int divisor3 = 48000000 / 2 / baud; /* divisor shifted 3 bits to the left */
  divisor = divisor3 >> 3;
  divisor |= (uint32_t) divfrac[divisor3 & 0x7] << 14;
  /* Deal with special cases for highest baud rates. */
  if (divisor == 1)
    divisor = 0;
  else /* 1.0 */ if (divisor == 0x4001)
    divisor = 1;                /* 1.5 */
  return divisor;
}


INTERNAL int
ftdi_usb_set_speed (struct ftdi_device *dev)
{
  uint16_t urb_value;
  uint16_t urb_index;
  uint32_t urb_index_value;
  int rv;
  char buf[1];

  urb_index_value = ftdi_usb_baud_to_divisor (250000);
  urb_value = (uint16_t) urb_index_value;
  urb_index = (uint16_t) (urb_index_value >> 16);

  if (usb_control_msg (dev->dh,
                       FTDI_SIO_SET_BAUDRATE_REQUEST_TYPE,
                       FTDI_SIO_SET_BAUDRATE_REQUEST,
                       urb_value, urb_index, buf, 0, TIMEOUT))
    {
      fprintf (stderr, "%s error from set baud rate\n", __FUNCTION__);
    }

  return 0;
}




INTERNAL int
ftdi_usb_setup (struct ftdi_device *dev)
{
  uint16_t urb_value;
  char buf[1];

  urb_value = FTDI_SIO_SET_DATA_STOP_BITS_2 | FTDI_SIO_SET_DATA_PARITY_NONE;
  urb_value |= 8;               /* number of data bits */

  if (usb_control_msg (dev->dh,
                       FTDI_SIO_SET_DATA_REQUEST_TYPE,
                       FTDI_SIO_SET_DATA_REQUEST,
                       urb_value, 0, buf, 0, TIMEOUT))
    {
      fprintf (stderr, "%s FAILED to set databits/stopbits/parity\n",
               __FUNCTION__);
    }

  if (usb_control_msg (dev->dh,
                       FTDI_SIO_SET_FLOW_CTRL_REQUEST_TYPE,
                       FTDI_SIO_SET_FLOW_CTRL_REQUEST, 0, 0, buf, 0, TIMEOUT))
    {
      fprintf (stderr, "%s error from disable flowcontrol urb\n",
               __FUNCTION__);
    }

  ftdi_usb_set_speed (dev);

  return 0;
}


INTERNAL void
ftdi_usb_set_break (struct ftdi_device *dev, int break_state)
{
  uint16_t urb_value =
    FTDI_SIO_SET_DATA_STOP_BITS_2 | FTDI_SIO_SET_DATA_PARITY_NONE | 8;
  char buf[2];
  int err;

  if (break_state)
    urb_value |= FTDI_SIO_SET_BREAK;

  if (usb_control_msg (dev->dh,
                       FTDI_SIO_SET_DATA_REQUEST_TYPE,
                       FTDI_SIO_SET_DATA_REQUEST,
                       urb_value, 0, buf, 0, TIMEOUT))
    {
      fprintf (stderr,
               "%s FAILED to enable/disable break state (state was %d) err=%d\n",
               __FUNCTION__, break_state, err);
    }


}

INTERNAL uint16_t
ftdi_usb_get_status (struct ftdi_device *dev)
{
  int retval = 0;
  size_t count = 0;
  uint16_t buf;

  if (usb_bulk_read (dev->dh, dev->iep,
                     (void *) &buf, sizeof (buf), TIMEOUT) != sizeof (buf))
    return 0;



  return buf;
}

INTERNAL int
ftdi_usb_temt (struct ftdi_device *dev)
{
  return ftdi_usb_get_status (dev) & (FTDI_RS_TEMT << 8);
}

INTERNAL int
ftdi_usb_write (struct ftdi_device *dev, void *buf, size_t len)
{
  return usb_bulk_write (dev->dh, dev->oep, buf, len, TIMEOUT);
}

INTERNAL void
ftdi_usb_close (struct ftdi_device *d)
{
  if (!d)
    return;
  if (d->dh)
    usb_release_interface (d->dh, 0);
  if (d->dh)
    usb_close (d->dh);
  free (d);

}



static struct ftdi_device *
make_device (struct usb_device *dev)
{
  struct ftdi_device *ret;
  ret = malloc (sizeof (struct ftdi_device));
  memset (ret, 0, sizeof (ret));

  ret->dev = dev;

  do
    {

      ret->dh = usb_open (ret->dev);

      if (!ret->dh)
        break;


      if (usb_set_configuration (ret->dh, 1) < 0)
        break;

      if (usb_claim_interface (ret->dh, 0) < 0)
        break;

      ret->iep =
        ret->dev->config->interface->altsetting->endpoint[0].bEndpointAddress;
      ret->oep =
        ret->dev->config->interface->altsetting->endpoint[1].bEndpointAddress;

      fprintf (stderr, "iep=%x oep=%x\n", ret->iep, ret->oep);
      return ret;

    }
  while (0);


  ftdi_usb_close (ret);

  return NULL;
}



INTERNAL struct ftdi_device *
ftdi_usb_open (int n)
{
  static int initted = 0;
  struct usb_bus *bus;
  struct usb_device *dev;
  usb_dev_handle *dh;


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
          if (dev->descriptor.idVendor != 0x403)
            continue;
          if (dev->descriptor.idProduct != 0x6001)
            continue;
          if (n--)
            continue;

          return make_device (dev);
        }
    }

  return NULL;
}
