#include <Arduino.h>
#include <Control_Surface.h>
#include <avr/power.h>
#include "lgtx8p.h"

const int ADC_bits = 10; // 10 or 12-bit
const int MaxADC = 1016; // 4064 for 12-bit or 1016 for 10-bit

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
  analogReference(DEFAULT);
  analogReadResolution(ADC_bits);
  Control_Surface.begin();
}

// Update the Control Surface
void loop() {
  Control_Surface.loop();
}
