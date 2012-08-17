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
#include "nunchuk.h"
#include "classic.h"
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int main () {
	wiimote wiimote;
	bdaddr_t bdaddr;
	int uinput_fd = -1;
	
	/* Sync memory with WiiMote ;-) */
	wiimote.last_status_update = 0; /* Request Update ASAP */
	wiimote.corebitmask        = 0; /* Startup with no buttons pressed */
	wiimote.statmask           = 0; /* Startup without extension / zero LEDs */
	
	
	/* Init uinput driver */
	uinput_fd = init_uinput_device();
	if(uinput_fd < 0)
		xxxdie("Registering the uinput device failed, aborting!");
	
	
	str2ba("2C:10:C1:BF:D5:B1", &bdaddr);

	printf("Searching Wiimotes ; press 1+2 or the red SYNC button...\n");
	if(wiimote_connect(bdaddr, &wiimote) != TRUE)
		xxxdie("Connecting to wiimote failed");
	
	printf("Connection established! %p\n", &wiimote);
	
	wiimote_exec(&wiimote, WIIMOTE_CMD_SETRPT, WIIMOTE_RPT_ACMETER);

	for(;;) {
		dispatch_data(&wiimote,uinput_fd);
	}
	
	return 0;
}





/* Search handler for wiimote-interrupt data */
int dispatch_data(wiimote *wiimote, int uinput) {
	static unsigned char dpatch[WIIMOTE_MAX_PAYLOAD];
	unsigned int         bytes_read = -1;
	time_t               ctime;
	//int		i;

	/* Refresh battery */
	ctime = time(0);
	if(ctime > wiimote->last_status_update) {
		wiimote->last_status_update = ctime + WIIMOTE_STATUS_UPDATE_DELAY;
		printf("Updating Wiimote Status (battery LEDs)\n");
		wiimote_exec(wiimote,WIIMOTE_CMD_RQSTATUS,0);
	}
	
	bytes_read = read(wiimote->sock_interrupt,dpatch,WIIMOTE_MAX_PAYLOAD);
	
	if(bytes_read < WIIMOTE_MIN_PAYLOAD)
		xxxdie("Reading interrupt-data failed");

	
	memcpy(wiimote->payload, dpatch, bytes_read);


	/*for(i=0;i<bytes_read;i++) {
		printf("%02X ",wiimote->payload[i]);
	}
	printf("\n");

	printf("%02X\n",dpatch[1]);*/

	if(dpatch[1] == WIIMOTE_RPT_COREONLY) {
		wiimote_input_corecore(wiimote,uinput, 2);
	}
	else if(dpatch[1] == WIIMOTE_RPT_COREACEX) {
	  //wiimote_input_extnunk(wiimote,uinput,wiimote_input_coreaccel(wiimote,uinput,wiimote_input_corecore(wiimote,uinput,2)));
		wiimote_input_ext_classic(wiimote,uinput,wiimote_input_coreaccel(wiimote,uinput,wiimote_input_corecore(wiimote,uinput,2)));
	}
	else if(dpatch[1] == WIIMOTE_RPT_ACMETER) {
		wiimote_input_coreaccel(wiimote,uinput,wiimote_input_corecore(wiimote,uinput,2));
		}
	else if(dpatch[1] == WIIMOTE_STATUS_EVENT) {
	        wiimote->last_status_update = ctime + WIIMOTE_STATUS_UPDATE_DELAY;
	  	wiimote_update_status(wiimote);
	}
	else if(dpatch[1] == WIIMOTE_STATUS_VOID) {
		xxxwarn("broken cable?!\n");
	}
	else if(dpatch[1] == WIIMOTE_STATUS_READ) {
		printf("< -- data received\n");
	}
	else if(dpatch[1] == WIIMOTE_STATUS_WRACK) {
		/* Write ACK */
	}
	else {
		xxxdie_i("Unhandled input type: ",dpatch[1]);
	}
	return FALSE;
}









int xxxdie(char *msg) {
	printf("Error: %s\n",msg);
	exit(1);
}

int xxxdie_i(char *msg, int i) {
	printf("Error: %s 0x%x\n",msg,i);
	exit(1);
}

int xxxwarn(char *msg) {
	printf("Warning: %s\n",msg);
	return TRUE;
}
