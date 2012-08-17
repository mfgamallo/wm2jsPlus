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

#include "wiimote.h"
#include "wm2jsPlus.h"
#include "uinput.h"
#include <bluetooth/l2cap.h>
#include <unistd.h>

/* Bluetooth-connect the wiimote */
int wiimote_connect(bdaddr_t bdaddr, wiimote *wiimote) {
	struct sockaddr_l2 wm_ctrl, wm_int;
	int cr_ctrl, cr_int;
	
	memset(&wm_ctrl,0, sizeof(wm_ctrl));
	wm_ctrl.l2_family = AF_BLUETOOTH;
	wm_ctrl.l2_bdaddr = bdaddr;
	wm_ctrl.l2_psm    = htobs(17);

	memset(&wm_int,0, sizeof(wm_int));
	wm_int.l2_family = AF_BLUETOOTH;
	wm_int.l2_bdaddr = bdaddr;
	wm_int.l2_psm    = htobs(19);
	
	wiimote->sock_controll = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	wiimote->sock_interrupt= socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	cr_ctrl = connect(wiimote->sock_controll, (struct sockaddr *)&wm_ctrl,
	                  sizeof(wm_ctrl));
	if(cr_ctrl)
		xxxdie("Unable to establish controlling channel");
	
	cr_int = connect(wiimote->sock_interrupt, (struct sockaddr *)&wm_int,
	                  sizeof(wm_int));
	if(cr_int)
		xxxdie("Unable to establish interrupt channel");
	return TRUE;
}


/* Execute simple WiiMote commands */
int wiimote_exec(wiimote *wiimote, char command, char value) {
	static unsigned char buf[4]; /* Wuuha! */
	static unsigned int  buflen = 0;

	switch (command) {
		case WIIMOTE_CMD_SETLED:
			buf[0]= 0xa2;
			buf[1]= WIIMOTE_CMD_SETLED;
			buf[2] = value;
			buflen = 3;
			break;
		case WIIMOTE_CMD_SETRPT:
		        buf[0] = 0xa2;
			buf[1] = WIIMOTE_CMD_SETRPT;
			buf[2] = 0x00;
			buf[3] = value;
			buflen = 4;
			break;
		case WIIMOTE_CMD_RQSTATUS:
			buf[0] = 0xa2;
			buf[1] = WIIMOTE_CMD_RQSTATUS;
			buf[2] = 0x00;
			buflen = 3;
			break;
		default:
			xxxdie_i("FATAL: Unknown command:",command);
			break;
	}
	
	if(write(wiimote->sock_interrupt,buf,buflen) != buflen)
		xxxdie("Write to wiimote failed");
	if(read(wiimote->sock_interrupt,buf,1) != 1)
		xxxdie("Reading ACK from wiimote failed");
	/*	if(buf[0] != 0x00)
		xxxdie_i("Error reported from Wiimote ; aborting -> ",buf[0]);*/
	
	return TRUE;
}


/* Set Battery Usage ; does not upgrade statmask for some strange reason .. */
int wiimote_set_battery_display(wiimote *wiimote) {
	int batlevel = -1;
	int cleds;
	
	if(wiimote->battery > 84)
		batlevel = WIIMOTE_LED_ALL;
	else if(wiimote->battery > 69)
		batlevel = WIIMOTE_LED_3|WIIMOTE_LED_2|WIIMOTE_LED_1;
	else if(wiimote->battery > 49)
		batlevel = WIIMOTE_LED_2|WIIMOTE_LED_1;
	else if(wiimote->battery > 19)
		batlevel = WIIMOTE_LED_LAST;
	
	if(batlevel < 0) {
		/* Battery almost empty! */
		wiimote_exec(wiimote, WIIMOTE_CMD_SETLED, WIIMOTE_LED_2|WIIMOTE_LED_3);
		return FALSE;
	}
	
	/* Grab current LEDs */
	cleds = wiimote->statmask & WIIMOTE_LED_ALL;
	
	if(cleds == batlevel) {
		/* void */
	}
	else {
		/* Status change */
		wiimote_exec(wiimote, WIIMOTE_CMD_SETLED,batlevel);
	}
	return TRUE;
}


int wiimote_read_data(wiimote *wiimote) {
	xxxdie("You shall not use unfinished code");
	static unsigned char buf[8];
	memset(&buf,0,sizeof(buf));
	buf[0] = 0x52;
	buf[1] = 0x17;
	buf[2] = 0x04; /* 00 eeprom | 04 spamce */
	buf[3] = 0xA4;
	buf[4] = 0x00;
	buf[5] = 0xFE;
	buf[6] = 0x00;
	buf[7] = 0x02;
	write(wiimote->sock_controll,buf,8);
	return TRUE;
}

