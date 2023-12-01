# MS325teensysynth
synthesizer built for teensy 4.1 using the teensy audio library


Teensy 4.1 based synthesizer with 7 voice polyphony
Uses the teensy audio library


CURRENT features

- 7 voice polyphony
- waveform selection with all tradional waveforms
- lfo controlling filter cutoff
- phasemod with amount, frequency, and amplitude controls
- full ADSR envelope
- reverb
- i2s dac output
- Midi input via trs jack. You must built a proper midi input circuit for this feature!  Pictures to come soon. 

  
FUTURE features

- Unison mode - monophonic playback with multiple voices and detune control
- FM synth mode - need to figure this out
- usb midi - should be easy to add, I have no interest since I would rather just use trs midi. 

- Currently, this synth is pretty all over the place as far as pin assignments, but they are all in the code. What is really messed up is that I wired my pots backwards, and need to reverse all the mappings to compensate. This will be added to the code as an option, incase pots get switched around, or I make v2 with the correct wiring.

- Physical build features 16 pots, since my DAC takes up 2 analog inputs (annoyingly). I think I will eventually reassign those to the other available pins to free up 2 more analog inputs, but for my current build, 16 pots is good. That will be V2.

- I am definitely not a software engineer, and have basic coding skills at best. There are probably better ways to do a lot of the things I have implemented here, and I am working to learn more to improve things as I go. Overall, 
