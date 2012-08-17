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

#include <bluetooth/bluetooth.h>
#include <time.h>

#define WIIMOTE_MAX_PAYLOAD 20           /* Do not read more than 20 bytes from Wiimote */
#define WIIMOTE_MIN_PAYLOAD 2            /* But expect to get 2 or more bytes */
#define WIIMOTE_STATUS_UPDATE_DELAY 120  /* How often we check the battery */

#define WIIMOTE_EXT_ISCON 0x0002         /* Extension is connected */
#define WIIMOTE_LED_1     0x0010         /* Enable LED # 1 */
#define WIIMOTE_LED_2     0x0020         /* Enable LED # 2 */
#define WIIMOTE_LED_3     0x0040         /* Enable LED # 3 */
#define WIIMOTE_LED_4     0x0080         /* Enable LED # 4 */
#define WIIMOTE_LED_ALL   0x00F0         /* ALL 4 LEDs */
#define WIIMOTE_LED_NONE  0x0000         /* None */
#define WIIMOTE_LED_LAST  0x0080         /* LED # 1 */
#define WIIMOTE_LED_FIRST 0x0010         /* LED # 2 */

#define WIIMOTE_CMD_SETLED   0x11
#define WIIMOTE_CMD_SETRPT   0x12
#define WIIMOTE_CMD_RQSTATUS 0x15
#define WIIMOTE_CMD_WRITE    0x16
#define WIIMOTE_CMD_READ     0x17

#define WIIMOTE_STATUS_EVENT 0x20
#define WIIMOTE_STATUS_READ  0x21
#define WIIMOTE_STATUS_WRACK 0x22
#define WIIMOTE_RPT_COREONLY 0x30
#define WIIMOTE_RPT_ACMETER  0x31
#define WIIMOTE_RPT_CORE8EX  0x32
#define WIIMOTE_RPT_COREACEX 0x35
#define WIIMOTE_STATUS_VOID  0xff



#define WIIMOTE_BUTTON_2     0x0001
#define WIIMOTE_BUTTON_1     0x0002
#define WIIMOTE_BUTTON_B     0x0004
#define WIIMOTE_BUTTON_A     0x0008
#define WIIMOTE_BUTTON_MINUS 0x0010
#define WIIMOTE_BUTTON_UNK1  0x0020
#define WIIMOTE_BUTTON_UNK2  0x0040
#define WIIMOTE_BUTTON_HOME  0x0080
#define WIIMOTE_BUTTON_LEFT  0x0100
#define WIIMOTE_BUTTON_RIGHT 0x0200
#define WIIMOTE_BUTTON_DOWN  0x0400
#define WIIMOTE_BUTTON_UP    0x0800
#define WIIMOTE_BUTTON_PLUS  0x1000
#define WIIMOTE_BUTTON_FIRST 1
#define WIIMOTE_BUTTON_LAST  4096


typedef struct  {
	int sock_controll;
	int sock_interrupt;
	unsigned char payload[WIIMOTE_MAX_PAYLOAD];
	unsigned int corebitmask;
	unsigned int extbitmask_nunchuk;
        unsigned int extbitmask_classic;
	unsigned char battery;
	time_t last_status_update;
	unsigned char statmask;
} wiimote;


int wiimote_connect(bdaddr_t bdaddr, wiimote *wiimote);
int wiimote_exec_through_controll(wiimote *wiimote, char command, char value);
int wiimote_exec_through_interrupt(wiimote *wiimote, char command, char value);
int wiimote_set_battery_display(wiimote *wiimote);
int wiimote_input_coreaccel(wiimote *wiimote, int uinput, int dataoffset);
int wiimote_decode_status(wiimote *wiimote);
int wiimote_input_corecore(wiimote *wiimote, int uinput, int dataoffset);
int wiimote_update_status(wiimote *wiimote);
