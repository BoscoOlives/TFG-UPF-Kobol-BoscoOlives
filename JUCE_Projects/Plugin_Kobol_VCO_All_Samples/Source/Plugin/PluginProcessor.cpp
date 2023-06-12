#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <vector>
#include <complex>
#include <cmath>

Kobol_VCOAudioProcessor::Kobol_VCOAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	audioFormatManager.registerBasicFormats();
	synthesiser.setPoly(4);

	//path_files = "C:\\Users\\bosco\\Desktop\\TFG\\TFG-Kobol-BoscoOlives\\rsf_kobol_expander_2-vco\\audio\\vco1\\";
	//path_jsons = "C:\\Users\\bosco\\Desktop\\TFG\\TFG-Kobol-BoscoOlives\\json_files\\vco1\\";
	File pluginFile = File::getSpecialLocation(File::currentExecutableFile);
	audioFolderSPR = pluginFile.getParentDirectory().getChildFile("data").getChildFile("audios").getChildFile("cut_synthesized_sinePlusRes");
	audioFolderS = pluginFile.getParentDirectory().getChildFile("data").getChildFile("audios").getChildFile("cut_synthesized_sine");
	jsonFolder = pluginFile.getParentDirectory().getChildFile("data").getChildFile("json_files");
	/*path_files = audioFolder.getFullPathName();
	path_jsons = jsonFolder.getFullPathName();*/
	/*path_files = path_files + "\\";
	path_jsons = path_jsons + "\\";*/
	readAudiosFromFolders();

	

	synthesiser.setSound(this->synthesiser.triangular_SPR[0]);

}

File Kobol_VCOAudioProcessor::getFile(const String& path)
{ 
	// esta funcion se encarga de leer los audios de las carpeta y devolverlos
	File file;
	DirectoryIterator it(path, true, "*.*");

	while (it.next())
	{
		File currentFile = it.getFile();

		if (currentFile.hasFileExtension("wav"))
		{
			return currentFile;
		}
	}

	return file;
}

void Kobol_VCOAudioProcessor::readAudiosFromFolders()
{
	
	initSamples("pulse", synthesiser.pulse_SPR, audioFolderSPR);
	initSamples("sawtooth", synthesiser.sawtooth_SPR, audioFolderSPR);
	initSamples("sawtooth +", synthesiser.sawtooth_plus_SPR, audioFolderSPR);
	initSamples("sawtooth-square", synthesiser.sawtooth_square_SPR, audioFolderSPR);
	initSamples("sawtooth-square +", synthesiser.sawtooth_square_plus_SPR, audioFolderSPR);
	initSamples("square", synthesiser.square_SPR, audioFolderSPR);
	initSamples("square +", synthesiser.square_plus_SPR, audioFolderSPR);
	initSamples("square-pulse", synthesiser.square_pulse_SPR, audioFolderSPR);
	initSamples("square-pulse +", synthesiser.square_pulse_plus_SPR, audioFolderSPR);
	initSamples("triangular", synthesiser.triangular_SPR, audioFolderSPR);
	initSamples("triangular +", synthesiser.triangular_plus_SPR, audioFolderSPR);
	initSamples("triangular-sawtooth", synthesiser.triangular_sawtooth_SPR, audioFolderSPR);
	initSamples("triangular-sawtooth +", synthesiser.triangular_sawtooth_plus_SPR, audioFolderSPR);

	// Creacion de la lista de waveforms_SPR en orden del Knob Selector
	synthesiser.waveformsVector_SPR.push_back(synthesiser.triangular_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.triangular_plus_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.triangular_sawtooth_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.triangular_sawtooth_plus_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.sawtooth_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.sawtooth_plus_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.sawtooth_square_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.sawtooth_square_plus_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.square_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.square_plus_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.square_pulse_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.square_pulse_plus_SPR);
	synthesiser.waveformsVector_SPR.push_back(synthesiser.pulse_SPR);

	initSamples("pulse", synthesiser.pulse_SINE, audioFolderS);
	initSamples("sawtooth", synthesiser.sawtooth_SINE, audioFolderS);
	initSamples("sawtooth +", synthesiser.sawtooth_plus_SINE, audioFolderS);
	initSamples("sawtooth-square", synthesiser.sawtooth_square_SINE, audioFolderS);
	initSamples("sawtooth-square +", synthesiser.sawtooth_square_plus_SINE, audioFolderS);
	initSamples("square", synthesiser.square_SINE, audioFolderS);
	initSamples("square +", synthesiser.square_plus_SINE, audioFolderS);
	initSamples("square-pulse", synthesiser.square_pulse_SINE, audioFolderS);
	initSamples("square-pulse +", synthesiser.square_pulse_plus_SINE, audioFolderS);
	initSamples("triangular", synthesiser.triangular_SINE, audioFolderS);
	initSamples("triangular +", synthesiser.triangular_plus_SINE, audioFolderS);
	initSamples("triangular-sawtooth", synthesiser.triangular_sawtooth_SINE, audioFolderS);
	initSamples("triangular-sawtooth +", synthesiser.triangular_sawtooth_plus_SINE, audioFolderS);

	// Creacion de la lista de waveforms_SINE en orden del knob Selector
	synthesiser.waveformsVector_SINE.push_back(synthesiser.triangular_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.triangular_plus_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.triangular_sawtooth_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.triangular_sawtooth_plus_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.sawtooth_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.sawtooth_plus_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.sawtooth_square_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.sawtooth_square_plus_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.square_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.square_plus_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.square_pulse_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.square_pulse_plus_SINE);
	synthesiser.waveformsVector_SINE.push_back(synthesiser.pulse_SINE);


}

