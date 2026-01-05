#pragma once
#include "PluginProcessor.h"


class AnnieLookAndFeel : public juce::LookAndFeel_V4
{
public:
    AnnieLookAndFeel()
    {
        customTypeface = juce::Typeface::createSystemTypefaceFor (
                                    BinaryData::CalamityJaneNF_ttf,
                                    BinaryData::CalamityJaneNF_ttfSize);
    }

    juce::Typeface::Ptr getCustomTypeface() { return customTypeface; }

   
    juce::Font getLabelFont (juce::Label&) override
    {
        return juce::FontOptions (customTypeface).withHeight (26.0f);
    }

 
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        const juce::Font font (getLabelFont (label));
        g.setFont (font);
        auto textArea = label.getLocalBounds();
        
        g.setColour (juce::Colours::dodgerblue);
        g.drawFittedText (label.getText(), textArea.translated (1.5f, 1.5f), juce::Justification::centred, 1);
        
        g.setColour (juce::Colours::hotpink);
        g.drawFittedText (label.getText(), textArea, juce::Justification::centred, 1);
    }


    void drawComboBox (juce::Graphics& g, int width, int height, bool,
                       int, int, int, int, juce::ComboBox& box) override
    {
        auto cornerSize = 0.0f;
        juce::Rectangle<float> boxBounds (0, 0, (float)width, (float)height);
        float shadowOffset = 1.5f;
        
        g.setColour (juce::Colours::dodgerblue);
  
        g.drawRoundedRectangle (boxBounds.reduced(1.0f).translated(shadowOffset, shadowOffset), cornerSize, 2.0f);


        g.setColour (juce::Colours::transparentBlack);
        g.fillRoundedRectangle (boxBounds, cornerSize);
 
        g.setColour (juce::Colours::hotpink);
        g.drawRoundedRectangle (boxBounds.reduced(1.0f), cornerSize, 2.0f);

        
        juce::Path trianglePath;
        trianglePath.addTriangle (width - 15.0f, height * 0.4f,
                                  width - 5.0f, height * 0.4f,
                                  width - 10.0f, height * 0.6f);

        g.setColour (juce::Colours::dodgerblue);
        g.fillPath (trianglePath, juce::AffineTransform::translation (shadowOffset, shadowOffset));

        g.setColour (juce::Colours::hotpink);
        g.fillPath (trianglePath);
    }


    juce::Font getComboBoxFont (juce::ComboBox& box) override {
        return juce::FontOptions (customTypeface).withHeight (18.0f);
    }


    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override
    {
        g.fillAll (juce::Colours::transparentBlack);
        g.fillAll();
        g.setColour (juce::Colours::hotpink);
        g.drawRect (0, 0, width, height, 2);
    }


    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted, bool isChecked, bool isEnabled,
                            const juce::String& text, const juce::String& shortcutKeyText,
                            const juce::Drawable* icon, const juce::Colour* textColourToUse) override
    {
        if (isHighlighted && isEnabled) {
                    g.setColour (juce::Colours::dodgerblue);
                    g.fillRect (area);
        }

        g.setFont (juce::FontOptions (customTypeface).withHeight (18.0f));
        

        auto r = area.reduced (4, 0);
        g.setColour (juce::Colours::black.withAlpha(0.5f));
        g.drawFittedText (text, r.translated(1, 1), juce::Justification::centredLeft, 1);
        
        g.setColour (isHighlighted ? juce::Colours::white : juce::Colours::hotpink);
        g.drawFittedText (text, r, juce::Justification::centredLeft, 1);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool, bool) override
    {
        g.setFont (juce::FontOptions (customTypeface).withHeight (18.0f));
        auto textArea = button.getLocalBounds();
        
        g.setColour (juce::Colours::dodgerblue);
        g.drawFittedText (button.getButtonText(), textArea.translated (1.2f, 1.2f), juce::Justification::centred, 1);
        
        g.setColour (juce::Colours::hotpink);
        g.drawFittedText (button.getButtonText(), textArea, juce::Justification::centred, 1);
    }


    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 20.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        float lineThickness = 6.0f;
        

        g.setColour (juce::Colours::dodgerblue);
        juce::Path shadowTrack;
        shadowTrack.addCentredArc (centreX + 2.0f, centreY + 2.0f, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath (shadowTrack, juce::PathStrokeType (lineThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));


        g.setColour (juce::Colours::hotpink);
        juce::Path mainTrack;
        mainTrack.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath (mainTrack, juce::PathStrokeType (lineThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

 
        auto dotRadius = 16.0f;
        auto dotX = centreX + std::sin (angle) * radius;
        auto dotY = centreY - std::cos (angle) * radius;

        g.setColour (juce::Colours::dodgerblue);
        g.fillEllipse (dotX - 6.0f, dotY - 6.0f, dotRadius, dotRadius);
        g.setColour (juce::Colours::hotpink);
        g.fillEllipse (dotX - 8.0f, dotY - 8.0f, dotRadius, dotRadius);
    }

private:
    juce::Typeface::Ptr customTypeface;
};

class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AnnieLookAndFeel annieStyle;
    juce::Slider panSlider;
    juce::ComboBox modeSelector;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    
    NewProjectAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
