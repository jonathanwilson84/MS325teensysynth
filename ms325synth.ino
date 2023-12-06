#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>


#define POLY_COUNT 7

struct waveform_modulator {
  AudioSynthWaveformModulated waveform1;
  AudioEffectEnvelope envelope1;
  int note;
  boolean in_use;
  unsigned long note_start_time;
};

waveform_modulator poly_waveforms[POLY_COUNT];


AudioEffectFreeverb       freeverb1;
AudioEffectDelay          delay1;  
AudioSynthWaveform        lfo;
AudioSynthWaveform        lfo2;
AudioSynthWaveform        lfo3;
AudioMixer4               mixer1;
AudioMixer4               mixer2;
AudioMixer4               mixer3;
AudioMixer4               mixer4;
AudioMixer4               mixer5;                
AudioFilterStateVariable filter1;
AudioOutputI2S2           i2s1;
AudioEffectEnvelope      envelope2;
AudioSynthWaveformModulated unison1;
AudioSynthWaveformModulated unison2;
AudioSynthWaveformModulated unison3;
AudioSynthWaveformModulated unison4;
AudioAnalyzeFFT256       fft256_1;   

AudioConnection patchCord20(lfo, 0, filter1, 1);
AudioConnection patchCord1(poly_waveforms[0].waveform1, 0, poly_waveforms[0].envelope1, 0);
AudioConnection patchCord2(poly_waveforms[1].waveform1, 0, poly_waveforms[1].envelope1, 0);
AudioConnection patchCord3(poly_waveforms[2].waveform1, 0, poly_waveforms[2].envelope1, 0);
AudioConnection patchCord4(poly_waveforms[3].waveform1, 0, poly_waveforms[3].envelope1, 0);
AudioConnection patchCord5(poly_waveforms[4].waveform1, 0, poly_waveforms[4].envelope1, 0);
AudioConnection patchCord6(poly_waveforms[5].waveform1, 0, poly_waveforms[5].envelope1, 0);
AudioConnection patchCord7(poly_waveforms[6].waveform1, 0, poly_waveforms[6].envelope1, 0);

AudioConnection patchCord8(poly_waveforms[0].envelope1, 0, mixer1, 0);
AudioConnection patchCord9(poly_waveforms[1].envelope1, 0, mixer1, 1);
AudioConnection patchCord10(poly_waveforms[2].envelope1, 0, mixer1, 2);
AudioConnection patchCord11(poly_waveforms[3].envelope1, 0, mixer1, 3);
AudioConnection patchCord12(poly_waveforms[4].envelope1, 0, mixer2, 0);
AudioConnection patchCord13(poly_waveforms[5].envelope1, 0, mixer2, 1);
AudioConnection patchCord14(poly_waveforms[6].envelope1, 0, mixer2, 2);

AudioConnection patchCord15(mixer5, 0, filter1, 0);
AudioConnection patchCord16(filter1, 0, i2s1, 0);
AudioConnection patchCord17(filter1, 0, i2s1, 1);
AudioConnection patchCord38(mixer5, fft256_1);
AudioConnection patchCord18(mixer1, 0, mixer5, 0);
AudioConnection patchCord19(mixer2, 0, mixer5, 1);

AudioConnection patchCord21(envelope2, 0, poly_waveforms[0].waveform1, 0);
AudioConnection patchCord22(envelope2, 0, poly_waveforms[1].waveform1, 0);
AudioConnection patchCord23(envelope2, 0, poly_waveforms[2].waveform1, 0);
AudioConnection patchCord24(envelope2, 0, poly_waveforms[3].waveform1, 0);
AudioConnection patchCord25(envelope2, 0, poly_waveforms[4].waveform1, 0);
AudioConnection patchCord26(envelope2, 0, poly_waveforms[5].waveform1, 0);
AudioConnection patchCord27(envelope2, 0, poly_waveforms[6].waveform1, 0);

AudioConnection patchCord37(lfo2, 0, envelope2, 0);

AudioConnection patchCord33(unison1, 0, mixer4, 0);
AudioConnection patchCord34(unison2, 0, mixer4, 1);
AudioConnection patchCord35(unison3, 0, mixer4, 2);
AudioConnection patchCord36(unison4, 0, mixer4, 3);

