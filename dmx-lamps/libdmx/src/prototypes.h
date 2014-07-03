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

/* libdmx.c */
/* version.c */
char *libdmx_get_version(void);
/* ftdi.c */
int ftdi_usb_set_speed(struct ftdi_device *dev);
int ftdi_usb_setup(struct ftdi_device *dev);
void ftdi_usb_set_break(struct ftdi_device *dev, int break_state);
uint16_t ftdi_usb_get_status(struct ftdi_device *dev);
int ftdi_usb_temt(struct ftdi_device *dev);
int ftdi_usb_write(struct ftdi_device *dev, void *buf, size_t len);
void ftdi_usb_close(struct ftdi_device *d);
struct ftdi_device *ftdi_usb_open(int n);
/* dmx.c */
struct dmx *dmx_open(int n);
void dmx_close(struct dmx *dmx);
void dmx_update(struct dmx *dmx);
void dmx_set(struct dmx *dmx, int i, int v);
