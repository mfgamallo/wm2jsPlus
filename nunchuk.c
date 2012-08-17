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
#include "nunchuk.h"
#include "wm2jsPlus.h"

/* Decrypt Zero-Key data from Extension-Device */
unsigned int ninty_decrypt(unsigned int encrypted) {
	unsigned int decrypted;
	decrypted = ((encrypted ^ 0x17)+17)&0xFF;
	return decrypted;
}

/* Parse Extension-Data from a NunChuk */
int wiimote_input_extnunk(wiimote *wiimote, int uinput, int dataoffset) {

	unsigned int btn_cz;
	unsigned int i,dochange;
	unsigned int current_bitmask = 0;
	unsigned int updown = 0;
	
	/* Send Joystick events to uinput */
	do_uinput(uinput,ABS_HAT0X,ninty_decrypt(wiimote->payload[dataoffset]), EV_ABS);
	do_uinput(uinput,ABS_HAT0Y,ninty_decrypt(wiimote->payload[dataoffset+1]), EV_ABS);
	
	/* Grab bitmask for C and Z */
	btn_cz = ninty_decrypt(wiimote->payload[dataoffset+5]);
	
	/* .. and cure some stupidity .. */
	current_bitmask = (((btn_cz & WIIMOTE_EXTENSION_BUTTON_NUNCHUK_Z) ? 0 : 1) + (btn_cz & WIIMOTE_EXTENSION_BUTTON_NUNCHUK_C));
	
	for(i=WIIMOTE_EXTENSION_BUTTON_NUNCHUK_Z; i<=WIIMOTE_EXTENSION_BUTTON_NUNCHUK_C; (i = i*2)) {
		dochange = 0;
		if((current_bitmask & i ) && !(wiimote->extbitmask_nunchuk & i)) {
			updown=1; dochange=1;
		}
		else if((!(current_bitmask & i )) && (wiimote->extbitmask_nunchuk & i)) {
			updown=0; dochange=1;
		}
		
		if(dochange) {
			switch(i) {
				case WIIMOTE_EXTENSION_BUTTON_NUNCHUK_Z:
					do_uinput(uinput,BTN_BASE5,updown,EV_KEY);
					break;
				case WIIMOTE_EXTENSION_BUTTON_NUNCHUK_C:
					do_uinput(uinput,BTN_BASE6,updown,EV_KEY);
					break;
				default:
					xxxdie_i("Unknown NUNCHUK button ",i);
			};
		}
	}
	
	/* Store bitmask */
	wiimote->extbitmask_nunchuk = current_bitmask;
	return TRUE;
}

