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
AudioOutputI2S           i2s1;
AudioEffectEnvelope      envelope2;
AudioSynthWaveformModulated unison1;
AudioSynthWaveformModulated unison2;
AudioSynthWaveformModulated unison3;
AudioSynthWaveformModulated unison4;


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
AudioConnection patchCord16(mixer3, 0, i2s1, 0);
AudioConnection patchCord17(mixer3, 0, i2s1, 1);
AudioConnection patchCord18(mixer1, 0, mixer5, 0);
AudioConnection patchCord19(mixer2, 0, mixer5, 1);

AudioConnection patchCord21(lfo2, 0, poly_waveforms[0].waveform1, 0);
AudioConnection patchCord22(lfo2, 0, poly_waveforms[1].waveform1, 0);
AudioConnection patchCord23(lfo2, 0, poly_waveforms[2].waveform1, 0);
AudioConnection patchCord24(lfo2, 0, poly_waveforms[3].waveform1, 0);
AudioConnection patchCord25(lfo2, 0, poly_waveforms[4].waveform1, 0);
AudioConnection patchCord26(lfo2, 0, poly_waveforms[5].waveform1, 0);
AudioConnection patchCord27(lfo2, 0, poly_waveforms[6].waveform1, 0);

AudioConnection patchCord28(filter1, 0, freeverb1, 0);
AudioConnection patchCord29(filter1, 0, mixer3, 1);
AudioConnection patchCord30(freeverb1, 0, mixer3, 0);

AudioConnection patchCord31(mixer4, 0, envelope2, 0);
AudioConnection patchCord31(envelope2, 0, filter1 0);
AudioConnection patchCord15(mixer5, 0, filter1, 0);
AudioConnection patchCord16(mixer3, 0, i2s1, 0);
AudioConnection patchCord17(mixer3, 0, i2s1, 1);
AudioConnection patchCord18(mixer1, 0, mixer5, 0);
AudioConnection patchCord19(mixer2, 0, mixer5, 1);
AudioConnection patchCord31(unison1, 0, mixer4, 0);
AudioConnection patchCord31(unison2, 0, mixer4, 1);
AudioConnection patchCord31(unison3, 0, mixer4, 2);
AudioConnection patchCord31(unison4, 0, mixer4, 3);

AudioControlSGTL5000 sgtl5000_1;



//PHASE MOD

int potPhaseAmplitude = A5;
int PotPhaseFreq = A3;
int potPhaseAmount = A0;

float modRatio = 1.0;

//LFO
int potAmplitudePin = A4;  
int potFrequencyPin = A1; 

//WAVEFORM BUTTON

const int waveformbutton = 5;
bool onAndOff = false;
int currentWaveformIndex = 0;
int buttonState = 0;
const int numWaveforms = 7;

const int availableWaveforms[numWaveforms] = {
  WAVEFORM_TRIANGLE,
  WAVEFORM_SAWTOOTH,
  WAVEFORM_SAWTOOTH_REVERSE,
  WAVEFORM_PULSE,
  WAVEFORM_SINE,
  WAVEFORM_SQUARE,
  WAVEFORM_TRIANGLE

};

//Envelope1
int potAttackPin = A2;
int potDecayPin = A13;
int potSustainPin = A12;
int potReleasePin = A10;

//FILTER1
int potCutoffPin = A11;
int potResonancePin = A14;

//REVERB

int reverbPin = A15;


MIDI_CREATE_DEFAULT_INSTANCE();

void OnNoteOn(byte channel, byte note, byte velocity);
void OnNoteOff(byte channel, byte note, byte velocity);


float midiNoteToFrequency(int note) {
  return 440.0 * pow(2.0, (note - 69.0) / 12.0);
}

void setup() {



  AudioMemory(100);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleNoteOff(OnNoteOff);



//WAVEFORM SETUP

  pinMode(waveformbutton, INPUT_PULLUP); 
  for (int i = 0; i < POLY_COUNT; i++) {
    poly_waveforms[i].waveform1.begin(availableWaveforms[currentWaveformIndex]);

    poly_waveforms[i].waveform1.amplitude(0.5);
    poly_waveforms[i].waveform1.phaseModulation(0);



    poly_waveforms[i].note = -1; 
    poly_waveforms[i].in_use = false;


    poly_waveforms[i].envelope1.delay(0);      
    poly_waveforms[i].envelope1.attack(15);    
    poly_waveforms[i].envelope1.hold(2.5);     
    poly_waveforms[i].envelope1.decay(127);    
    poly_waveforms[i].envelope1.sustain(0.5);  
    poly_waveforms[i].envelope1.release(63);   
  }

  //MOD

  pinMode(potPhaseAmplitude, INPUT);
 pinMode(potPhaseAmount, INPUT);
 pinMode(potPhaseFreq, INPUT);


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

//REVERB

pinMode(reverbPin, INPUT);


  //LFO TO FILTER
 // lfo.amplitude(.5);
  //lfo.frequency(10);
  lfo.begin(WAVEFORM_SINE);


  //LFO2 - PHASEMOD
  lfo2.amplitude(.95);
   lfo2.frequency(440);
 lfo2.begin(WAVEFORM_SINE);


  //FILTER SETUP
  filter1.frequency(800.0); 
  filter1.resonance(0.7);   


//REVERB SETUP
      freeverb1.roomsize(.7);
      freeverb1.damping(.1);


  //MIXER STUFF
  mixer1.gain(0, .2);
  mixer1.gain(1, .2);
  mixer1.gain(2, .2);
  mixer1.gain(3, .2);

  mixer2.gain(0, .2);
  mixer2.gain(1, .2);
  mixer2.gain(2, .2);

  mixer3.gain(0, 0.9);
  mixer3.gain(1, 0.1);
  
  mixer4.gain(0, 0);
  mixer4.gain(1, 0);
  mixer4.gain(2, 0);
  mixer4.gain(3, 0);



  mixer5.gain(0, .5);
  mixer5.gain(1, .5);
}



