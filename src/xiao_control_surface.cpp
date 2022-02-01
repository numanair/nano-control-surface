#include <Arduino.h>
#include <Control_Surface.h>

// Number of potentiometers or faders
const int NUM_SLIDERS = 5;

// Adjusts linearity correction for my specific potentiometers.
// 1 = fully linear but jittery. 0.7 is about max for no jitter.
const float correctionMultiplier = 0.78;

// measured output every 5mm increment
int measuredInput[] = {25, 200,  660,  1650, 3628, 5800, 7900, 10180, 12380, 14580, 15690, 16120, 16350};

// MIDI over USB :)
USBMIDI_Interface midi;

// Potentiometer array only sends MIDI
// messages when a value changes
CCPotentiometer volumePotentiometers[] = {
  {A0, {MIDI_CC::Channel_Volume, CHANNEL_1}},
  {A1, {MIDI_CC::Channel_Volume, CHANNEL_2}},
  {A2, {MIDI_CC::Channel_Volume, CHANNEL_3}},
  {A3, {MIDI_CC::Channel_Volume, CHANNEL_4}},
  {A4, {MIDI_CC::Channel_Volume, CHANNEL_5}},
};

// Probably no need to change these calculated values
int idealOutputValues[] = {0,  1365, 2730, 4096, 5461, 6826, 8192, 9557,  10922, 12288, 13653, 15018, 16383};
// Note: 16383 = 2¹⁴ - 1 (the maximum value that can be represented by
// a 14-bit unsigned number

// number of elements in the MultiMap arrays
const int arrayQty = sizeof(measuredInput) / sizeof(measuredInput[0]);
int adjustedinputval[arrayQty] = {0};

// make sure Control Surface is using 12-bit ADC 
constexpr uint8_t ADC_BITS = 12;

// milliseconds between sending serial data
Timer<millis> timer = 10;

// note: the _in array must have increasing values
// same function as MultiMap Arduino library
int multiMap(int val, int* _in, int* _out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] already tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}

analog_t mappingFunction(analog_t raw) {
    int mapped = multiMap(raw, adjustedinputval, idealOutputValues, arrayQty);
    // adjusts slider output for better linearity
    return mapped;
}

void sendSliderValues();

// Initialize the Control Surface
void setup() {
  for (auto &volumePotentiometers : volumePotentiometers)
  volumePotentiometers.map(mappingFunction);
  analogReadResolution(12);
  Control_Surface.begin();
  Serial.begin(115200);
  // Start serial for Deej

  // multiplier correction
  for (size_t i = 0; i < arrayQty; i++)
  {
    adjustedinputval[i] = round(idealOutputValues[i] + (measuredInput[i] - idealOutputValues[i]) * correctionMultiplier);
    // theoretical ideal + (measured - theoretical)*multi
  }
}

// Update the Control Surface
void loop() {
  Control_Surface.loop();
  if (timer)
  {
    sendSliderValues();
  }
}

// Deej Serial Support
void sendSliderValues() {
  String builtString = String("");
  for (int i = 0; i < NUM_SLIDERS; i++) {
    // builtString += String((int)volumePotentiometers[i].getRawValue()); // raw
    builtString += String((int)volumePotentiometers[i].getValue()); // filtered
    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  Serial.println(builtString);
}
