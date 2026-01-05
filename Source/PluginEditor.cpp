#include "PluginProcessor.h"
#include "PluginEditor.h"

NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setLookAndFeel (&annieStyle);

    panSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 25);

    panSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::hotpink);
    panSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    
    addAndMakeVisible (panSlider);

    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "pan", panSlider);

    modeSelector.clear();
    modeSelector.addItem("Balance", 1);
    modeSelector.addItem("Stereo Pan", 2); 
    //modeSelector.addItem("Equal Power", 3);
    

    modeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::hotpink);
    modeSelector.setColour(juce::ComboBox::arrowColourId, juce::Colours::hotpink);
    
    addAndMakeVisible(modeSelector);
    
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "panMode", modeSelector);

    setSize (300, 380);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
  
    setLookAndFeel (nullptr);
}

void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{

    g.fillAll (juce::Colour (0xFF14FFDC));

    auto myTypeface = annieStyle.getCustomTypeface();

    if (myTypeface != nullptr)
    {
  
        auto fontOptions = juce::FontOptions(myTypeface).withHeight(60.0f);
        g.setFont (fontOptions);


        g.setColour (juce::Colours::dodgerblue);
        g.drawText ("Annie's Panner", 2, 17, getWidth(), 80, juce::Justification::centred);

 
        g.setColour (juce::Colours::hotpink);
        g.drawText ("Annie's Panner", 0, 15, getWidth(), 80, juce::Justification::centred);
    }
}

void NewProjectAudioProcessorEditor::resized()
{
    panSlider.setBounds (50, 92, 200, 200);
    modeSelector.setBounds (75, 308, 150, 30);
}
