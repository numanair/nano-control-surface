#include <Arduino.h>
#include <Control_Surface.h>

// Number of potentiometers or faders
const int NUM_SLIDERS = 5;

// Potentiometer pins assignment
pin_t analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A4};

// Adjusts linearity correction for my specific potentiometers.
// 1 = fully linear but jittery. 0.7 is about max for no jitter.
const float correctionMultiplier = 0.70;

// measured output every equal 5mm increment in 14-bit. Minimum and maximum values are not affected by correctionMultiplier.
float measuredInput[] = {55, 200,  660,  1650, 3628, 5800, 7900, 10180, 12380, 14580, 15690, 16120, 16350};

// MIDI over USB :)
USBMIDI_Interface midi;

// 7, 14, 21, 11, 1

// Potentiometer array only sends MIDI
// messages when a value changes
// https://www.midi.org/specifications-old/item/table-3-control-change-messages-data-bytes-2
// Set MIDI CC here
CCPotentiometer volumePotentiometers[] = {
  {analogInputs[0], {0x07, CHANNEL_1}},
  {analogInputs[1], {0x0E, CHANNEL_1}},
  {analogInputs[2], {0x15, CHANNEL_1}},
  {analogInputs[3], {0x0B, CHANNEL_1}},
  {analogInputs[4], {0x01, CHANNEL_1}},
};

// number of elements in the MultiMap arrays
const int arrayQty = sizeof(measuredInput) / sizeof(measuredInput[0]);
float adjustedinputval[arrayQty] = {0};

// Probably no need to change these calculated values
float idealOutputValues[arrayQty] = {0,  1365, 2730, 4096, 5461, 6826, 8192, 9557,  10922, 12288, 13653, 15018, 16383};
// Note: 16383 = 2¹⁴ - 1 (the maximum value that can be represented by
// a 14-bit unsigned number

// make sure Control Surface is using 12-bit ADC 
constexpr uint8_t ADC_BITS = 12;

// milliseconds between sending serial data
Timer<millis> timer = 10;

// note: the _in array must have increasing values
// same function as MultiMap Arduino library
float multiMap(float val, float* _in, float* _out, uint8_t size) {
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
    float mapped = multiMap(raw, adjustedinputval, idealOutputValues, arrayQty);
    // adjusts slider output for better linearity
    return mapped;
}

int analogSliderValues[NUM_SLIDERS];

void sendSliderValues();
void updateSliderValues();

// Initialize the Control Surface
void setup() {
  for (auto &volumePotentiometers : volumePotentiometers)
    volumePotentiometers.map(mappingFunction);
  analogReadResolution(12);
  Control_Surface.begin();

  // multiplier correction
  for (size_t i = 0; i < arrayQty; i++) {
    adjustedinputval[i] = round(idealOutputValues[i] + (measuredInput[i] - idealOutputValues[i]) * correctionMultiplier);
    // theoretical ideal + (measured - theoretical) * multi
  }
  adjustedinputval[0] = measuredInput[0]; // min value
  adjustedinputval[arrayQty] = measuredInput[arrayQty]; // max value

  // Start serial for Deej
  Serial.begin(9600);
}

// Update the Control Surface
void loop() {
  Control_Surface.loop();
  
  // Deej loop
  if (timer)
  {
    updateSliderValues();
    sendSliderValues();
  }
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    analogSliderValues[i] = round((multiMap(analogRead(analogInputs[i]) * 4, adjustedinputval, idealOutputValues, arrayQty))) / 16;
    //analogSliderValues[i] = analogRead(analogInputs[i]); //unfiltered output
  }
}

// Deej Serial Support
void sendSliderValues() {
  String builtString = String("");
  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)round(analogSliderValues[i])); // raw (Deej software does the filtering)
    // builtString += String((int)round(volumePotentiometers[i].getRawValue() / 16)); // raw (Deej software does the filtering)
    //builtString += String((int)volumePotentiometers[i].getValue()); // filtered
    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  Serial.println(builtString);
}
