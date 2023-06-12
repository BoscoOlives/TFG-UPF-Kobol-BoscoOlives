#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../Synth/Synthesiser.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>// incluir la librería nlohmann/json

using json = nlohmann::json;

class Kobol_VCOAudioProcessor : public AudioProcessor
{
public:
	Kobol_VCOAudioProcessor();
	~Kobol_VCOAudioProcessor();

	File getFile(const String& path);
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void processBlock(AudioBuffer<float>& buffer, MidiBuffer& incomingMidi) override;

	MidiKeyboardState& getMidiKeyboardState();
	Kobol_VCOSynthesiser& getSynthesiser();

	//==========================================================================

	void releaseResources() override;
#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	// Declare synthesiser object
	Kobol_VCOSynthesiser synthesiser;
	// Declare all samples
	
	String path_files;
	String path_jsons;
	File audioFolderSPR;
	File audioFolderS;
	File jsonFolder;
	void initSamples(String waveform, std::vector<Kobol_VCOSynthesiserSample*>& vector_waveform, File audioFolder);
	int readJSON(const std::string& path);
	void readAudiosFromFolders();


private:
	AudioFormatManager audioFormatManager;

	MidiKeyboardState keyboardState;
	
	bool lastWasPlaying = false;
	bool curIsPlaying = false;
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Kobol_VCOAudioProcessor)
};