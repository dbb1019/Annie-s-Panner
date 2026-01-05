#include "PluginProcessor.h"
#include "PluginEditor.h"

NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{}

NewProjectAudioProcessor::~NewProjectAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
   
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"pan", 1}, "Pan", -100.0f, 100.0f, 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID {"panMode", 1}, "Pan Mode",
                                                               juce::StringArray {"Balance", "Equal Power"}, 0));
        
        return layout;
    return layout;
}

//void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
//{
//    juce::ScopedNoDenormals noDenormals;
//    auto totalNumInputChannels  = getTotalNumInputChannels();
//    auto totalNumOutputChannels = getTotalNumOutputChannels();
//
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//        buffer.clear (i, 0, buffer.getNumSamples());
//
//    float panValue = apvts.getRawParameterValue("pan")->load();
//    float normalizedPan = (panValue + 100.0f) / 200.0f;
//
//    //Equal power pan, but the center position is 3dB lower.

//    float angle = normalizedPan * juce::MathConstants<float>::halfPi;
//    float gainL = std::cos(angle);
//    float gainR = std::sin(angle);
//
//    if (totalNumInputChannels >= 2)
//    {
//        buffer.applyGain(0, 0, buffer.getNumSamples(), gainL);
//        buffer.applyGain(1, 0, buffer.getNumSamples(), gainR);
//    }
//}


void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    const int numIn  = getTotalNumInputChannels();
    const int numOut = getTotalNumOutputChannels();

    for (int ch = 2; ch < numOut; ++ch)
        buffer.clear (ch, 0, numSamples);
    
    float panValue = apvts.getRawParameterValue("pan")->load();
    float pan = juce::jlimit(-1.0f, 1.0f, panValue / 100.0f);
    
    
    int mode = (int)apvts.getRawParameterValue("panMode")->load();

    float targetL = 1.0f;
    float targetR = 1.0f;

    if (numIn == 1) {
        // Mono - Equal Power
        //This seems redundant since there are few practical scenarios for it. In most DAWs, tracks (whether mono, stereo, or multi-channel) always have matching input and output channel configurations.
        
        float normPan = (pan + 1.0f) * 0.5f;
        float angle = normPan * juce::MathConstants<float>::halfPi;
        targetL = std::cos(angle);
        targetR = std::sin(angle);
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
    else {
        if (mode == 0) { // Balance Mode
            targetL = juce::jmin(1.0f, 1.0f - pan);
            targetR = juce::jmin(1.0f, 1.0f + pan);
        }
        else { // Equal Power Mode
            // map -1~1 to 0~1
            float normPan = (pan + 1.0f) * 0.5f;
            float angle = normPan * juce::MathConstants<float>::halfPi;
            targetL = std::cos(angle);
            targetR = std::sin(angle);
            
            // Note: Equal Power is typically used for positioning monaural signals.
            // For stereo input, this is equivalent to applying gain to the left and right channels separately.
        }
    }

    smoothedGainL.setTargetValue(targetL);
    smoothedGainR.setTargetValue(targetR);

    auto* outL = buffer.getWritePointer(0);
    auto* outR = buffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i)
    {
        outL[i] *= smoothedGainL.getNextValue();
        outR[i] *= smoothedGainR.getNextValue();
    }
}

void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}


const juce::String NewProjectAudioProcessor::getName() const { return JucePlugin_Name; }
bool NewProjectAudioProcessor::acceptsMidi() const { return false; }
bool NewProjectAudioProcessor::producesMidi() const { return false; }
bool NewProjectAudioProcessor::isMidiEffect() const { return false; }
double NewProjectAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int NewProjectAudioProcessor::getNumPrograms() { return 1; }
int NewProjectAudioProcessor::getCurrentProgram() { return 0; }
void NewProjectAudioProcessor::setCurrentProgram (int index) {}
const juce::String NewProjectAudioProcessor::getProgramName (int index) { return {}; }
void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName) {}
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
   
        smoothedGainL.reset (sampleRate, 0.02);
        smoothedGainR.reset (sampleRate, 0.02);
        
        smoothedGainL.setCurrentAndTargetValue (1.0f);
        smoothedGainR.setCurrentAndTargetValue (1.0f);
    
}
void NewProjectAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getChannelSet (true,  0); // input bus 0
    const auto& out = layouts.getChannelSet (false, 0); // output bus 0

    // stereo out
    if (out != juce::AudioChannelSet::stereo())
        return false;

    // mono or stereo in
    if (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo())
        return true;

    return false;
}
#endif


bool NewProjectAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor() { return new NewProjectAudioProcessorEditor (*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new NewProjectAudioProcessor(); }
