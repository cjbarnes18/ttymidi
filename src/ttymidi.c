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

#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>

bool verbose = true;

int open_seq(snd_seq_t** seq) 
{
	int port_out_id, port_in_id; // actually port_in_id is not needed nor used anywhere

	if (snd_seq_open(seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) 
	{
		fprintf(stderr, "Error opening ALSA sequencer.\n");
		exit(1);
	}

	snd_seq_set_client_name(*seq, "ttymidi");

	if ((port_out_id = snd_seq_create_simple_port(*seq, "MIDI out",
					SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					SND_SEQ_PORT_TYPE_APPLICATION)) < 0) 
	{
		fprintf(stderr, "Error creating sequencer port.\n");
	}

	if ((port_in_id = snd_seq_create_simple_port(*seq, "MIDI in",
					SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
					SND_SEQ_PORT_TYPE_APPLICATION)) < 0) 
	{
		fprintf(stderr, "Error creating sequencer port.\n");
	}

	return port_out_id;
}

void parse_midi_command(snd_seq_t* seq, int port_out_id, char *buf)
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
			if (verbose) 
				printf("Serial  0x%x Note off           %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_noteoff(&ev, channel, param1, param2);
			break;
			
		case 0x90:
			if (verbose) 
				printf("Serial  0x%x Note on            %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_noteon(&ev, channel, param1, param2);
			break;
			
		case 0xA0:
			if (verbose) 
				printf("Serial  0x%x Pressure change    %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_keypress(&ev, channel, param1, param2);
			break;

		case 0xB0:
			if (verbose) 
				printf("Serial  0x%x Controller change  %03u %03u %03u\n", operation, channel, param1, param2);
			snd_seq_ev_set_controller(&ev, channel, param1, param2);
			break;

		case 0xC0:
			if (verbose) 
				printf("Serial  0x%x Program change     %03u %03u\n", operation, channel, param1);
			snd_seq_ev_set_pgmchange(&ev, channel, param1);
			break;

		case 0xD0:
			if (verbose) 
				printf("Serial  0x%x Channel change     %03u %03u\n", operation, channel, param1);
			snd_seq_ev_set_chanpress(&ev, channel, param1);
			break;

		case 0xE0:
			param1 = (param1 & 0x7F) + ((param2 & 0x7F) << 7);
			if (verbose) 
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