/*
  This file is public domain. Use it as you like.
  
  This is example program to showcase reading MIDI
  coming from Alsa sequencer with Arduino. We check
  whether there is MIDI message waiting at the serial
  buffer, then read the message and send it back
  to ttyMidi (if it happened to be either noteon or
  noteof message) 
  
 */

#include <ardumidi.h>

void setup()
{
  Serial.begin(115200);
}

int counter = 0;

void loop()
{
  while (midiMessage_available()>0) {
    MidiMessage m = read_MidiMessage();
    if (m.command == MIDI_NOTEON) {
      midi_note_on(m.channel, m.param1, m.param1);
    } 
    else if (m.command == MIDI_NOTEOFF) {
      midi_note_off(m.channel, m.param1, m.param2);
    }
    else if (m.command == MIDI_PITCHBEND) {
      midi_pitch_bend(m.channel,get_pitch_bend(m));  
    }
  }
}