AudioControlSGTL5000 sgtl5000_1;





//LEDS

int ledPower = 34;
int led1 = 32;

int led2 = 37;
int led3 = 36;
int led4 = 35; 



const int button1 = 6;
bool buttonPressed = false;
bool frequencyModOn = false;
int freqButtonState = 0;
int prevFreqButtonState = LOW;
float phaseAmountMapped = 0;
float freqAmountMapped = 0;  



//VOLUME

int volumePin = A9;

//fm envelope

int fmAttackPin = A16;
int fmDecayPin = A8;
int fmSustainPin = A17;
//PHASE MOD
int speed = 60;
int potPhaseAmplitude = A5;
int potPhaseFreq = A3;
int potPhaseAmount = A0;
int potFreqAmount = A15;


//LFO
int potAmplitudePin = A4;  
int potFrequencyPin = A1; 

//WAVEFORM BUTTON


int waveformButtonLFO = 30;
int waveformButtonLFO2 = 9;

bool onAndOffLFO = false;
bool onAndOffLFO2 = false;
int buttonStateLfo = 0;
int buttonStateLfo2 = 0;




int currentLFOWaveformIndex = 0;
int currentLFO2WaveformIndex = 0;


const int waveformbutton = 5;
bool onAndOff = false;
int currentWaveformIndex = 0;
int buttonState = 0;
const int numWaveforms = 6;




const int availableWaveforms[numWaveforms] = {
 
  WAVEFORM_SINE,
  WAVEFORM_TRIANGLE,
  WAVEFORM_SAWTOOTH,
  WAVEFORM_SAWTOOTH_REVERSE,
  WAVEFORM_PULSE,
  WAVEFORM_SQUARE


};

//Envelope1
int potAttackPin = A2;
int potDecayPin = A13;
int potSustainPin = A12;
int potReleasePin = A10;

//FILTER1
int potCutoffPin = A11;
int potResonancePin = A14;




MIDI_CREATE_DEFAULT_INSTANCE();

void OnNoteOn(byte channel, byte note, byte velocity);
void OnNoteOff(byte channel, byte note, byte velocity);








float midiNoteToFrequency(int note) {
  return 440.0 * pow(2.0, (note - 69.0) / 12.0);
}

void setup() {

Serial.begin(9600);

  AudioMemory(18);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  MIDI.begin(MIDI_CHANNEL_OMNI);


usbMIDI.setHandleNoteOn(OnNoteOn);
	usbMIDI.setHandleNoteOff(OnNoteOff);

pinMode(ledPower, OUTPUT);
  digitalWrite(ledPower, HIGH);
pinMode(led1, OUTPUT);
pinMode(led1, HIGH);
pinMode(led2, OUTPUT);
pinMode(led3, OUTPUT);
pinMode(led4, OUTPUT);



 frequencyModOn = false;

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);

  pinMode(button1, INPUT_PULLUP); 

//WAVEFORM SETUP


pinMode(waveformButtonLFO2, INPUT_PULLUP); 
pinMode(waveformButtonLFO, INPUT_PULLUP); 
  pinMode(waveformbutton, INPUT_PULLUP); 
  for (int i = 0; i < POLY_COUNT; i++) {
    poly_waveforms[i].waveform1.begin(availableWaveforms[currentWaveformIndex]);

    poly_waveforms[i].waveform1.amplitude(0.5);

 
    poly_waveforms[i].note = -1; 
    poly_waveforms[i].in_use = false;


    poly_waveforms[i].envelope1.delay(0);      
    poly_waveforms[i].envelope1.attack(15);    
    poly_waveforms[i].envelope1.hold(2.5);     
    poly_waveforms[i].envelope1.decay(127);    
    poly_waveforms[i].envelope1.sustain(0.5);  
    poly_waveforms[i].envelope1.release(63);   
  }




//volume

pinMode(volumePin, INPUT);

// FM ENVELOPE

