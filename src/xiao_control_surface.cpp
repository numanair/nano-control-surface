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

// int inputval[] =  {35,	128,	580,	1412,	3636,	16256};
// int outputval[] = {0,	1364,	2728,	4092,	5456,	16383};

int inputval[] =  {40, 1365, 2730, 4096, 5461, 6826, 8192, 9557, 10922, 12288, 13653, 15018, 16384};
int outputval[] = {0, 900, 2000, 3200, 4800, 6550, 8192, 9557, 10922, 12288, 13653, 15018, 16384};

// int inputval[] =  {25,	180,	620,	1390,	3620,	5680,	7890,	10180,	12380,	14580,	15690,	16120,	16350};
// int outputval[] = {0,	1365,	2730,	4096,	5461,	6826,	8192,	9557,	10922,	12288,	13653,	15018,	16383};
// Note: 16383 = 2¹⁴ - 1 (the maximum value that can be represented by
// a 14-bit unsigned number
const int arrayQty = sizeof(inputval) / sizeof(inputval[0]);
// number of elements in the MultiMap arrays

Timer<millis> timer = 10; // milliseconds between sending serial data

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
  // Start serial for Deej
}

// Update the Control Surface
void loop() {
  Control_Surface.loop();
  for (size_t i = 0; i < 5; i++)
  {
    if (timer)
    {
    sendSliderValues();
    }
  }
}

// Deej Serial Support
void sendSliderValues() {
  String builtString = String("");
  for (int i = 0; i < NUM_SLIDERS; i++) {
    //builtString += String((int)volumePotentiometers[i].getRawValue()); // raw
    builtString += String((int)volumePotentiometers[i].getValue()); // filtered
    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  Serial.println(builtString);
}