void loop() {
  MIDI.read();




//WAVEFORM BUTTON

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

  // ENVELOPE1

  int attackValue = analogRead(potAttackPin);
   int decayValue = analogRead(potDecayPin);
  int sustainValue = analogRead(potSustainPin);
  int releaseValue = analogRead(potReleasePin);


  float attackMapped = map(attackValue, 0, 1023, 25, 500);
  float decayMapped = map(decayValue, 0, 1023, 25, 500);
  float sustainMapped = map(sustainValue, 0, 1023, 0, 50);
  float releaseMapped = map(releaseValue, 0, 1023, 20, 5000); 


  for (int i = 0; i < POLY_COUNT; i++) {
    // set the envelope "attack" parameter
    poly_waveforms[i].envelope1.attack(attackMapped);
    poly_waveforms[i].envelope1.decay(decayMapped);
    poly_waveforms[i].envelope1.sustain(sustainMapped);
    poly_waveforms[i].envelope1.release(releaseMapped);
  }

  // FILTER

  int cutoffValue = analogRead(potCutoffPin);
  int resonanceValue = analogRead(potResonancePin);


  float cutoffMapped = map(cutoffValue, 0, 1023, 20, 8000);
  float resonanceMapped = map(resonanceValue, 0, 1023, 2, 6);

  filter1.frequency(cutoffMapped);
  filter1.resonance(resonanceMapped);

  // LFO

 int lfoSpeed = analogRead(potFrequencyPin);
int lfoAmount = analogRead(potAmplitudePin);
  float lfoSpeedMapped = map(lfoSpeed, 0, 1023, .4, 40);
  float lfoAmplitudeMapped = map(lfoAmount, 0, 1023, 0, 1);



Serial.println(lfoSpeed);
  lfo.amplitude(lfoAmplitudeMapped);
  lfo.frequency(lfoSpeedMapped);

  // MOD
  int FmAmount = analogRead(potPhaseAmplitude);
  //int FmFreq = analogRead(PotPhaseFreq);

  float FmAmountMapped = map(FmAmount, 0, 1023, 0, 1);
  //float FmFreqMapped = map(FmFreq, 0, 1023, 0, 1200);



lfo2.amplitude(FmAmountMapped);
//lfo2.frequency(FmFreqMapped);


int modRatioPotValue = analogRead(PotPhaseFreq);
if (modRatioPotValue < 150) {
  modRatio = 2.0;      // 0 - 341 corresponds to 1:1
} else if (modRatioPotValue < 300) {
  modRatio = 4.0;      // 342 - 682 corresponds to 1:2
} else if (modRatioPotValue < 450) {
  modRatio = 6.0;      // 683 - 1023 corresponds to 1:3
} else if (modRatioPotValue < 600) {
  modRatio = 8.0;      // 683 - 1023 corresponds to 1:3
 } else if (modRatioPotValue < 750) {
  modRatio = .25;      // 683 - 1023 corresponds to 1:3
 } else if (modRatioPotValue < 900) {
  modRatio = .5;      // 683 - 1023 corresponds to 1:3
 }

  int phaseAmount = analogRead(potPhaseAmount);

  float phaseAmountMapped = map(phaseAmount, 0, 1023, 0, 800);

  for (int i = 0; i < POLY_COUNT; i++) {

    poly_waveforms[i].waveform1.phaseModulation(phaseAmountMapped);
  }

//REVERB


int reverbAmount = analogRead(reverbPin);

float reverbAmountMapped = map(reverbAmount, 0, 1023, 0, 1);

mixer3.gain(0, reverbAmountMapped);
mixer3.gain(1, 1.0 - reverbAmountMapped);

}
void OnNoteOn(byte channel, byte note, byte velocity) {



  

  int freq = (int)(440.0 * pow(2.0, ((float)note - 69.0) / 12.0));
  int poly_index = -1;
  unsigned long oldest_note_start_time = millis();


lfo2.frequency(freq * modRatio);




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
}
