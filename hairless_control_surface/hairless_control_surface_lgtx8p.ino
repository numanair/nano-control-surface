#include <Arduino.h>
#include <Control_Surface.h>
#include <avr/power.h>
#include "lgtx8p.h"

// MIDI Interface for use with Hairless
//USBMIDI_Interface midi;
HairlessMIDI_Interface midi;

// Potentiometer array only sends MIDI
// messages when a value changes
CCPotentiometer volumePotentiometers[] = {
  {A0, {MIDI_CC::Channel_Volume, CHANNEL_1}},
  {A1, {MIDI_CC::Channel_Volume, CHANNEL_2}},
  {A2, {MIDI_CC::Channel_Volume, CHANNEL_3}},
  {A3, {MIDI_CC::Channel_Volume, CHANNEL_4}},
  {A4, {MIDI_CC::Channel_Volume, CHANNEL_5}},
};

// Initialize the Control Surface
void setup() {
  Control_Surface.begin();
}

// Update the Control Surface
void loop() {
  Control_Surface.loop();
}
