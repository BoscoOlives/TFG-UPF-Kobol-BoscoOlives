#include "Synthesiser.h"

Kobol_VCOSynthesiserSample::Kobol_VCOSynthesiserSample(AudioFormatReader& file, String name_file)
{
	if (&file != nullptr)
	{
		sampleRate = file.sampleRate;
		length = (int)file.lengthInSamples;
		data = new AudioBuffer<float>(2, (int)file.lengthInSamples + 4);
		file.read(data, 0, (int)file.lengthInSamples + 4, 0, true, true);
		this->name_file = name_file;
		//find_freq_from_voltage(); //init f0 variable
		//find_nearest_frequency(); //init fSemitone variable
	}
}

AudioBuffer<float>* Kobol_VCOSynthesiserSample::getData()
{
	return data;
}

double Kobol_VCOSynthesiserSample::getSampleRate()
{
	return sampleRate;
}

int Kobol_VCOSynthesiserSample::getF0()
{
	return f0;
}
float Kobol_VCOSynthesiserSample::getFSemitone()
{
	return fSemitone;
}

int Kobol_VCOSynthesiserSample::getMidiNoteNumber()
{
	return midiNoteNumber;
}

int Kobol_VCOSynthesiserSample::getLength()
{
	return length;
}

void Kobol_VCOSynthesiserSample::setF0(int f0)
{
	this->f0 = f0;
	find_nearest_frequency(); //init fSemitone variable
}

//void Kobol_VCOSynthesiserSample::find_freq_from_voltage()
//{
//
//	for (int i = 0; i < voltajes.size(); ++i)
//	{
//		if (this->name_file.contains(String(voltajes[i])))
//		{
//			this->f0 = this->f0s[i];
//			
//		}
//	}
//}


void Kobol_VCOSynthesiserSample::find_nearest_frequency()
{
	//busca la frecuencia más cercana a la frecuencia fundamental "f0" del sample de audio, en un listado de frecuencias predefinidas (semitonos)
	float min_distance = std::abs(this->f0 - this->frequencies[0]);
	int nearest_index = 0;
	for (int i = 1; i < this->frequencies.size(); i++) {
		float distance = std::abs(this->f0 - this->frequencies[i]);

		if (distance < min_distance) {
			min_distance = distance;
			nearest_index = i;
		}
	}
	// Agregamos la frec. del semitono encontrado y su respectivo numero midi
	this->fSemitone =  this->frequencies[nearest_index];
	this->midiNoteNumber = this->noteMidiNumbers[nearest_index];
	
}

Kobol_VCOSynthesiserSample::~Kobol_VCOSynthesiserSample() {}

//==============================================================================

Kobol_VCOSynthesiserVoice::Kobol_VCOSynthesiserVoice() {}
Kobol_VCOSynthesiserVoice::~Kobol_VCOSynthesiserVoice() {}

void Kobol_VCOSynthesiserVoice::keyPress(int midiNote, int midiChannel, float midiVelocity, Kobol_VCOSynthesiserSample* sample, int f0, float fSemitone, int midiNoteNumber, float attack_time, float decay_time, float sustain_val, float release_time)
{
	this->midiNote = midiNote;
	//DBG(midiNote);
	
	this->midiChannel = midiChannel;
	this->midiVelocity = midiVelocity;
	this->sample = sample; // per si volem posar dos samples
	
	curSamplePosition = 0.0;
	pitchSpeed = transposeNotesCalc(midiNote, f0, fSemitone, midiNoteNumber);
	adsr.setSampleRate(this->sample->getSampleRate());
	adsr.setParameters(attack_time, decay_time, sustain_val, release_time);
	adsr.keyPress();
}

void Kobol_VCOSynthesiserVoice::keyRelease()
{
	adsr.keyRelease();
}

void Kobol_VCOSynthesiserVoice::cutOut()
{
	adsr.cutOut();
}

void Kobol_VCOSynthesiserVoice::getNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples, float gain)
{
	if (sample != nullptr && sampleRate > 0.0)
	{
		int sampleLength = sample->getLength();
		const float* inL = sample->getData()->getReadPointer(0);
		const float* inR = sample->getData()->getReadPointer(1);
		float* outL = outputBuffer.getWritePointer(0, startSample);
		float* outR = outputBuffer.getWritePointer(1, startSample);

		for (int i = startSample; i < numSamples; i++)
		{
			int nearestSample_left = static_cast<int>(curSamplePosition);
			int nearestSample_right = nearestSample_left + 1;
			float distance_left = curSamplePosition - nearestSample_left;
			float distance_right = 1.0f - distance_left;
			float envelopeValue = adsr.getNextSample();

			// Verificar si se excede la longitud de la muestra
			if (nearestSample_right >= sampleLength)
				nearestSample_right = 0;

			outL[i] += (inL[nearestSample_left] * distance_right + inL[nearestSample_right] * distance_left) * (midiVelocity * envelopeValue) * juce::Decibels::decibelsToGain(3.0f) * juce::Decibels::decibelsToGain(gain);
			outR[i] += (inR[nearestSample_left] * distance_right + inR[nearestSample_right] * distance_left) * (midiVelocity * envelopeValue) * juce::Decibels::decibelsToGain(3.0f) * juce::Decibels::decibelsToGain(gain);

			curSamplePosition += pitchSpeed;

			// Verificar los límites del audio y ajustar curSamplePosition si se excede
			if (curSamplePosition >= sampleLength)
				curSamplePosition -= sampleLength;
			else if (curSamplePosition < 0.0)
				curSamplePosition += sampleLength;
		}
	}
}