pinMode(fmAttackPin, INPUT);
pinMode(fmDecayPin, INPUT);
pinMode(fmSustainPin, INPUT);



envelope2.attack(15);
envelope2.decay(100);
envelope2.sustain(0);
envelope2.release(20);

  //MOD

  pinMode(potPhaseAmplitude, INPUT);
 pinMode(potPhaseAmount, INPUT);
 pinMode(potPhaseFreq, INPUT);
pinMode(potFreqAmount, INPUT);

  //LFO filter
  pinMode(potAmplitudePin, INPUT);
  pinMode(potFrequencyPin, INPUT);


  //envelope
  pinMode(potAttackPin, INPUT);
  pinMode(potDecayPin, INPUT);
  pinMode(potSustainPin, INPUT);
  pinMode(potReleasePin, INPUT);


  //FILTER
  pinMode(potCutoffPin, INPUT);
  pinMode(potResonancePin, INPUT);
  pinMode(potPhaseAmount, INPUT);



  //LFO TO FILTER
 // lfo.amplitude(.5);
  //lfo.frequency(10);
 lfo.begin(availableWaveforms[currentLFOWaveformIndex]);


  //LFO2 - PHASEMOD
  lfo2.amplitude(.95);
   lfo2.frequency(440);
 lfo2.begin(WAVEFORM_SINE);

  //FILTER SETUP
  filter1.frequency(800.0); 
  filter1.resonance(0.7);   





  //MIXER STUFF
  mixer1.gain(0, .2);
  mixer1.gain(1, .2);
  mixer1.gain(2, .2);
  mixer1.gain(3, .2);

  mixer2.gain(0, .2);
  mixer2.gain(1, .2);
  mixer2.gain(2, .2);
  
  mixer4.gain(0, 0);
  mixer4.gain(1, 0);
  mixer4.gain(2, 0);
  mixer4.gain(3, 0);

  mixer5.gain(0, .4);
  mixer5.gain(1, .4);
}



