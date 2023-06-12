#include "PluginProcessor.h"
#include "PluginEditor.h"

Kobol_VCOAudioProcessorEditor::Kobol_VCOAudioProcessorEditor(Kobol_VCOAudioProcessor& p) : AudioProcessorEditor(&p), processor(p), synthesiser(p.getSynthesiser())
{
	int defPoly = synthesiser.getPoly();
	int polyRange = synthesiser.getPolyRange();
	
	//personalFont.setBold(true);

	noteWidth = 22;

	float defGain = synthesiser.getGain(); //default Gain dado por el synthe

	setLabels(polySlider, polyLabel, defPoly, 1, polyRange, 1.0, "Poly", true);


	setLabels(attackKnob, attackLabel, 0.0, 0.0, 1.0, 0.001, "A", true, false);
	setLabels(decayKnob, decayLabel, 0.0, 0.0, 1.0, 0.001, "D", true, false);
	setLabels(sustainKnob, sustainLabel, 1.0, 0.0, 2.0, 0.001, "S", true, false);
	setLabels(releaseKnob, releaseLabel, 0.0, 0.0, 1.0, 0.001, "R", true, false);
	setLabels(waveformKnob, waveformLabel, 0.0, 0.0, 12, 1.0, " ", false, false);
	setLabels(volumeKnob, volumeLabel, defGain, -30.0, 10.0, 0.1, " ", false, false);
	
	
	keyboardComponent.clearKeyMappings();
	keyboardComponent.setAvailableRange(41, 84); //igual que el KOBOL original
	keyboardComponent.setKeyWidth(noteWidth);
	keyboardComponent.setOctaveForMiddleC(4); //para que vaya de C0 a C10
	addAndMakeVisible(keyboardComponent);
	setOpaque(true);

	
	
	// set Knobs Color
	getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::ghostwhite);

	// boton para SINE PLUS RES
	button.setClickingTogglesState(true);
	button.setToggleState(true, true);
	button.onClick = [&]()
	{
		synthesiser.isSPR = !synthesiser.isSPR;
	};
	addAndMakeVisible(button);
	
	setSize(700, 700);
}

Kobol_VCOAudioProcessorEditor::~Kobol_VCOAudioProcessorEditor() {}

void Kobol_VCOAudioProcessorEditor::paint(Graphics& g)
{
	//g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	//g.fillAll(Colours::black);


	// Crear un gradiente de colores
	ColourGradient gradient(Colour(0xff000000), 0, 0, Colour(0xff333333), 0, getHeight(), false);

	g.setGradientFill(gradient);
	g.fillAll();

	//waveformsImg = ImageCache::getFromMemory(BinaryData::plugin_waveforms_png, BinaryData::plugin_waveforms_pngSize);
	//volumeImg = ImageCache::getFromMemory(BinaryData::plugin_volume_png, BinaryData::plugin_volume_pngSize);
	wavePlusVolImg = ImageCache::getFromMemory(BinaryData::plugin_waveforms_quadrat_png, BinaryData::plugin_waveforms_quadrat_pngSize);
	//g.drawImageWithin(waveformsImg, 0, 0, waveformsImg.getWidth(), waveformsImg.getHeight(), RectanglePlacement::doNotResize);
	//g.drawImageWithin(volumeImg, getWidth()/2, 0, volumeImg.getWidth(), volumeImg.getHeight(), RectanglePlacement::doNotResize);
	g.drawImageWithin(wavePlusVolImg, 0, 0, wavePlusVolImg.getWidth(), wavePlusVolImg.getHeight(), RectanglePlacement::doNotResize);
	// LAF del Selector Waveform
	waveformKnob.setLookAndFeel(customLookAndFeelSelector.get());
	volumeKnob.setLookAndFeel(customLookAndFeelSelector.get());
	
	//ADSR COLOURS
	attackKnob.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke.withAlpha(0.5f));
	decayKnob.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke.withAlpha(0.5f));
	sustainKnob.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke.withAlpha(0.5f));
	releaseKnob.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::whitesmoke.withAlpha(0.5f));

	// button color
	button.setColour(TextButton::ColourIds::buttonOnColourId, Colours::whitesmoke);
	button.setColour(TextButton::ColourIds::buttonColourId, Colours::dimgrey);
	button.setColour(TextButton::ColourIds::textColourOffId, Colours::black);
	button.setColour(TextButton::ColourIds::textColourOnId, Colours::black);
	
	juce::LookAndFeel* lookAndFeel = &button.getLookAndFeel();
	// Establece la fuente para el LookAndFeel
	lookAndFeel->setDefaultSansSerifTypeface(personalFont.getTypeface());
}