double Kobol_VCOSynthesiserVoice::transposeNotesCalc(int midiNote, int f0, float fSemitone, int midiNoteNumber)
{
	// f0 del sample que se utiliza para reproducir la nota midi.
	// fSemitone: es la f0 del midiNoteNumber.
	//Objetivo: hay que transponer el sonido con f0 a la fSemitone
	double pitchRatio = fSemitone / f0;
	//DBG("f0: " + String(f0));
	//DBG("fSemitone: " + String(fSemitone));
	//Logger::writeToLog("midiNoteNumber: " + String(midiNoteNumber));
	//DBG("midiNote: " + String(midiNote));

	// Calculate the pitch speed using the pitch ratio
	// c5 = 84 = 523.25Hz
	// c5 = 84, 1 octave = 12 semitones, 12 semitones = 1200 cents
	double pitchOffset = (std::log2(pitchRatio));
	
	//midiNote: es la tecla midi que el usuario esta pulsando
	// midiNoteNumber: es la nota midi asignada a un sample, es la mas proxima a la nota midi posible.
	double octaveShift = (((midiNote - midiNoteNumber) / 12.0) + pitchOffset);

	// +1 octave = pitch * 2, -1 octave = pitch / 2
	return pow(2.0, octaveShift) * (this->sample->getSampleRate() / this->sampleRate);
}

bool Kobol_VCOSynthesiserVoice::isEnvelopeOff()
{
	return adsr.getEnvelopeStage() == Stage::off;
}

void Kobol_VCOSynthesiserVoice::setSampleRate(double sampleRate)
{
	this->sampleRate = sampleRate;
}

int Kobol_VCOSynthesiserVoice::getMidiNote()
{
	return midiNote;
}

int Kobol_VCOSynthesiserVoice::getMidiChannel()
{
	return midiChannel;
}

//==============================================================================

Kobol_VCOSynthesiser::Kobol_VCOSynthesiser() {}
Kobol_VCOSynthesiser::~Kobol_VCOSynthesiser() {}

void Kobol_VCOSynthesiser::keyPress(int midiChannel, int midiNote, float midiVelocity, int f0, float fSemitone, int midiNoteNumber)
{
	if (!voices.isEmpty() && sampleRate > 0.0)
	{
		Kobol_VCOSynthesiserVoice* v = findFreeOrStealLowestVoice();
		v->setSampleRate(sampleRate);
		v->keyPress(midiNote, midiChannel, midiVelocity, sample, f0, fSemitone, midiNoteNumber, attack_time, decay_time, sustain_val, release_time);
	}
}

void Kobol_VCOSynthesiser::keyRelease(int midiChannel, int midiNote)
{
	for (Kobol_VCOSynthesiserVoice* v : voices)
		if (v->getMidiNote() == midiNote && v->getMidiChannel() == midiChannel)
			v->keyRelease();
}

void Kobol_VCOSynthesiser::cutOutVoices()
{
	for (Kobol_VCOSynthesiserVoice* v : voices)
		v->cutOut();
}

void Kobol_VCOSynthesiser::getNextBlock(AudioBuffer<float>& outputBuffer, MidiBuffer& incomingMidi, int startSample, int numSamples)
{
	if (&outputBuffer != nullptr && sampleRate > 0.0)
	{
		MidiBuffer::Iterator midiIterator(incomingMidi);
		MidiMessage m;
		curMidiPosition = startSample;
		midiIterator.setNextSamplePosition(curMidiPosition);

		while (curMidiPosition < numSamples)
		{
			if (midiIterator.getNextEvent(m, curMidiPosition))
			{
				if (m.isNoteOn())
				{
					// Buscar sample con fSemitone más cercano a m.getNoteNumber()
					this->findNearestMidiNumber(m.getNoteNumber());

					keyPress(m.getChannel(), m.getNoteNumber(), m.getFloatVelocity(), this->sample->getF0(), this->sample->getFSemitone(), this->sample->getMidiNoteNumber());
				}
				else if (m.isNoteOff())
				{
					keyRelease(m.getChannel(), m.getNoteNumber());
				}

				for (Kobol_VCOSynthesiserVoice* v : voices)
					v->getNextBlock(outputBuffer, lastMidiPosition, curMidiPosition, gain_val);

				lastMidiPosition = curMidiPosition;
			}
			else
			{
				for (Kobol_VCOSynthesiserVoice* v : voices)
					v->getNextBlock(outputBuffer, curMidiPosition, (numSamples - curMidiPosition), gain_val);

				return;
			}
		}
	}
}

