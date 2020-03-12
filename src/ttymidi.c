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
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>  
#include <alsa/asoundlib.h>
#include <fcntl.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdbool.h>  
#include <errno.h>  

#define SERIAL_PATH "/dev/ttyS4"
#define MAX_MSG_SIZE 1024
#define PRINTONLY 0
#define VERBOSE 1
#define SILENT 0

int serial;
int port_out_id;

int open_seq (snd_seq_t** seq) 
{
	int port;

	if (snd_seq_open(seq, "default", SND_SEQ_OPEN_INPUT, 0) < 0) 
	{
		fprintf(stderr, "Error opening ALSA sequencer.\n");
		exit(1);
	}

	snd_seq_set_client_name(*seq, "ttymidi");

	if ((port = snd_seq_create_simple_port(*seq, "ttymidi",
					SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					SND_SEQ_PORT_TYPE_APPLICATION)) < 0) 
	{
		fprintf(stderr, "Error creating sequencer port.\n");
	}

	return port;
}

int setup_serial_port (const char path[], int speed)
{
    //Below is some new code for setting up serial comms which allows me
    //to use non-standard baud rates (such as 31250 for MIDI interface comms).
    //This code was provided in this thread:
    //https://groups.google.com/forum/#!searchin/beagleboard/Peter$20Hurdley%7Csort:date/beagleboard/GC0rKe6rM0g/lrHWS_e2_poJ
    //This is a direct link to the example code:
    //https://gist.githubusercontent.com/peterhurley/fbace59b55d87306a5b8/raw/220cfc2cb1f2bf03ce662fe387362c3cc21b65d7/anybaud.c

    int fd;
    struct termios2 tio;
    
    // open device for read
    fd = open (path, O_RDONLY | O_NOCTTY);
    
    //if can't open file
    if (fd < 0)
    {
        //show error and exit
        perror (path);
        return (-1);
    }
    
    if (ioctl (fd, TCGETS2, &tio) < 0)
        perror("TCGETS2 ioctl");
    
    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= BOTHER;
    tio.c_ispeed = speed;
    tio.c_ospeed = speed;
    
    if (ioctl (fd, TCSETS2, &tio) < 0)
        perror("TCSETS2 ioctl");
    
    printf("%s speed set to %d baud\r\n", path, speed);

    return fd;
}

void parse_midi_command (snd_seq_t* seq, int port_out_id, char *buf)
{
	/*
	   MIDI COMMANDS
	   -------------------------------------------------------------------
	   name                 status      param 1          param 2
	   -------------------------------------------------------------------
	   note off             0x80+C       key #            velocity
	   note on              0x90+C       key #            velocity
	   poly key pressure    0xA0+C       key #            pressure value
	   control change       0xB0+C       control #        control value
	   program change       0xC0+C       program #        --
	   mono key pressure    0xD0+C       pressure value   --
	   pitch bend           0xE0+C       range (LSB)      range (MSB)
	   system               0xF0+C       manufacturer     model
	   -------------------------------------------------------------------
	   C is the channel number, from 0 to 15;
	   -------------------------------------------------------------------
	   source: http://ftp.ec.vanderbilt.edu/computermusic/musc216site/MIDI.Commands.html
	
	   In this program the pitch bend range will be transmitter as 
	   one single 8-bit number. So the end result is that MIDI commands 
	   will be transmitted as 3 bytes, starting with the operation byte:
	
	   buf[0] --> operation/channel
	   buf[1] --> param1
	   buf[2] --> param2        (param2 not transmitted on program change or key press)
   */

	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port_out_id);
	snd_seq_ev_set_subs(&ev);

	int operation, channel, param1, param2;

	operation = buf[0] & 0xF0;
	channel   = buf[0] & 0x0F;
	param1    = buf[1];
	param2    = buf[2];

	switch (operation)
	{
		case 0x80:
			if (!SILENT && VERBOSE) 
				printf("Serial  0x%x Note off           %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_noteoff(&ev, channel, param1, param2);
			break;
			
		case 0x90:
			if (!SILENT && VERBOSE) 
				printf("Serial  0x%x Note on            %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_noteon(&ev, channel, param1, param2);
			break;
			
		case 0xA0:
			if (!SILENT && VERBOSE) 
				printf("Serial  0x%x Pressure change    %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_keypress(&ev, channel, param1, param2);
			break;

		case 0xB0:
			if (!SILENT && VERBOSE) 
				printf("Serial  0x%x Controller change  %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_controller(&ev, channel, param1, param2);
			break;

		case 0xC0:
			if (!SILENT && VERBOSE) 
				printf("Serial  0x%x Program change     %03u %03u\n", operation, channel, param1);
			snd_seq_ev_set_pgmchange(&ev, channel, param1);
			break;

		case 0xD0:
			if (!SILENT && VERBOSE) 
				printf("Serial  0x%x Channel change     %03u %03u\n", operation, channel, param1);
			snd_seq_ev_set_chanpress(&ev, channel, param1);
			break;

		case 0xE0:
			param1 = (param1 & 0x7F) + ((param2 & 0x7F) << 7);
			if (!SILENT && VERBOSE) 
				printf("Serial  0x%x Pitch bend         %03u %05i\n", operation, channel, param1);
			snd_seq_ev_set_pitchbend(&ev, channel, param1 - 8192); // in alsa MIDI we want signed int
			break;

		/* Not implementing system commands (0xF0) */
			
		default:
			printf("0x%x Unknown MIDI cmd   %03u %03u %03u\n", operation, channel, param1, param2);
		    break;
	}

	snd_seq_event_output_direct(seq, &ev);
	snd_seq_drain_output(seq);
}

void read_midi_from_serial_port (snd_seq_t* seq) 
{
	char buf[3], msg[MAX_MSG_SIZE];
	int i, msglen;
	
	/* Lets first fast forward to first status byte... */
	if (PRINTONLY) {
		do read(serial, buf, 1);
		while (buf[0] >> 7 == 0);
	}

	while (1) 
	{
		/* 
		 * super-debug mode: only print to screen whatever
		 * comes through the serial port.
		 */

		if (PRINTONLY) 
		{
			read(serial, buf, 1);
			printf("%x\t", (int) buf[0]&0xFF);
			fflush(stdout);
			continue;
		}

        int i = 1;
        while (i < 3)
        {
            read(serial, buf+i, 1);

			if (buf[i] >> 7 != 0)
            {
				// status byte
				buf[0] = buf[i];
				i = 1;
			}
            else
            {
                // data byte
                if (i == 2) {i = 3;}
                else
                {
                    // if the message type is program change or mono key pressure, it only uses 2 bytes
                    if ((buf[0] & 0xF0) == 0xC0 || (buf[0] & 0xF0) == 0xD0)
						i = 3;
                    else
						i = 2;
                }
            }
        }

		parse_midi_command(seq, port_out_id, buf);
	}
}

int main (void)   
{
    // setup sequencer 
    printf ("Setting up ALSA sequencer...\n");
    snd_seq_t *seq;
    port_out_id = open_seq (&seq);

    // open UART device file for read/write
    printf ("Opening %s...\n", SERIAL_PATH);
    serial = setup_serial_port (SERIAL_PATH, 31250);

    // start main thread
    printf ("Starting read thread.\n");
    read_midi_from_serial_port (seq);
  
    return 0;  
}