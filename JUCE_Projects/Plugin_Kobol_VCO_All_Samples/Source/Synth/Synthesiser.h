#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ADSREnv.h"



class Kobol_VCOSynthesiserSample
{
public:
	Kobol_VCOSynthesiserSample(AudioFormatReader& file, String name_file);
	~Kobol_VCOSynthesiserSample();

	AudioBuffer<float>* getData();
	double getSampleRate();
	int getLength();
	int getF0();
	float getFSemitone();
	int getMidiNoteNumber();
	//void find_freq_from_voltage();
	void find_nearest_frequency();
	void setF0(int f0);
	String name_file;
	
private:
	AudioBuffer<float>* data = nullptr;
	double sampleRate = 48000.0;
	int length = 0;
	
	int f0;
	float fSemitone;
	int midiSemitone;
	int midiNoteNumber;
	/*const std::vector<int> f0s = { 20, 30, 40, 60, 80, 120, 160, 240, 
									320, 480, 640, 960, 1280, 1920, 
									2560, 3840, 5120, 7680, 10240, 15360, 20480 };*/

	/*const std::vector<String> voltajes = { "00.0", "00.5", "01.0", "01.5", "02.0", "02.5", "03.0",
										"03.5", "04.0", "04.5", "05.0", "05.5", "06.0", "06.5",
										"07.0", "07.5", "08.0", "08.5", "09.0", "09.5", "10.0" };*/

	const std::vector<float> frequencies = { 12543.85, 11839.82, 11175.30, 10548.08, 9956.06, 9397.27, 8869.84,
										8372.02, 7902.13, 7458.62, 7040.00, 6644.88, 6271.93, 5919.91, 5587.65,
										5274.04, 4978.03, 4698.64, 4434.92, 4186.01, 3951.07, 3729.31, 3520.00,
										3322.44, 3135.96, 2959.96, 2793.83, 2637.02, 2489.02, 2349.32, 2217.46,
										2093.00, 1975.53, 1864.66, 1760.00, 1661.22, 1567.98, 1479.98, 1396.91,
										1318.51, 1244.51, 1174.66, 1108.73, 1046.50, 987.77, 932.33, 880.00,
										830.61, 783.99, 739.99, 698.46, 659.26, 622.25, 587.33, 554.37, 523.25,
										493.88, 466.16, 440.00, 415.30, 392.00, 369.99, 349.23, 329.63, 311.13,
										293.66, 277.18, 261.63, 246.94, 233.08, 220.00, 207.65, 196.00, 185.00,
										174.61, 164.81, 155.56, 146.83, 138.59, 130.81, 123.47, 116.54, 110.00,
										103.83, 98.00, 92.50, 87.31, 82.41, 77.78, 73.42, 69.30, 65.41, 61.74,
										58.27, 55.00, 51.91, 49.00, 46.25, 43.65, 41.20, 38.89, 36.71, 34.65,
										32.70, 30.87, 29.14, 27.50, 25.96, 24.50, 23.12, 21.83, 20.60, 19.45,
										18.35, 17.32, 16.35, 15.43, 14.57, 13.75, 12.98, 12.25, 11.56, 10.91,
										10.30, 9.72, 9.18, 8.66, 8.18 };

	const std::vector<int> noteMidiNumbers = { 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115,
											114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 
											101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 
											85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 
											69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 
											53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 
											37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22,
											21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4,
											3, 2, 1, 0 };


	JUCE_LEAK_DETECTOR(Kobol_VCOSynthesiserSample)
};

class Kobol_VCOSynthesiserVoice
{
public:
	Kobol_VCOSynthesiserVoice();
	~Kobol_VCOSynthesiserVoice();

	void keyPress(int midiNote, int midiChannel, float midiVelocity, Kobol_VCOSynthesiserSample* sample, 
		int f0, float fSemitone, int midiNoteNumber, float attack_time, float decay_time, float sustain_val, float release_time);
	void keyRelease();
	void cutOut();

	void getNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples, float gain);
	bool isEnvelopeOff();

	void setSampleRate(double sampleRate);
	int getMidiNote();
	int getMidiChannel();

private:
	double transposeNotesCalc(int midiNote, int f0, float fSemitone, int midiNoteNumber);
	Kobol_VCOADSREnv adsr;
	Kobol_VCOSynthesiserSample* sample = nullptr;

	int midiNote = 0, midiChannel = 0;
	float midiVelocity = 0.0;

	double curSamplePosition = 0.0, pitchSpeed = 0.0;
	double sampleRate = 48000.0;

	JUCE_LEAK_DETECTOR(Kobol_VCOSynthesiserVoice)
};

class Kobol_VCOSynthesiser
{
public:
	Kobol_VCOSynthesiser();
	~Kobol_VCOSynthesiser();

	void keyPress(int midiChannel, int midiNote, float midiVelocity, int f0, float fSemitone, int midiNoteNumber);
	void keyRelease(int midiChannel, int midiNote);
	void cutOutVoices();

	void getNextBlock(AudioBuffer<float>& outputBuffer, MidiBuffer& incomingMidi, int startSample, int numSamples);
	Kobol_VCOSynthesiserVoice* findFreeOrStealLowestVoice();

	void setSound(Kobol_VCOSynthesiserSample* sample);
	void setSampleRate(double sampleRate);

	void setPoly(int poly);
	void setPolyRange(int polyRange);

	int getPoly();
	int getPolyRange();

	void setAttack(float attack_time);
	void setDecay(float decay_time);
	void setSustain(float sustain_val);
	void setRelease(float release_time);
	void setWave(float waveform);
	
	// ADSR
	float getAttack();
	float getDecay();
	float getSustain();
	float getRelease();
	int getWaveform();

	//==============================================================================
	// 
	//Fader GAIN
	void setGain(float gain_val);
	float getGain();

	void findNearestMidiNumber(int midiNumber);
	void setWaveform(int waveformID);

	//Declare all samples of Sine Plus Residual
	std::vector<Kobol_VCOSynthesiserSample*> pulse_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_plus_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_square_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_square_plus_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> square_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> square_plus_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> square_pulse_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> square_pulse_plus_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_plus_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_sawtooth_SPR;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_sawtooth_plus_SPR;
	std::vector<std::vector<Kobol_VCOSynthesiserSample*>> waveformsVector_SPR;

	//Declare all samples of Sine
	std::vector<Kobol_VCOSynthesiserSample*> pulse_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_plus_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_square_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> sawtooth_square_plus_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> square_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> square_plus_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> square_pulse_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> square_pulse_plus_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_plus_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_sawtooth_SINE;
	std::vector<Kobol_VCOSynthesiserSample*> triangular_sawtooth_plus_SINE;
	std::vector<std::vector<Kobol_VCOSynthesiserSample*>> waveformsVector_SINE;

	int current_waveform = 0; // 0 to 12
	bool isSPR = true;


private:
	OwnedArray<Kobol_VCOSynthesiserVoice> voices;


	Kobol_VCOSynthesiserSample* sample = nullptr;

	int poly = 4, polyRange = 64;
	float gain_val = 0.0f; //Fader Gain
	float attack_time = 0.0f, decay_time = 0.0f, sustain_val = 1.0f, release_time = 0.0f; //ADSR
	float waveform;

	int curMidiPosition = 0, lastMidiPosition = 0;
	double sampleRate = 0.0;
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Kobol_VCOSynthesiser)
};
