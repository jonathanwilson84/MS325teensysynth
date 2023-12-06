# MS325teensysynth
synthesizer built for teensy 4.1 using the teensy audio library
<img width="1126" alt="ms325 main" src="https://github.com/jonathanwilson84/MS325teensysynth/assets/64935938/a10d36cc-4549-4a64-baed-18daca28c470">

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


Build guide

For those brave enough to attempt making something like this, I will include some documentation here for now. I will eventually have a full walkthrough on my site at mountsquishy.com

This is a pretty basic build, and you won't need many unusual parts. 

Parts List

Teensy 4.1 microcontroller
16 10k potentiometers - I prefer the ones with little loops instead of just straight pins. They are easier to solder.
6 220 resistors
1 270 resistor
1 6N138 optoisolator
IN4148 diode
2 stereo 3.5mm input jacks - one for audio, one for midi
soldering iron

For the sake of simplicity, I will assume that this will make sense. If you have never done any electronics projects before, this wouldn't be the worst place to start, but there is a lot of pots. 

For my build, I used a sheet of 1/8 inch acrylic, that I laser cut with all of my openings for pots, buttons and leds. You could definitely forgo the laser cutter, and use any material you like. 1/8 or 1/16 inch birch plywood would work well, and be much easier to drill through than acrylic. The hole sizes are going to depend on what hardware you end up using. Check the data sheet to find the shaft width, and probably check the shaft height before ordering to make sure it goes all the way through whatever you are using for the faceplate. Most pots are fine, but 3.5mm jacks often has frustratingly short attachment threads that only work on very thin mounting materials. 

For the first step, I basically just daisychain all of the grounds, and all of the powers with each other. 

<img width="1126" alt="rear view wiring 1" src="https://github.com/jonathanwilson84/MS325teensysynth/assets/64935938/c0077064-e7ed-4d0e-bd91-f749c4118d29">

The red wire represents the power, and white represents ground. Leave enough wire at the end to attach to your board, but probably at least 10 cm. Once you have these all wired, and connected to the 5v and ground on your Teensy, it is time to start connecting our actual pins to our hardware. This is fairly straight forward, although, I will say my assignments don't really make any sense, so pay extra attention and make sure you look at a Teensy 4.1 pinout diagram to make sure you are connecting the correct pins!

<img width="1126" alt="rear with pin assignments" src="https://github.com/jonathanwilson84/MS325teensysynth/assets/64935938/b59adbd2-00b6-4b2d-8684-9f583e8a8b33">

Now, there are 2 more things to do. First, we will connect the DAC to the teensy.

![image](https://github.com/jonathanwilson84/MS325teensysynth/assets/64935938/cf6d5046-9c17-4755-a43a-5e6fe1099e97)

I am using a PCM5102A dac. These are cheap and easy to find, and work great. There is a consideration in this case though, because the default i2s output in the Teensy library uses 2 analog pins, one of which we want to be able to use.  This is why we use AudioOutputI2S2, instead of AudioOutputI2S, because it uses the alternative set of i2s pins on the teensy, that happen to be digital pins. So, usually, I would probably just use the regular output, but it is good to know we can free up 2 extra analog inputs by using the i2s2 output.
These are the pin assignments for the DAC to Teensy, remember we are you Teensy 4.1.

![Frame 1 (13)](https://github.com/jonathanwilson84/MS325teensysynth/assets/64935938/974e1868-5354-4fb9-a2fb-f3a6d3c6a20a)

The last main part is the MIDI in circuit. Unfortunately, you can't just attach your MIDI jack to RX and be ready to go. It needs an optoisolator. From what I understand, MIDI signals can get a little crazy, and need to be put in check, that is where the optoisolator comes in. I wish I had a more technical disussion on this, but you can go google it if you want to learn why this is necessary. My MIDI input circuit design comes from Blog Hoskins $20 arduino synth project. https://bloghoskins.blogspot.com/2020/11/20-synth-project-complete-build-guide.html  You can check it out if you want, it uses the mozzi audio library and was the first arduino synth project I ever made, so thanks to Blog Hoskins! I have been using their MIDI input circuit ever since and it works great. 

![midi IN circuit](https://github.com/jonathanwilson84/MS325teensysynth/assets/64935938/cde44329-634b-4abe-a29b-5ff6d3f9c5b4)

The most important thing to note in the diagram is the little circle at the top of the optoisolator. It needs to match this to work. The other consideration is the diode. It is directional, and needs to be facing the correct way, with black band at the top. For the sake of this diagram, we are assuming that the purple lines are attached via solder underneath. For the rest, probably makes sense to use solder to connect the optoisolator to the 2 resistors and diode using solder on the opposite side of perf board, and then you can connect wire with a bridge of solder as well. 

A NOTE ABOUT MIDI - You can use either a 5 pin midi connector for this, or a trs jack (stereo jack). Ive found that since the pins on a stereo jack are very hard to figure out which pin connects to which part of the connection, I just tend to test and figure it out.  So, generally, the sleeve is the ground, and that is usually pretty obvious. The other two pins will be l and r, or tip and sleeve. Ignore the ground, and just hook midi 1 wire to one pin, and midi 2 to the other. If that doesnt work, then you know it goes the other way!

I would recommend using a midi keyboard, attach the midi input to two pins, and see if it works. If not, switch them, test again, if it works, solder them in place. The most frustrating thing I have found is that different keyboards I have use different 3.5mm pin assignments. Yes, you heard that correct. So frustrating. I am working on designing a little dongle that flips the pin assignments for keyboards that don't work with my midi input.  YMMV. 

And that is basically it!  Using the Arduino IDE, make sure you have the Teensy audio library installed.  https://github.com/PaulStoffregen/Audio  There is a link, but you should be able to find it by searching in the arduino library section. Once installed, upload the sketch to your teensy and start messing around. This thing makes some really wild sounds!
