/*
    This file is part of ttymidi.

    ttymidi is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ttymidi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ttymidi.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

#include "term_posix.h"

static struct termios oldtio;

int setup_posix_tty(int tty_fd, uint32_t speed)
{
	struct termios newtio;
	int b_speed;
	int ret;

	switch (speed)
	{
		case 1200   : b_speed = B1200  ; break;
		case 2400   : b_speed = B2400  ; break;
		case 4800   : b_speed = B4800  ; break;
		case 9600   : b_speed = B9600  ; break;
		case 19200  : b_speed = B19200 ; break;
		case 38400  : b_speed = B38400 ; break;
		case 57600  : b_speed = B57600 ; break;
		case 115200 : b_speed = B115200; break;
		default:
			printf("Baud rate %i is not supported.\n", speed);
			return -EINVAL;
	}


	/* save current serial port settings */
	ret = tcgetattr(tty_fd, &oldtio); 
	if (ret) {
		printf("Can't get current tty settings: %s\n", strerror(errno));
		return ret;
	}

	/* clear struct for new port settings */
	memset(&newtio, 0, sizeof(newtio)); 

	/* 
	 * BAUDRATE : Set bps rate. You could also use cfsetispeed and cfsetospeed.
	 * CRTSCTS  : output hardware flow control (only used if the cable has
	 * all necessary lines. See sect. 7 of Serial-HOWTO)
	 * MIDI 5 Pin DIN doesn't have such line.
	 * CS8      : 8n1 (8bit, no parity, 1 stopbit)
	 * CLOCAL   : local connection, no modem contol
	 * CREAD    : enable receiving characters
	 */
	newtio.c_cflag = b_speed | CS8 | CLOCAL | CREAD;

	/*
	 * IGNPAR  : ignore bytes with parity errors
	 * ICRNL   : map CR to NL (otherwise a CR input on the other computer
	 * will not terminate input)
	 * otherwise make device raw (no other input processing)
	 */
	newtio.c_iflag = IGNPAR;

	/* Raw output */
	newtio.c_oflag = 0;

	/*
	 * ICANON  : enable canonical input
	 * disable all echo functionality, and don't send signals to calling program
	 */
	newtio.c_lflag = 0; // non-canonical

	/* 
	 * set up: we'll be reading 4 bytes at a time.
	 */
	newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;     /* blocking read until n character arrives */

	/* 
	 * now clean the modem line and activate the settings for the port
	 */
	tcflush(tty_fd, TCIFLUSH);

	return tcsetattr(tty_fd, TCSANOW, &newtio);
}

int exit_posix_tty(int tty_fd)
{
	tcflush(tty_fd, TCIFLUSH);
	/* restore the old port settings */
	return tcsetattr(tty_fd, TCSANOW, &oldtio);
}