Kobol_VCOSynthesiserVoice* Kobol_VCOSynthesiser::findFreeOrStealLowestVoice()
{
	if (voices.isEmpty())
	{
		return nullptr;
	}
	else
	{
		Kobol_VCOSynthesiserVoice* lowestVoice = voices.getFirst();

		for (Kobol_VCOSynthesiserVoice* v : voices)
		{
			if (v->isEnvelopeOff())
			{
				return v;
			}
			else if (v->getMidiNote() < lowestVoice->getMidiNote())
			{
				lowestVoice = v;
			}

		}
		return lowestVoice;
	}
}

void Kobol_VCOSynthesiser::setSound(Kobol_VCOSynthesiserSample* sample)
{
	this->sample = sample;
}


void Kobol_VCOSynthesiser::setSampleRate(double sampleRate)
{
	this->sampleRate = sampleRate;
	for (Kobol_VCOSynthesiserVoice* v : voices)
		v->setSampleRate(sampleRate);
}

void Kobol_VCOSynthesiser::setPoly(int poly)
{
	// cuantas notas pueden ser tocadas al mismo tiempo
	this->poly = poly;
	voices.clear();
	for (int i = 0; i < poly; i++)
	{
		Kobol_VCOSynthesiserVoice* new_v = new Kobol_VCOSynthesiserVoice();
		new_v->setSampleRate(this->sampleRate);
		voices.add(new_v);
	}
}

void Kobol_VCOSynthesiser::setPolyRange(int polyRange)
{
	//dar valor máximo de teclas que pueden ser tocadas simultaniamente, establecido por defecto 64
	cutOutVoices();
	this->polyRange = polyRange;
}

int Kobol_VCOSynthesiser::getPoly()
{
	return poly;
}

int Kobol_VCOSynthesiser::getPolyRange()
{
	return polyRange;
}


float Kobol_VCOSynthesiser::getGain()
{
	return gain_val;
}

void Kobol_VCOSynthesiser::setGain(float gain_val)
{
	this->gain_val = gain_val;
}

void Kobol_VCOSynthesiser::findNearestMidiNumber(int midiNumber)
{	
	// Esta función busca de entre un vector de samples, el sample que tenga el midi number más cercano al midi number recibido como argumento.
	// midiNumber: El midi number para el cual queremos encontrar el sample más cercano.
	if (isSPR) {
		int nearestMidiNumber;
		float min_distance = std::abs(midiNumber - waveformsVector_SPR[current_waveform][0]->getMidiNoteNumber());
		int nearest_index = 0;
		for (size_t i = 1; i < waveformsVector_SPR[current_waveform].size(); i++)
		{
			nearestMidiNumber = waveformsVector_SPR[current_waveform][i]->getMidiNoteNumber();
			float distance = std::abs(midiNumber - nearestMidiNumber);
			if (distance < min_distance){
				min_distance = distance;
				nearest_index = i;
			}
		}
		// Agregamos el sample con el midi number más cercano a la tecla midi que se esta pulsando, del tipo de waveform que toca.
	
		this->setSound(waveformsVector_SPR[current_waveform][nearest_index]);
		
	}
	else {
		int nearestMidiNumber;
		float min_distance = std::abs(midiNumber - waveformsVector_SINE[current_waveform][0]->getMidiNoteNumber());
		int nearest_index = 0;
		for (size_t i = 1; i < waveformsVector_SINE[current_waveform].size(); i++)
		{
			nearestMidiNumber = waveformsVector_SINE[current_waveform][i]->getMidiNoteNumber();
			float distance = std::abs(midiNumber - nearestMidiNumber);
			if (distance < min_distance) {
				min_distance = distance;
				nearest_index = i;
			}
		}
		// Agregamos el sample con el midi number más cercano a la tecla midi que se esta pulsando, del tipo de waveform que toca.

		this->setSound(waveformsVector_SINE[current_waveform][nearest_index]);
	}

}

void Kobol_VCOSynthesiser::setWaveform(int waveformID)
{ //Setear el tipo de waveform que se esta usando, dependiendo del selector de PluginEditor
	//current_waveform = (Kobol_VCOSynthesiser::WAVEFORMS_ID)waveformID;
	current_waveform = waveformID; //waveformID start from 1
	//DBG(current_waveform);
}
//==============================================================================

void Kobol_VCOSynthesiser::setAttack(float attack_time)
{
	this->attack_time = attack_time;
}

void Kobol_VCOSynthesiser::setDecay(float decay_time)
{
	this->decay_time = decay_time;
}

void Kobol_VCOSynthesiser::setSustain(float sustain_val)
{
	this->sustain_val = sustain_val;
}

void Kobol_VCOSynthesiser::setRelease(float release_time)
{
	this->release_time = release_time;
}

void Kobol_VCOSynthesiser::setWave(float waveform)
{
	this->waveform = waveform;
}

float Kobol_VCOSynthesiser::getAttack()
{
	return attack_time;
}

float Kobol_VCOSynthesiser::getDecay()
{
	return decay_time;
}

float Kobol_VCOSynthesiser::getSustain()
{
	return sustain_val;
}

float Kobol_VCOSynthesiser::getRelease()
{
	return release_time;
}

int Kobol_VCOSynthesiser::getWaveform()
{
	return waveform;
}