void Kobol_VCOAudioProcessor::initSamples(String waveform, std::vector<Kobol_VCOSynthesiserSample*>& vector_waveform, File audioFolder)
{
	AudioFormatReader* reader = nullptr;
	File audioFolders = audioFolder.getChildFile(waveform);
	File jsonFolders = jsonFolder.getChildFile(waveform);


	String path_waveform = audioFolders.getFullPathName();

	//Logger::writeToLog("path_waveform" + path_waveform);

	if (!File(path_waveform).exists())
	{
		DBG("ERROR: Path does not exist!");
	}

	DirectoryIterator it(path_waveform, true, "*.*");
	while (it.next())
	{
		File file = it.getFile();
		String name_file = file.getFileNameWithoutExtension();
		// método toStdString() de juce::String para convertir la ruta a una cadena de caracteres de tipo std::string

		std::string path_json = jsonFolders.getChildFile(String(name_file + ".json")).getFullPathName().toStdString();


		reader = audioFormatManager.createReaderFor(file);
		Kobol_VCOSynthesiserSample* sample = new Kobol_VCOSynthesiserSample(*reader, name_file);
		sample->setF0(readJSON(path_json)); // leer el archivo JSON correspondiente
		vector_waveform.push_back(sample);
		//Logger::writeToLog("NameFile: " + String(name_file));

		//Logger::writeToLog("fSemitone: " + String(sample->getFSemitone()));
		//Logger::writeToLog("midiNoteNumber: " + String(sample->getMidiNoteNumber()));
	}
}

// Función para leer archivos JSON
int Kobol_VCOAudioProcessor::readJSON(const std::string& path) {
	std::ifstream archivo(path); //solo acepta std::string cuidado! no acepta juce::String
	if (!archivo.is_open()) {
		std::cerr << "No se pudo abrir el archivo " << path << '\n';
		return 0;
	}

	json datos;
	try {
		archivo >> datos; // leer el archivo JSON
	}
	catch (json::parse_error& e) {
		std::cerr << "Error al parsear el archivo " << path << ": " << e.what() << '\n';
		return 0;
	}

	// leer los valores y almacenarlos en diferentes variables según su tipo
	std::string signalName = datos["Signal Name"].get<std::string>();
	int fundFrec = datos["Fund Frec"].get<int>();
	int sampleRate = datos["Sample Rate"].get<int>();
	int frameSize = datos["Frame Size"].get<int>();
	int hopSize = datos["Hop Size"].get<int>();
	std::vector<std::vector<float>> harmonicMag = datos["Harmonic Magnitud"].get<std::vector<std::vector<float>>>();
	std::vector<std::vector<float>> harmonicFreq = datos["Harmonic Frequency"].get<std::vector<std::vector<float>>>();
	std::vector<std::vector<float>> harmonicPhase = datos["Harmonic Phase"].get<std::vector<std::vector<float>>>();
	std::vector<std::vector<float>> harmonicRes = datos["Harmonic Res"].get<std::vector<std::vector<float>>>();

	return fundFrec; //return provisional, aqui se tienen que devolver todas las listas cuando se implemente SprModeSynth

}

