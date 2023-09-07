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

#ifndef __TERMIOS2_H__
#define __TERMIOS2_H__
#include <stdint.h>

#ifdef __gnu_linux__
int setup_termios2_tty(int tty_fd, uint32_t speed);
int exit_termios2_tty(int tty_fd);
#else
static inline int setup_termios2_tty(int tty_fd, uint32_t speed)
{
	return -1;
}
static inline int exit_termios2_tty(int tty_fd)
{
	return -1;
}
#endif

#endif