void Kobol_VCOAudioProcessorEditor::resized()
{
	
	//DBG(keyboardComponent.getWidth());
	int keyboardWidth = noteWidth * 26;
	keyboardComponent.setBounds((getWidth() / 2) - (keyboardWidth / 2), getHeight() - 115, keyboardWidth, 115); // Dimensions del teclat
	

	//int knobSide = getWidth() / 8;
	int knobSide = 125;
	int knobY = keyboardComponent.getY() - knobSide - 10;
	int knobWaveformX = 76;
	int knobWaveformY = 123;
	int knobVolumeX = knobWaveformX + getWidth()/2 - 10;
	int knobVolumeY = knobWaveformY;

	attackKnob.setBounds(getWidth() / 2 - 2*knobSide, knobY, knobSide, knobSide);
	decayKnob.setBounds(getWidth() / 2 - knobSide, knobY, knobSide, knobSide);
	sustainKnob.setBounds(getWidth()/2, knobY, knobSide, knobSide);
	releaseKnob.setBounds(getWidth() / 2 +  knobSide, knobY, knobSide, knobSide);
	waveformKnob.setBounds(knobWaveformX, knobWaveformY, knobSide*1.8, knobSide*1.8);
	volumeKnob.setBounds(knobVolumeX, knobVolumeY, knobSide*1.8, knobSide*1.8);

	int sliderHeight = getWidth() / 12;
	//finePitchSlider.setBounds(50, attackKnob.getY() - 70 - sliderHeight, getWidth() - 50, sliderHeight);
	polySlider.setBounds(50, 20, getWidth() - 180, sliderHeight);

	button.setBounds(getWidth() - 120, 25, 100, 50);

	setValues();


	//textLabel.setBounds(10, 20, getWidth() - 20, 20);
	//menu.setBounds(10, textLabel.getY() + 30, getWidth() - 20, 20);
}

void Kobol_VCOAudioProcessorEditor::setValues()
{
	attackKnob.setValue(synthesiser.getAttack());
	decayKnob.setValue(synthesiser.getDecay());
	sustainKnob.setValue(synthesiser.getSustain());
	releaseKnob.setValue(synthesiser.getRelease());
	waveformKnob.setValue(synthesiser.getWaveform());

	polySlider.setValue(synthesiser.getPoly());

	volumeKnob.setValue(synthesiser.getGain()); //volumen slider set value
}

void Kobol_VCOAudioProcessorEditor::setLabels(Slider& knob, Label& label, double defaultfVal, double startVal, double endVal, double stepVal, String textOnLabel, bool doubleClick, bool TextBox)
{
	
	knob.setRange(startVal, endVal, stepVal);
	if (TextBox){ //para el polyslider
		knob.setTextBoxStyle(Slider::TextBoxLeft, false, 50, 20);
	}
	else {
		knob.setSliderStyle(Slider::Rotary);
		knob.setTextBoxStyle(Slider::NoTextBox, false, 50, 20);
		
	}
	knob.setDoubleClickReturnValue(doubleClick, defaultfVal);
	knob.addListener(this);
	knob.setPopupDisplayEnabled(true, true, nullptr, -1);
	addAndMakeVisible(knob);
	personalFont.setHeight(16);
	label.setText(textOnLabel, dontSendNotification);
	label.setFont(personalFont);
	label.attachToComponent(&knob, TextBox);
	label.setJustificationType(Justification::centred);
	label.setColour(Label::ColourIds::textColourId, Colours::whitesmoke);
	addAndMakeVisible(label);

}

void Kobol_VCOAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider == &attackKnob)				synthesiser.setAttack(attackKnob.getValue());
	else if (slider == &decayKnob)			synthesiser.setDecay(decayKnob.getValue());
	else if (slider == &sustainKnob)		synthesiser.setSustain(sustainKnob.getValue());
	else if (slider == &releaseKnob)		synthesiser.setRelease(releaseKnob.getValue());
	else if (slider == &waveformKnob)		synthesiser.setWaveform(waveformKnob.getValue());
	else if (slider == &polySlider)			synthesiser.setPoly((int)polySlider.getValue());
	else if (slider == &volumeKnob)			synthesiser.setGain(volumeKnob.getValue());

}