// para no utilizar la libreria JSON, aunque es más extenso...
//int Kobol_VCOAudioProcessor::readJSON(const std::string& path)
//{
//	juce::File file(path);
//
//	if (!file.existsAsFile())
//	{
//		std::cerr << "No se pudo abrir el archivo " << path << '\n';
//		return 0;
//	}
//
//	juce::String jsonString = file.loadFileAsString();
//	auto resultVar = juce::JSON::parse(jsonString);
//
//	if (resultVar.isVoid())
//	{
//		std::cerr << "Error al parsear el archivo " << path << ": " << resultVar.toString() << '\n';
//		return 0;
//	}
//
//	juce::var jsonObj(resultVar.getDynamicObject());
//
//	//std::string signalName = (const char*)jsonObj["Signal Name"];
//	std::string signalName = jsonObj["Signal Name"].toString().toStdString();
//	int fundFrec = jsonObj["Fund Frec"];
//	int sampleRate = jsonObj["Sample Rate"];
//	int frameSize = jsonObj["Frame Size"];
//	int hopSize = jsonObj["Hop Size"];
//
//	juce::var harmonicMagVar = jsonObj["Harmonic Magnitud"];
//	juce::Array<juce::var>& harmonicMagArray = *harmonicMagVar.getArray();
//	std::vector<std::vector<float>> harmonicMag;
//	for (int i = 0; i < harmonicMagArray.size(); i++)
//	{
//		juce::Array<juce::var>& rowArray = *harmonicMagArray[i].getArray();
//		std::vector<float> rowVec(rowArray.begin(), rowArray.end());
//		harmonicMag.push_back(rowVec);
//	}
//
//	juce::var harmonicFreqVar = jsonObj["Harmonic Frequency"];
//	juce::Array<juce::var>& harmonicFreqArray = *harmonicFreqVar.getArray();
//	std::vector<std::vector<float>> harmonicFreq;
//	for (int i = 0; i < harmonicFreqArray.size(); i++)
//	{
//		juce::Array<juce::var>& rowArray = *harmonicFreqArray[i].getArray();
//		std::vector<float> rowVec(rowArray.begin(), rowArray.end());
//		harmonicFreq.push_back(rowVec);
//	}
//
//	juce::var harmonicPhaseVar = jsonObj["Harmonic Phase"];
//	juce::Array<juce::var>& harmonicPhaseArray = *harmonicPhaseVar.getArray();
//	std::vector<std::vector<float>> harmonicPhase;
//	for (int i = 0; i < harmonicPhaseArray.size(); i++)
//	{
//		juce::Array<juce::var>& rowArray = *harmonicPhaseArray[i].getArray();
//		std::vector<float> rowVec(rowArray.begin(), rowArray.end());
//		harmonicPhase.push_back(rowVec);
//	}
//
//	juce::var harmonicResVar = jsonObj["Harmonic Res"];
//	juce::Array<juce::var>& harmonicResArray = *harmonicResVar.getArray();
//	std::vector<std::vector<float>> harmonicRes;
//	for (int i = 0; i < harmonicResArray.size(); i++)
//	{
//		juce::Array<juce::var>& rowArray = *harmonicResArray[i].getArray();
//		std::vector<float> rowVec(rowArray.begin(), rowArray.end());
//		harmonicRes.push_back(rowVec);
//	}
//
//	return fundFrec;
//}


void Kobol_VCOAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	synthesiser.cutOutVoices();
	synthesiser.setSampleRate(sampleRate);
}

void Kobol_VCOAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& incomingMidi)
{
	buffer.clear();
	keyboardState.processNextMidiBuffer(incomingMidi, 0, buffer.getNumSamples(), true);
	synthesiser.getNextBlock(buffer, incomingMidi, 0, buffer.getNumSamples());

}

MidiKeyboardState& Kobol_VCOAudioProcessor::getMidiKeyboardState()
{
	return keyboardState;
}

Kobol_VCOSynthesiser& Kobol_VCOAudioProcessor::getSynthesiser()
{
	return synthesiser;
}

Kobol_VCOAudioProcessor::~Kobol_VCOAudioProcessor() {}

//==============================================================================

const String Kobol_VCOAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool Kobol_VCOAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool Kobol_VCOAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool Kobol_VCOAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double Kobol_VCOAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int Kobol_VCOAudioProcessor::getNumPrograms()
{
	return 1;
}

int Kobol_VCOAudioProcessor::getCurrentProgram()
{
	return 0;
}

void Kobol_VCOAudioProcessor::setCurrentProgram(int index)
{
}

const String Kobol_VCOAudioProcessor::getProgramName(int index)
{
	return {};
}

void Kobol_VCOAudioProcessor::changeProgramName(int index, const String& newName)
{
}

void Kobol_VCOAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Kobol_VCOAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

bool Kobol_VCOAudioProcessor::hasEditor() const
{
	return true;
}

AudioProcessorEditor* Kobol_VCOAudioProcessor::createEditor()
{
	return new Kobol_VCOAudioProcessorEditor(*this);
}

void Kobol_VCOAudioProcessor::getStateInformation(MemoryBlock& destData)
{
}

void Kobol_VCOAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new Kobol_VCOAudioProcessor();
}