int wiimote_write_data(wiimote *wiimote, int offset) {

  	static unsigned char buf[23];
	memset(&buf,0,sizeof(buf));
	
	printf("WARNING: wiimote_write writes hardcoded values .. iieks!\n");
	
	buf[0] = 0xa2;
	buf[1] = 0x16;
	buf[2] = 0x04;
	buf[3] = 0xA4;
	buf[4] = 0x00;
	buf[5] = 0xF0;
	buf[6] = 0x00;
	buf[7] = 0x55;
	buf[22] = 0x55;

       	write(wiimote->sock_interrupt,buf,23);

	buf[0] = 0xa2;
	buf[1] = 0x16;
	buf[2] = 0x04;
	buf[3] = 0xA4;
	buf[4] = 0x00;
	buf[5] = 0xFB;
	buf[6] = 0x00;
	buf[7] = 0x00;
	buf[22] = 0x00;

       	write(wiimote->sock_interrupt,buf,23);

	return TRUE;
}

/* Send Accel events to uinput */
int wiimote_input_coreaccel(wiimote *wiimote, int uinput, int dataoffset) {
	do_uinput(uinput,ABS_RX,wiimote->payload[dataoffset++], EV_ABS);
	do_uinput(uinput,ABS_RY,wiimote->payload[dataoffset++], EV_ABS);
	do_uinput(uinput,ABS_RZ,wiimote->payload[dataoffset++], EV_ABS);
	return dataoffset;
}

/* Update wiimote status and inits Extension if needed */
int wiimote_update_status(wiimote *wiimote) {
        wiimote_decode_status(wiimote);
	wiimote_set_battery_display(wiimote);
	
	/* Bug: Battery-Update does also change mask */
	if(wiimote->statmask & WIIMOTE_EXT_ISCON) {
		printf("-> Extension is connected\n");
		wiimote_write_data(wiimote, 0x04000000|0xA40040);
		wiimote_exec(wiimote,WIIMOTE_CMD_SETRPT,WIIMOTE_RPT_COREACEX);
	}
	else {
		printf("-> No extension connected\n");
		wiimote_exec(wiimote,WIIMOTE_CMD_SETRPT,WIIMOTE_RPT_ACMETER);
	}
	
	return TRUE;
}

/* Updates statmask and battery status */
int wiimote_decode_status(wiimote *wiimote) {
	wiimote->battery  = wiimote->payload[7];
	wiimote->statmask = wiimote->payload[4];
	return TRUE;
}



/* Sendout CoreButton events to uinput */
int wiimote_input_corecore(wiimote *wiimote, int uinput, int dataoffset) {
	int bpressed,i,dochange = 0;
	int updown = 0;
	
	bpressed = ((wiimote->payload[dataoffset++] * 0x100));
	bpressed += wiimote->payload[dataoffset++];
	
	
	for(i=WIIMOTE_BUTTON_FIRST;i<=WIIMOTE_BUTTON_LAST; (i = i * 2)) {
		dochange = 0;
		if((bpressed & i) && !(wiimote->corebitmask & i)) {
			updown = 1;
			dochange = 1;
		}
		else if( (!(bpressed & i)) && (wiimote->corebitmask & i) ) {
			updown = 0;
			dochange = 1;
		}
		
		if(dochange) {
			switch(i) {
				case WIIMOTE_BUTTON_1:
					do_uinput(uinput,BTN_TOP2,updown,EV_KEY); // OK
					break;
				case WIIMOTE_BUTTON_2:
					do_uinput(uinput,BTN_TRIGGER,updown,EV_KEY);  // OK
					break;
				case WIIMOTE_BUTTON_PLUS:
					do_uinput(uinput,BTN_BASE2,updown,EV_KEY);
					break;
				case WIIMOTE_BUTTON_MINUS:
					do_uinput(uinput,BTN_BASE,updown,EV_KEY);
					break;
				case WIIMOTE_BUTTON_HOME:
					do_uinput(uinput,BTN_THUMB,updown,EV_KEY);
					break;
				case WIIMOTE_BUTTON_B:                      // OK
					do_uinput(uinput,BTN_TOP,updown,EV_KEY);
					break;
				case WIIMOTE_BUTTON_A:                      // OK
					do_uinput(uinput,BTN_BASE3,updown,EV_KEY);
					break;
				case WIIMOTE_BUTTON_UP:
					do_uinput(uinput,ABS_X, (updown*-0xFFFF),EV_ABS);
					break;
				case WIIMOTE_BUTTON_DOWN:
					do_uinput(uinput,ABS_X,(updown*0xFFFF),EV_ABS);
					break;
				case WIIMOTE_BUTTON_LEFT:
					do_uinput(uinput,ABS_Y,(updown*0xFFFF), EV_ABS);
					break;
				case WIIMOTE_BUTTON_RIGHT:
					do_uinput(uinput,ABS_Y,(updown*-0xFFFF), EV_ABS);
					break;
				case WIIMOTE_BUTTON_UNK1:
					//printf("-> Unknown 1 button! %d\n",updown);
					break;
				case WIIMOTE_BUTTON_UNK2:
					//printf("-> Unknown 2 button! %d\n", updown);
					break;
				default:
					xxxdie_i("Unknown button keycode! ",i);
					break;
			}
		}
	}
	
	/* Set new mask */
	wiimote->corebitmask = bpressed;
	
	return dataoffset;
}

