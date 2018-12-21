# GoMoDo

Sometimes it pays to be picky.
A hybrid between noise gating and granular processing.

## Description

"With controls that should be familiar to most noise gate users, GoMoDo allows you to be more selective with your grain production. Instead of processing everything that comes down the pipeline, the user can specify how loud the incoming sound must be before it will be added to the sampling buffer. Grain production continues even when the buffer is not being updated, the processor just keeps using the last bit of sound that was over the threshold. No more processing that background hiss, just give me the good stuff!" - *from the original Hipno documentation* 

GoMoDo was originally part of the Hipno plug-in package distributed by Cycling'74. While this 2.0 version is not an exact replica of the original, it resurrects many of the key features for use in Ableton Live and Cycling'74 Max.

## Parameters

### Buffer State

Typically, this device is used to 'stream' new audio from the track into a sampling buffer for grain production. This toggle allows you to temporarily 'freeze' the contents of the sampling buffer.

### Buffer Flux

This introduces random variations into where the device reads from the sampling buffer. At low values, it will have fewer variations and sound more stable. At high values, it will fluctuate between old and new audio in the sampling buffer.

### Threshold

Sets the threshold level used to automatically select incoming audio for the sampling buffer. Depending on the Threshold Mode, either audio above or below the threshold is added to the sampling buffer. This allows dynamics to control when audio content is added to the sampling buffer, sort of like a Gate.

### Threshold Mode

When Threshold Mode is set to 'Above' and Threshold is set to its minimum value, all incoming audio is added to the sampling buffer. Raising Threshold to a higher level will cause only audio that exceeds the threshold to be added to the sampling buffer. Changing the Threshold Mode to 'Below' reverses this function, so that only audio under the threshold will be added to the sampling buffer.

### Threshold Indicator 

Flashes whenever audio is being added to the sampling buffer.

### Randomize Periods

When set to Off, only the Period1 value is used. When set to On, the device introduces random variations that will fluctuate between Period1 and Period2. It does not matter which of these values is minimum or maximum, as the device automatically sorts them into the necessary order.

### Randomize Durations

When set to Off, only the Duration1 value is used. When set to On, the device introduces random variations that will fluctuate between Duration1 and Duration2. It does not matter which of these values is minimum or maximum, as the device automatically sorts them into the necessary order.

### Randomize Pitch Shifts

When set to Off, only the PitchShift1 value is used. When set to On, the device introduces random variations that will fluctuate between PitchShift1 and PitchShift2. It does not matter which of these values is minimum or maximum, as the device automatically sorts them into the necessary order.

### Randomize Gains

When set to Off, only the Gain1 value is used. When set to On, the device introduces random variations that will fluctuate between Gain1 and Gain2. It does not matter which of these values is minimum or maximum, as the device automatically sorts them into the necessary order.

### Period1

Period is the time in milliseconds between the start of consecutive grains. At low values, grains are produced more rapidly which results in higher densities. At higher values, grains are produced less frequently which results in more audible pulsing.

### Period2

Period is the time in milliseconds between the start of consecutive grains. When both Period1 and Period2 are active, they constrain the random variations in Period between consecutive grains.

### Duration1

Duration is the time in milliseconds between the start and end of an individual grain. When Duration is lower than Period, it produces gaps between consecutive grains. When Duration is higher than Period, there will be overlap between consecutive grains. These interactions between Duration and Period produce complex timbre effects.

### Duration2

Duration is the time in milliseconds between the start and end of an individual grain. When both Duration1 and Duration2 are active, they constrain the random variations in Duration between consecutive grains.

### PitchShift1

Pitch Shift transposes the audio for individual grains while reading from the sampling buffer. When Pitch Shift is 0, there should be no audible change in the source pitch. Values are expressed in cents, so that -100 cents equals a half step lower and +100 cents equals a half step higher. 

### PitchShift2

Pitch Shift transposes the audio for individual grains while reading from the sampling buffer. When both PitchShift1 and PitchShift2 are active, they constrain the random variations in Pitch Shift between consecutive grains.

### Gain1

Gain will boost or attenuate the audio for individual grains while reading from the sampling buffer. When Gain is 0 dB, there should be no audible change in the source gain.

### Gain2

Gain will boost or attenuate the audio for individual grains while reading from the sampling buffer. When both Gain1 and Gain2 are active, they constrain the random variations in Gain between consecutive grains.

### Dry/Wet

This adjusts the balance between unprocessed (dry) and processed (wet) audio. When set to 100%, only the granular sampling will be heard. When set to 0%, only the original input audio will be heard. 

## About the Author

Nathan Wolek is an audio artist and researcher whose work encompasses advanced signal processing techniques, multimedia performance, and electronic music history. Wolek completed his Ph.D. in Music Technology at Northwestern University, and is currently Professor of Digital Arts at Stetson University in DeLand, FL. He is known primarily for the Granular Toolkit and LowkeyNW package, both popular extensions to Cycling74's Max. 


