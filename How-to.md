
# MIX5R MIDI Configuration

How to customize and flash the MIX5R firmware. These instructions are for custom Seeeduino XIAO based mixer from here: <https://www.etsy.com/shop/ReturnToParadise>. Note that the base Deej mixer is different and incompatible!

## Part 1: Tools and Files Setup

1. Download Microsoft Visual Studio Code (VS Code) <https://code.visualstudio.com/Download>
2. Install the PlatformIO extension within VS Code. See instructions here:
<https://platformio.org/install/ide?install=vscode>
3. Download the MIDI code from GitHub:
<https://github.com/numanair/nano-control-surface/tree/seeeduino-xiao-composer>  
**NOTE:** there are multiple branches on this GitHub. If the file names do not match in the next steps make sure you downloaded from the correct *seeeduino-xiao-composer* branch.
4. Click the green Code button to find the ZIP download option.  Extract the downloaded ZIP to a project folder of your choosing.
5. In VS Code, click the house icon in the bottom toolbar to open the PlatformIO home.
6. Click *Projects* on the left panel of the PlatformIO home. Click the *Add Existing* button and navigate to the extracted folder.
7. Open the newly added project after it finishes loading it.
8. To test compilation, click the build button in the bottom toolbar (checkmark icon). A terminal will open showing compilation status. This will be slowest the first time it compiles.

## Part 2: Modifying the Code

1. In VS Code, use the *Explorer* tab of the leftmost activity bar to open `src/xiao_control_surface.cpp`. This file is where the project’s code lives.
2. For the purposes of this guide we will only be modifying one part of this file. Find the block of code that looks like so:  

```CPP
CCPotentiometer volumePotentiometers[] = {
  {analogInputs[0], {0x07, CHANNEL_1}},
  {analogInputs[1], {0x14, CHANNEL_1}},
  {analogInputs[2], {0x21, CHANNEL_1}},
  {analogInputs[3], {0x11, CHANNEL_1}},
  {analogInputs[4], {0x01, CHANNEL_1}},
};
```

As you can see, there are 5 lines that look nearly identical. In each of these we only want to modify two parts that are within the `{MIDI_CC, CHANNEL_#}` element.

### Setting the MIDI CC

The MIDI CC can either be set to a name, such as `Channel_Volume`, or directly to a CC number. When setting a name, make sure to include `MIDI_CC::` before it. Example:

```CPP
CCPotentiometer volumePotentiometers[] = {
  {analogInputs[0], {MIDI_CC::Channel_Volume, CHANNEL_1}},
  {analogInputs[1], {MIDI_CC::Channel_Volume, CHANNEL_2}},
  {analogInputs[2], {MIDI_CC::Channel_Volume, CHANNEL_3}},
  {analogInputs[3], {MIDI_CC::Channel_Volume, CHANNEL_4}},
  {analogInputs[4], {MIDI_CC::Channel_Volume, CHANNEL_5}},
};
```

The list of names is here: <https://tttapa.github.io/Control-Surface-doc/Doxygen/d4/dbe/namespaceMIDI__CC.html>

Sometimes it is easier to set the CC number directly. The CC numbers are hex. To check available hex values for a CC number look at the table here: <https://www.midi.org/specifications-old/item/table-3-control-change-messages-data-bytes-2> So MIDI CC `14` becomes `0x0E`. This allows for setting unnamed CC's.

### Setting the MIDI Channel

Channel configuration is pretty straight forward. Set the number at the end of the line to the desired channel.

## Compiling & Uploading

1. After modifying the MIDI parameters, it is time to test out compilation. Click the checkmark icon in the bottom toolbar. If there are any errors, review what they are and make any fixes before compiling again.
2. Time to flash the XIAO control board! With the mixer plugged in, click the arrow next to the checkmark to begin compiling and uploading. A window with 3 files might open while the code is uploading (these are just board version info). Give it a minute and it will disappear when done.
3. Once the board reboots it will reconnect running the new code. Try it out! Depending on your software there may be a step to reconnect or redetect the device.
4. If something went wrong with the previous upload it could interfere with the flashing process. If the COM port for the MIX5R is no longer showing in Device Manager this may be the issue. Fortunately the XIAO is easy to wipe back to its factory state with the reset button on the back of the MIX5R. With the device connected to the computer, use a pin to click the reset button twice. Give the board a moment to boot and then try flashing the corrected code. **NOTE: this wipes any existing code from the MIX5R.**
