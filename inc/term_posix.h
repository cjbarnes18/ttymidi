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
#ifndef __TERM_POSIX_H__
#define __TERM_POSIX_H__

#include <stdint.h>

int setup_posix_tty(int tty_fd, uint32_t speed);
int exit_posix_tty(int tty_fd);

#endif
