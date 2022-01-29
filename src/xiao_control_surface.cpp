#include <Arduino.h>
#include <Control_Surface.h>

// MIDI Interface for use with Hairless
//HairlessMIDI_Interface midi;
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

int inputval[] =  {2, 32,    145,   353,    909,  4064};
int outputval[] = {0, 85, 171, 256, 341,  1023};

// note: the _in array should have increasing values
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

// The filtered value read when potentiometer is at the 0% position
constexpr analog_t minimumValue = 255;
// The filtered value read when potentiometer is at the 100% position
constexpr analog_t maximumValue = 16383 - 255;

analog_t mappingFunction(analog_t raw) {
    // make sure that the analog value is between the minimum and maximum
    // raw = constrain(raw, minimumValue, maximumValue);
    // map the value from [minimumValue, maximumValue] to [0, 16383]
    int mapped = multiMap(raw, inputval, outputval, 6);
    return map(mapped, 2, 1023, 0, 16383);
    // Note: 16383 = 2¹⁴ - 1 (the maximum value that can be represented by
    // a 14-bit unsigned number
}

// Initialize the Control Surface
void setup() {
  for (auto &volumePotentiometers : volumePotentiometers)
  volumePotentiometers.map(mappingFunction);
  Control_Surface.begin();
  // Serial.begin(115200);
}

// Update the Control Surface
void loop() {
  Control_Surface.loop();
}