void loop() {
  MIDI.read();
usbMIDI.read();




      

 freqButtonState = digitalRead(button1);

  if (freqButtonState != prevFreqButtonState) {
    // Button state has changed
    if (freqButtonState == HIGH) {
      // Button went from LOW to HIGH, switch back to phase modulation
      frequencyModOn = !frequencyModOn;  // Toggle frequency modulation
     
      // Clear modulation settings for all waveforms
      for (int i = 0; i < POLY_COUNT; i++) {
        poly_waveforms[i].waveform1.frequencyModulation(0);
        poly_waveforms[i].waveform1.phaseModulation(0);
      }
    }
  }

  // Update previous button state
  prevFreqButtonState = freqButtonState;

  // Apply modulation parameters directly within the loop based on the mode
  if (frequencyModOn) {
    // Apply parameters for frequency modulation
    int freqAmount = analogRead(potFreqAmount);
    float freqAmountMapped = map(freqAmount, 0, 1023, 8, 0);
   
    for (int i = 0; i < POLY_COUNT; i++) {
      poly_waveforms[i].waveform1.frequencyModulation(freqAmountMapped);
    }
  } else {
    // Apply parameters for phase modulation
    int phaseAmount = analogRead(potPhaseAmount);
    float phaseAmountMapped = map(phaseAmount, 0, 1023, 800, 0);
   

    for (int i = 0; i < POLY_COUNT; i++) {
      poly_waveforms[i].waveform1.phaseModulation(phaseAmountMapped);
    }
  }



  buttonState = digitalRead(waveformbutton);
  if (buttonState == HIGH) {
    if (!onAndOff) {
      onAndOff = true;

      // Move to the next waveform
      currentWaveformIndex = (currentWaveformIndex + 1) % numWaveforms;

      // Change the waveform based on the current index
      for (int i = 0; i < POLY_COUNT; i++) {
        poly_waveforms[i].waveform1.begin(availableWaveforms[currentWaveformIndex]);
      }
    }
  } else {
    onAndOff = false;
  }

  for (int i = 0; i < POLY_COUNT; i++) {
    // check any slot that shows it is "in use"
    if (poly_waveforms[i].in_use == true) {
      // check to see if the envelope in any slot has completed - if so, then free up the slot
      if (poly_waveforms[i].envelope1.isActive() == false) {
        // clear out this slot & make it available for use once again
        poly_waveforms[i].note = -1;
        poly_waveforms[i].in_use = false;
      }
    }
  }


buttonStateLfo = digitalRead(waveformButtonLFO);
if (buttonStateLfo == HIGH) {
    if (!onAndOffLFO) {
        onAndOffLFO = true;

        // Move to the next LFO waveform
        currentLFOWaveformIndex = (currentLFOWaveformIndex + 1) % numWaveforms;

        // Change the LFO waveform based on the current index
        lfo.begin(availableWaveforms[currentLFOWaveformIndex]);

      
    }
} else {
    onAndOffLFO = false;
}

      buttonStateLfo2 = digitalRead(waveformButtonLFO2);
if (buttonStateLfo2 == HIGH) {
    if (!onAndOffLFO2) {
        onAndOffLFO2 = true;

        // Move to the next LFO waveform
        currentLFO2WaveformIndex = (currentLFO2WaveformIndex + 1) % numWaveforms;

        // Change the LFO waveform based on the current index
        lfo2.begin(availableWaveforms[currentLFO2WaveformIndex]);

     
    }
} else {
    onAndOffLFO2 = false;
} 

  
//VOLUME

int volume = analogRead(volumePin);
float volumeMapped = map(volume, 0, 1023, 100, 0);


 mixer5.gain(0, volumeMapped/100);
  mixer5.gain(1, volumeMapped/100);



  // ENVELOPE1

  int attackValue = analogRead(potAttackPin);
   int decayValue = analogRead(potDecayPin);
  int sustainValue = analogRead(potSustainPin);
  int releaseValue = analogRead(potReleasePin);


  float attackMapped = map(attackValue, 0, 1023, 500, 25);
  float decayMapped = map(decayValue, 0, 1023, 500, 25);
  float sustainMapped = map(sustainValue, 0, 1023, 100, 0);
  float releaseMapped = map(releaseValue, 0, 1023, 2000, 20); 


  for (int i = 0; i < POLY_COUNT; i++) {
    // set the envelope "attack" parameter
    poly_waveforms[i].envelope1.attack(attackMapped);
    poly_waveforms[i].envelope1.decay(decayMapped);
    poly_waveforms[i].envelope1.sustain(sustainMapped / 100);
    poly_waveforms[i].envelope1.release(releaseMapped);
  }


//FM ENVELOPE

int fmAttack = analogRead(fmAttackPin);
int fmDecay = analogRead(fmDecayPin);

int fmSustain = analogRead(fmSustainPin);



float fmAttackMapped = map(fmAttack, 0, 1023, 250, 15);
  float fmDecayMapped = map(fmDecay, 0, 1023, 2000, 25);
float fmSustainMapped = map(fmSustain, 0, 1023, 100, 0);

  envelope2.attack(fmAttackMapped);
  envelope2.decay(fmDecayMapped);
envelope2.sustain(fmSustainMapped / 100);



  // FILTER

  int cutoffValue = analogRead(potCutoffPin);
  int resonanceValue = analogRead(potResonancePin);


  float cutoffMapped = map(cutoffValue, 0, 1023, 8000, 20);
  float resonanceMapped = map(resonanceValue, 0, 1023, 6, 2);

  filter1.frequency(cutoffMapped);
  filter1.resonance(resonanceMapped);

  // LFO
int lfoSpeed = analogRead(potFrequencyPin);
 int lfoAmount = analogRead(potAmplitudePin);

  float lfoSpeedMapped = map(lfoSpeed, 0, 1023, 40, .4);
  float lfoAmplitudeMapped = map(lfoAmount, 0, 1023, 100, 0);



 lfo.amplitude(lfoAmplitudeMapped / 100);
  lfo.frequency(lfoSpeedMapped);




  // MOD
  int FmAmount = analogRead(potPhaseAmplitude);
 

  float FmAmountMapped = map(FmAmount, 0, 1023, 100, 0);
 



lfo2.amplitude(FmAmountMapped / 100);
//lfo2.frequency(modRatioMapped);

/*
int modRatioPotValue = analogRead(potPhaseFreq);
if (modRatioPotValue < 150) {
  modRatio = 2.0;      
} else if (modRatioPotValue < 300) {
  modRatio = 4.0;     
} else if (modRatioPotValue < 450) {
  modRatio = 6.0;    
} else if (modRatioPotValue < 600) {
  modRatio = 8.0;    
 } else if (modRatioPotValue < 750) {
  modRatio = .25;     
 } else if (modRatioPotValue < 900) {
  modRatio = .5;   
 }


*/


if (Serial.available()) {
    char c = Serial.read();
    if ((c == 'r' || c == 'R')) {

      fft256_1.processorUsageMaxReset();
      AudioProcessorUsageMaxReset();
      AudioMemoryUsageMaxReset();
      Serial.println("Reset all max numbers");
    }
    if ((c == 'f' || c == 'F') && speed > 16) {
      speed = speed - 2;
    }
    if ((c == 's' || c == 'S') && speed < 250) {
      speed = speed + 2;
    }
  }

  // print a summary of the current & maximum usage
  Serial.print("CPU: ");
 
  Serial.print("fft=");
  Serial.print(fft256_1.processorUsage());
  Serial.print(",");
  Serial.print(fft256_1.processorUsageMax());
  Serial.print("  ");
  Serial.print("all=");
  Serial.print(AudioProcessorUsage());
  Serial.print(",");
  Serial.print(AudioProcessorUsageMax());
  Serial.print("    ");
  Serial.print("Memory: ");
  Serial.print(AudioMemoryUsage());
  Serial.print(",");
  Serial.print(AudioMemoryUsageMax());
  Serial.print("    ");
  Serial.print("Send: (R)eset, (S)lower, (F)aster");
  Serial.println();

}



