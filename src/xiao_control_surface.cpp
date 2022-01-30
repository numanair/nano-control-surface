#include <Arduino.h>
#include <Control_Surface.h>

const int NUM_SLIDERS = 5;

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

// make sure Control Surface is using 12-bit ADC 
constexpr uint8_t ADC_BITS = 12;

int inputval[] =  {20, 16383};
int outputval[] = {0,  16383};
// Note: 16383 = 2¹⁴ - 1 (the maximum value that can be represented by
// a 14-bit unsigned number
const int arrayQty = 2;
// number of elements in the MultiMap arrays

Timer<millis> timer = 500; // milliseconds between sending serial data

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
    int mapped = multiMap(raw, inputval, outputval, arrayQty);
    // adjusts slider output for better linearity
    return mapped;
}

void sendSliderValues();

// Initialize the Control Surface
void setup() {
  for (auto &volumePotentiometers : volumePotentiometers)
  volumePotentiometers.map(mappingFunction);
  Control_Surface.begin();
  Serial.begin(115200);
}

// Update the Control Surface
void loop() {
  Control_Surface.loop();
  for (size_t i = 0; i < 5; i++)
  {
  // Serial.println(volumePotentiometers[1].getRawValue());
    if (timer)
    {
    sendSliderValues();
    }
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)volumePotentiometers[i].getRawValue());
    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  Serial.println(builtString);
}
