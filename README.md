# ttyMIDI for Bela

A very stripped-down version of ttyMIDI designed for Bela, whose sole task is to receive MIDI over a pre-specified port and pass it to ALSA. It sets the specifed UART port (UART4 by default, defined as the macro SERIAL_PATH) to a baud rate of 31250, so it can natively interface with any standard MIDI device.
