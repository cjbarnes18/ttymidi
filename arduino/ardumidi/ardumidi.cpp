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

#include "WProgram.h"
#include "HardwareSerial.h"
#include "ardumidi.h"

void midi_note_off(byte channel, byte key, byte velocity)
{
	midi_command(0x80, channel, key, velocity);
}

void midi_note_on(byte channel, byte key, byte velocity)
{
	midi_command(0x90, channel, key, velocity);
}

void midi_key_pressure(byte channel, byte key, byte value)
{
	midi_command(0xA0, channel, key, value);
}

void midi_controller_change(byte channel, byte control, byte value)
{
	midi_command(0xB0, channel, control, value);
}

void midi_program_change(byte channel, byte program)
{
	midi_command(0xC0, channel, program, 0);
}

void midi_channel_pressure(byte channel, byte value)
{
	midi_command(0xD0, channel, value, 0);
}

void midi_pitch_bend(byte channel, int value)
{
	midi_command(0xE0, channel, value & 0x7F, value >> 7);
}

void midi_command(byte command, byte channel, byte param1, byte param2)
{
	Serial.print(command | (channel & 0x0F), BYTE);
	Serial.print(param1 & 0x7F, BYTE);
	Serial.print(param2 & 0x7F, BYTE);
	Serial.flush();
}

void midi_printbytes(char* msg, int len)
{
	Serial.print(0xFF, BYTE);
	Serial.print(0x00, BYTE);
	Serial.print(0x00, BYTE);
	Serial.print(len , BYTE);
	Serial.print(msg);
	Serial.flush();
}

void midi_comment(char* msg)
{
	int   len = 0;
	char* ptr = msg;
	while (*ptr++) len++;
	midi_printbytes(msg, len);
}
