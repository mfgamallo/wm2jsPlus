/*
 *  Hack on the original wm2js from https://trac.v2.nl/browser/andres/wm2js/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "uinput.h"
#include "wiimote.h"
#include "wm2jsPlus.h"
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* Create uinput output */
int do_uinput(int fd, unsigned short key, int pressed, unsigned short event_type) {
	struct uinput_event event;
	memset(&event, 0 , sizeof(event));
	
	event.type = event_type;
	event.code = key;
	event.value = pressed;
	
	if(write(fd,&event,sizeof(event)) != sizeof(event))
		xxxdie("Writing event to uinput driver failed ; Aborting");
	return TRUE;
}


/* Init uinput */
int init_uinput_device(void) {
	struct uinput_dev dev;
	int fd = -1;
	
	fd = open("/dev/uinput", O_RDWR);
	if(fd < 0)
		fd = open("/dev/misc/uinput", O_RDWR);

	if(fd < 0) {
		xxxwarn("Unable to open uinput device ; hint: 'modprobe uinput' ?!");
		return -1;
	}
	
	memset(&dev,0,sizeof(dev));
	strncpy(dev.name, "WiiMote", UINPUT_MAX_NAME_SIZE);
	dev.idbus     = 0;
	dev.idvendor  = WIIMOTE_VENDOR;
	dev.idproduct = WIIMOTE_PRODUCT;
	dev.idversion = 0x01;

	if(write(fd, &dev, sizeof(dev)) < sizeof(dev)) {
		xxxwarn("Registering device at uinput failed");
		return -1;
	}
	
	ioctl(fd, UI_SET_EVBIT, EV_ABS)        && xxxdie("Can't set EV_ABS"); /* Joydev */
	ioctl(fd, UI_SET_EVBIT, EV_KEY)        && xxxdie("Can't set EV_KEY"); /* Keys */

	ioctl(fd, UI_SET_ABSBIT, ABS_X)        && xxxdie("Can't set ABS_X");  /* + */
	ioctl(fd, UI_SET_ABSBIT, ABS_Y)        && xxxdie("Can't set ABS_Y");
	
	ioctl(fd, UI_SET_ABSBIT, ABS_RX)       && xxxdie("Can't set ABS_RX"); /* Motion sensor */
	ioctl(fd, UI_SET_ABSBIT, ABS_RY)       && xxxdie("Can't set ABS_RY");
	ioctl(fd, UI_SET_ABSBIT, ABS_RZ)       && xxxdie("Can't set ABS_RZ");

	ioctl(fd, UI_SET_ABSBIT, ABS_HAT0X)       && xxxdie("Can't set ABS_RZ"); /* Nunchuck joystick */
	ioctl(fd, UI_SET_ABSBIT, ABS_HAT0Y)       && xxxdie("Can't set ABS_RZ");

	ioctl(fd, UI_SET_KEYBIT, BTN_TOP2)     && xxxdie("Can't set BTN_TOP2");
	ioctl(fd, UI_SET_KEYBIT, BTN_BASE)     && xxxdie("Can't set BTN_BASE");
	ioctl(fd, UI_SET_KEYBIT, BTN_BASE2)    && xxxdie("Can't set BTN_BASE2");
	ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER)  && xxxdie("Can't set BTN_TRIGGER");
	ioctl(fd, UI_SET_KEYBIT, BTN_THUMB)    && xxxdie("Can't set BTN_THUMB");
	ioctl(fd, UI_SET_KEYBIT, BTN_TOP)      && xxxdie("Can't set BTN_TOP");
	ioctl(fd, UI_SET_KEYBIT, BTN_BASE3)    && xxxdie("Can't set BTN_BASE3");
	ioctl(fd, UI_SET_KEYBIT, BTN_BASE5)    && xxxdie("Can't set BTN_BASE5");
	ioctl(fd, UI_SET_KEYBIT, BTN_BASE6)    && xxxdie("Can't set BTN_BASE6");

	/* Classic controller */

	ioctl(fd, UI_SET_ABSBIT, ABS_LAX)          && xxxdie("Can't set ABS_LAX");  /* Left analog */
	ioctl(fd, UI_SET_ABSBIT, ABS_LAY)          && xxxdie("Can't set ABS_HAT1Y");
	ioctl(fd, UI_SET_ABSBIT, ABS_RAX)          && xxxdie("Can't set ABS_HAT2X");  /* Right analog */
	ioctl(fd, UI_SET_ABSBIT, ABS_RAY)          && xxxdie("Can't set ABS_HAT2Y");
	ioctl(fd, UI_SET_ABSBIT, ABS_LS)           && xxxdie("Can't set ABS_HAT3X");  /* Left shoulder */
	ioctl(fd, UI_SET_ABSBIT, ABS_RS)           && xxxdie("Can't set ABS_HAT3Y");  /* Right shoulder */

	ioctl(fd, UI_SET_KEYBIT, BTN_FORWARD)        && xxxdie("Can't set BTN_FORWARD"); /* pad up */
	ioctl(fd, UI_SET_KEYBIT, BTN_BACK)           && xxxdie("Can't set BTN_BACK"); /* pad down */
	ioctl(fd, UI_SET_KEYBIT, BTN_LEFT)           && xxxdie("Can't set BTN_LEFT"); /* pad left */
	ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT)          && xxxdie("Can't set BTN_RIGHT"); /* pad right */
	ioctl(fd, UI_SET_KEYBIT, BTN_SELECT)         && xxxdie("Can't set BTN_SELECT");
	ioctl(fd, UI_SET_KEYBIT, BTN_START)          && xxxdie("Can't set BTN_START");
	ioctl(fd, UI_SET_KEYBIT, BTN_MODE)           && xxxdie("Can't set BTN_MODE"); /* home */
	ioctl(fd, UI_SET_KEYBIT, BTN_A)              && xxxdie("Can't set BTN_A");
	ioctl(fd, UI_SET_KEYBIT, BTN_B)              && xxxdie("Can't set BTN_B");
	ioctl(fd, UI_SET_KEYBIT, BTN_X)              && xxxdie("Can't set BTN_X");
	ioctl(fd, UI_SET_KEYBIT, BTN_Y)              && xxxdie("Can't set BTN_Y");
	ioctl(fd, UI_SET_KEYBIT, BTN_TL)             && xxxdie("Can't set BTN_TL"); /* L */
	ioctl(fd, UI_SET_KEYBIT, BTN_TR)             && xxxdie("Can't set BTN_TR"); /* R */
	ioctl(fd, UI_SET_KEYBIT, BTN_TL2)            && xxxdie("Can't set BTN_TL2"); /* ZL */
	ioctl(fd, UI_SET_KEYBIT, BTN_TR2)            && xxxdie("Can't set BTN_TR2"); /* ZR */

	ioctl(fd, UI_DEV_CREATE)               && xxxdie_i("Unable to create device on fd ",fd);
	return fd;
}
