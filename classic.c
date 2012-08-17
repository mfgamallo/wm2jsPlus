/*
 *  Copyright (C) 2012  Manuel Gamallo <manuf_81@mixmail.com>
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
#include "classic.h"
#include "wm2jsPlus.h"

/* Return data for given key */
int decode_data(wiimote *wiimote, int dataoffset, unsigned short key) {
  short int value = 0;
  switch(key) {
  case WIIMOTE_CLASSIC_LEFT_ANALOG_X:
    value = ((wiimote->payload[dataoffset] - 33) & 0x3F) << 10;
    break;

  case WIIMOTE_CLASSIC_LEFT_ANALOG_Y:
    value = (30 - (wiimote->payload[dataoffset+1] & 0x3F)) << 10;
    break;

  case WIIMOTE_CLASSIC_RIGHT_ANALOG_X:
    value = ((((wiimote->payload[dataoffset+2] & 0x80) >> 7) |
            ((wiimote->payload[dataoffset+1] & 0xC0) >> 5) |
	     ((wiimote->payload[dataoffset] & 0xC0) >> 3))
	     - 15) << 11;
    break;

  case WIIMOTE_CLASSIC_RIGHT_ANALOG_Y:
    value = (15 - (wiimote->payload[dataoffset+2] & 0x1F)) << 11;
    break;

  case WIIMOTE_CLASSIC_L_ANALOG:
    value = ((((wiimote->payload[dataoffset+3] & 0xE0) >> 5) |
	      ((wiimote->payload[dataoffset+2] & 0x60) >> 2))
	     -15 ) << 11;
    break;
  case WIIMOTE_CLASSIC_R_ANALOG:
    value = ((wiimote->payload[dataoffset+3] & 0x1F) -15 ) << 11;
    break;
  }
  return value;
}

/* Parse Extension-Data from a NunChuk */
int wiimote_input_ext_classic(wiimote *wiimote, int uinput, int dataoffset) {

       /*printf("%02X ", wiimote->payload[dataoffset]);
         printf("%02X ", wiimote->payload[dataoffset+1]);
	 printf("%02X ", wiimote->payload[dataoffset+2]);
	 printf("%02X ", wiimote->payload[dataoffset+3]);
	 printf("%02X ", wiimote->payload[dataoffset+4]);
	 printf("%02X\n", wiimote->payload[dataoffset+5]);*/

	unsigned int i, dochange;
	unsigned int current_bitmask = 0;
	unsigned int updown = 0;

	/* Send Joystick events to uinput */
	do_uinput(uinput,ABS_LAX, decode_data(wiimote, dataoffset, WIIMOTE_CLASSIC_LEFT_ANALOG_X), EV_ABS);
        do_uinput(uinput,ABS_LAY, decode_data(wiimote, dataoffset, WIIMOTE_CLASSIC_LEFT_ANALOG_Y), EV_ABS);
	do_uinput(uinput,ABS_RAX, decode_data(wiimote, dataoffset, WIIMOTE_CLASSIC_RIGHT_ANALOG_X), EV_ABS);
	do_uinput(uinput,ABS_RAY, decode_data(wiimote, dataoffset, WIIMOTE_CLASSIC_RIGHT_ANALOG_Y), EV_ABS);
	do_uinput(uinput,ABS_LS, decode_data(wiimote, dataoffset, WIIMOTE_CLASSIC_L_ANALOG), EV_ABS);
	do_uinput(uinput,ABS_RS, decode_data(wiimote, dataoffset, WIIMOTE_CLASSIC_R_ANALOG), EV_ABS);

	do_uinput(uinput, SYN_REPORT, 0, EV_SYN);

	current_bitmask = ((wiimote->payload[dataoffset+4] ^ 0xFF) << 8) | (wiimote->payload[dataoffset+5] ^ 0xFF);

	for(i=WIIMOTE_CLASSIC_UP; i<=WIIMOTE_CLASSIC_RIGHT; (i <<= 1)) {
	        dochange = 0;
		if((current_bitmask & i ) && !(wiimote->extbitmask_classic & i)) {
			updown=1; dochange=1;
		}
		else if((!(current_bitmask & i )) && (wiimote->extbitmask_classic & i)) {
			updown=0; dochange=1;
		}
		
		if(dochange) {
			switch(i) {
			case WIIMOTE_CLASSIC_UP:
			  do_uinput(uinput,BTN_FORWARD, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_DOWN:
			  do_uinput(uinput,BTN_BACK, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_LEFT:
			  do_uinput(uinput,BTN_LEFT, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_RIGHT:
			  do_uinput(uinput,BTN_RIGHT, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_MINUS:
			  do_uinput(uinput,BTN_SELECT, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_PLUS:
			  do_uinput(uinput,BTN_START, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_HOME:
			  do_uinput(uinput,BTN_MODE, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_L:
			  do_uinput(uinput,BTN_TL, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_R:
			  do_uinput(uinput,BTN_TR, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_ZL:
			  do_uinput(uinput,BTN_TL2, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_ZR:
			  do_uinput(uinput,BTN_TR2, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_A:
			  do_uinput(uinput,BTN_A, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_B:
			  do_uinput(uinput,BTN_B, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_X:
			  do_uinput(uinput,BTN_X, updown, EV_KEY);
			  break;
			case WIIMOTE_CLASSIC_Y:
			  do_uinput(uinput,BTN_Y, updown, EV_KEY);
			  break;
			case 0x100: /* not used */
			  break;
			default:
			  xxxdie_i("Unknown classic gamepad button ", i);
			};
		}
	}
	
	/* Store bitmask */
	wiimote->extbitmask_classic = current_bitmask;
	return TRUE;
}