void OnNoteOn(byte channel, byte note, byte velocity) {



if (velocity > 40) {
 digitalWrite(led1, HIGH);
   digitalWrite(led2, HIGH);
 digitalWrite(led3, HIGH);
}

  if (velocity > 105) {
 digitalWrite(led1, HIGH);
   digitalWrite(led2, HIGH);
   digitalWrite(led3, HIGH);
digitalWrite(led4, HIGH);
  }


  int freq = (int)(440.0 * pow(2.0, ((float)note - 69.0) / 12.0));
  int poly_index = -1;
  unsigned long oldest_note_start_time = millis();

envelope2.noteOn();


 int modRatio = analogRead(potPhaseFreq);
 float modRatioMapped = map(modRatio, 0, 1023, 8, 1);
lfo2.frequency(freq * modRatioMapped);




  // Find the oldest note
  for (int i = 0; i < POLY_COUNT; i++) {
    if (!poly_waveforms[i].in_use) {
      // Found an empty slot, use it
      poly_index = i;
      break;
    } else if (poly_waveforms[i].note_start_time < oldest_note_start_time) {
      oldest_note_start_time = poly_waveforms[i].note_start_time;
      poly_index = i;
    }
  }

  if (poly_index != -1) {

    poly_waveforms[poly_index].envelope1.noteOff();
  }

  // Set the slot for the new note
  poly_waveforms[poly_index].in_use = true;
  poly_waveforms[poly_index].note = note;
  poly_waveforms[poly_index].note_start_time = millis(); 
  poly_waveforms[poly_index].waveform1.frequency(freq);
  poly_waveforms[poly_index].waveform1.amplitude((float)velocity / 255.0);
  poly_waveforms[poly_index].envelope1.noteOn();
}

void OnNoteOff(byte channel, byte note, byte velocity) {



  int poly_index = -1;  

  for (int i = 0; i < POLY_COUNT; i++) { 
   
    if (poly_waveforms[i].note == note && poly_waveforms[i].in_use) {
    
      poly_index = i;
      break;
    }
  }


  if (poly_index != -1) {
   
    poly_waveforms[poly_index].envelope1.noteOff();

   
    poly_waveforms[poly_index].in_use = false;
  }

 // Turn off all LEDs when a note is released
 
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}



