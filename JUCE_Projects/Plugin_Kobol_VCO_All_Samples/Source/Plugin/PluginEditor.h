#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class CustomLookAndFeel : public LookAndFeel_V4
{
public:
	// se define un booleano para determinar cual de las dos imagenes se va a utilizar
	CustomLookAndFeel() {}
	
	~CustomLookAndFeel() {}

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override
	{
		Image customKnobImage;
		customKnobImage = ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);

		// Calcula el ángulo de rotación basado en la posición del slider
		float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		// Rota la imagen del knob personalizado
		g.saveState();
		g.addTransform(AffineTransform::rotation(angle, x + width * 0.5f, y + height * 0.5f));
		g.drawImage(customKnobImage, x, y, width, height, 0, 0, customKnobImage.getWidth(), customKnobImage.getHeight());
		// Para los knobs pequeños, de momento queda desactivado
		//g.drawImageWithin(customKnobImage, x, y, width, height, RectanglePlacement::doNotResize);
		g.restoreState();
	}
};

class Kobol_VCOAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener
{
public:
	Kobol_VCOAudioProcessorEditor(Kobol_VCOAudioProcessor&);
	~Kobol_VCOAudioProcessorEditor();

	void paint(Graphics&) override;
	void resized() override;

	//slider volumen
	//juce::ScopedPointer <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttach;
	Font personalFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::MichromaRegular_ttf, BinaryData::MichromaRegular_ttfSize) };


private:
	void setValues();
	void setLabels(Slider& knob, Label& label, double defaultfVal, double startVal, double endVal, double stepVal, String textOnLabel, bool doubleClick, bool TextBox = true);
	void sliderValueChanged(Slider*) override;
	//void WaveformMenuChanged();


	Kobol_VCOAudioProcessor& processor;
	Kobol_VCOSynthesiser& synthesiser;
	MidiKeyboardState& keyboardState = processor.getMidiKeyboardState();

	//==============================================================================

	MidiKeyboardComponent keyboardComponent{ keyboardState, MidiKeyboardComponent::horizontalKeyboard };

	Slider polySlider, finePitchSlider;
	Label polyLabel, finePitchLabel;

	Slider attackKnob, decayKnob, sustainKnob, releaseKnob, waveformKnob, volumeKnob;
	Label attackLabel, decayLabel, sustainLabel, releaseLabel, waveformLabel, volumeLabel;

	TextButton button{ "SinePlusRes" };

	//Label textLabel{ {}, "Waveforms" };
	//Font textFont{ 15.0f };
	//ComboBox menu;

	// Create enum value for every waveform (wf)
	enum IDsWaveforms
	{
		wfPulse = 1,
		wfSawtooth,
		wfSawtoothPlus,
		wfSawtoothSquare,
		wfSawtoothSquarePlus,
		wfSquare,
		wfSquarePlus,
		wfSquarePulse,
		wfSquarePulsePlus,
		wfTriangular,
		wfTriangularPlus,
		wfTriangularSawtooth,
		wfTriangularSawtoothPlus,
		numberOfWaveforms
	};


	// Image Background
	Image waveformsImg;
	Image volumeImg;
	Image wavePlusVolImg;
	int noteWidth;
	std::unique_ptr<CustomLookAndFeel> customLookAndFeelSelector = std::make_unique<CustomLookAndFeel>();
	//std::unique_ptr<CustomLookAndFeel> customLookAndFeelKnobs = std::make_unique<CustomLookAndFeel>();
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Kobol_VCOAudioProcessorEditor)
};
