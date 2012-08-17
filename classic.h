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

#define WIIMOTE_CLASSIC_LEFT_ANALOG_X 0x01
#define WIIMOTE_CLASSIC_LEFT_ANALOG_Y 0x02
#define WIIMOTE_CLASSIC_RIGHT_ANALOG_X 0x03
#define WIIMOTE_CLASSIC_RIGHT_ANALOG_Y 0x04
#define WIIMOTE_CLASSIC_L_ANALOG 0x05
#define WIIMOTE_CLASSIC_R_ANALOG 0x06

#define WIIMOTE_CLASSIC_UP 0x0001
#define WIIMOTE_CLASSIC_DOWN 0x4000
#define WIIMOTE_CLASSIC_LEFT 0x0002
#define WIIMOTE_CLASSIC_RIGHT 0x8000
#define WIIMOTE_CLASSIC_MINUS 0x1000
#define WIIMOTE_CLASSIC_PLUS 0x0400
#define WIIMOTE_CLASSIC_HOME 0x0800
#define WIIMOTE_CLASSIC_L 0x2000
#define WIIMOTE_CLASSIC_R 0x0200
#define WIIMOTE_CLASSIC_ZL 0x0080
#define WIIMOTE_CLASSIC_ZR 0x0004
#define WIIMOTE_CLASSIC_A 0x0010
#define WIIMOTE_CLASSIC_B 0x0040
#define WIIMOTE_CLASSIC_X 0x0008
#define WIIMOTE_CLASSIC_Y 0x0020

int wiimote_input_ext_classic(wiimote *wiimote, int uinput, int dataoffset);
