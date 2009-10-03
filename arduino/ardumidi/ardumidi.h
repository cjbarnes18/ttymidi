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
// file version 0.12

#ifndef ardumidi_h
#define ardumidi_h

#include "WProgram.h"

#define MIDI_C0            0
#define MIDI_D0            2
#define MIDI_E0            4
#define MIDI_F0            5
#define MIDI_G0            7
#define MIDI_A0            9
#define MIDI_B0           11
#define MIDI_C            60
#define MIDI_D            62
#define MIDI_E            64
#define MIDI_F            65
#define MIDI_G            67
#define MIDI_A            69
#define MIDI_B            71
#define MIDI_SHARP         1
#define MIDI_FLAT         -1
#define MIDI_OCTAVE       12

void midi_note_off(byte channel, byte key, byte velocity);
void midi_note_on(byte channel, byte key, byte velocity);
void midi_key_pressure(byte channel, byte key, byte value);
void midi_controller_change(byte channel, byte control, byte value);
void midi_program_change(byte channel, byte program);
void midi_channel_pressure(byte channel, byte value);
void midi_pitch_bend(byte channel, int value);
void midi_command(byte command, byte channel, byte param1, byte param2);
void midi_print(char* msg, int len);
void midi_comment(char* msg);


#endif
