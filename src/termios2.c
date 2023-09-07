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

#ifdef __gnu_linux__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm/termbits.h>

#include <termios2.h>

static struct termios2 oldtio;

int setup_termios2_tty(int tty_fd, uint32_t speed)
{
	struct termios2 newtio;
	int ret;

	/* save current serial port settings */
	ret = ioctl(tty_fd, TCGETS2, &oldtio); 
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
	newtio.c_cflag = BOTHER | CS8 | CLOCAL | CREAD
			 | ((BOTHER) << IBSHIFT);

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

	newtio.c_ospeed = speed;
	newtio.c_ispeed = speed;

	/* 
	 * now clean the modem line and activate the settings for the port
	 */
	return ioctl(tty_fd, TCSETSF2, &newtio);
}

int exit_termios2_tty(int tty_fd)
{
	/* restore the old port settings */
	return ioctl(tty_fd, TCSETSF2, &oldtio);
}

#endif
