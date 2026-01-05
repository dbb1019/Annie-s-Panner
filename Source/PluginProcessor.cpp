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


juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
   
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"pan", 1}, "Pan", -100.0f, 100.0f, 0.0f));
    

//    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"width", 1}, "Width", 0.0f, 100.0f, 100.0f));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID {"panMode", 1}, "Pan Mode",
                                                                juce::StringArray {"Balance", "Stereo Pan"}, 0));
        //return layout;
    return layout;
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
   
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
    // map -100~100 to -1.0~1.0
    float pan = juce::jlimit(-1.0f, 1.0f, panValue / 100.0f);
    int mode = (int)apvts.getRawParameterValue("panMode")->load();

    if (numIn == 1) {
        // Mono - Equal Power
        float normPan = (pan + 1.0f) * 0.5f;
        float angle = normPan * juce::MathConstants<float>::halfPi;
        smoothedGainL.setTargetValue(std::cos(angle));
        smoothedGainR.setTargetValue(std::sin(angle));
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
    
    else {
        if (mode == 0) { // Balance Mode
            smoothedGainL.setTargetValue(juce::jmin(1.0f, 1.0f - pan));
            smoothedGainR.setTargetValue(juce::jmin(1.0f, 1.0f + pan));
        }
        
        else if (mode == 1) { //Stereo Pan - Width Spread
            
                    smoothedGainL.setCurrentAndTargetValue(1.0f);
                    smoothedGainR.setCurrentAndTargetValue(1.0f);
                    
                    // 1. 计算随动宽度：Pan 在中间时为 1.0，Pan 到极值时为 0.0
                    float dynamicWidth = 1.0f - std::abs(pan);

                    // 2. 计算左右声道的“逻辑位置”
                    // 当 pan=0, width=1 -> posL=-1, posR=1 (全立体声)
                    // 当 pan=1, width=0 -> posL=1, posR=1 (右边单声道)
                    float posL = juce::jlimit(-1.0f, 1.0f, pan - dynamicWidth);
                    float posR = juce::jlimit(-1.0f, 1.0f, pan + dynamicWidth);

                    // 3. 等功率平移计算函数
                    auto calculateGains = [](float pos, float& leftGain, float& rightGain) {
                        float norm = (pos + 1.0f) * 0.5f;
                        float angle = norm * juce::MathConstants<float>::halfPi;
                        leftGain = std::cos(angle);
                        rightGain = std::sin(angle);
                    };

                    float tLL, tLR, tRL, tRR;
                    calculateGains(posL, tLL, tLR); // 左输入在输出中的分布
                    calculateGains(posR, tRL, tRR); // 右输入在输出中的分布
            
                    float compensation = 1.0f - (std::abs(pan) * (1.0f - 0.707f));

                    gLL.setTargetValue(tLL * compensation);
                    gLR.setTargetValue(tLR * compensation);
                    gRL.setTargetValue(tRL * compensation);
                    gRR.setTargetValue(tRR * compensation);
                    
                    auto* outL = buffer.getWritePointer(0);
                    auto* outR = buffer.getWritePointer(1);
            

                    for (int i = 0; i < numSamples; ++i) {
                        float inL = outL[i];
                        float inR = outR[i];
                        
                        // 应用 4 路增益混合
                        outL[i] = (inL * gLL.getNextValue()) + (inR * gRL.getNextValue());
                        outR[i] = (inL * gLR.getNextValue()) + (inR * gRR.getNextValue());
                    }
                    
                    return;
            
                }
        
//        else if (mode == 1) { // Stereo Pan (Linked M/S)
        
//            float normPan = (pan + 1.0f) * 0.5f;
//            float angle = normPan * juce::MathConstants<float>::halfPi;
//
//            smoothedGainL.setTargetValue(std::cos(angle) * 1.414f);
//            smoothedGainR.setTargetValue(std::sin(angle) * 1.414f);
//
//            float sideTarget = std::cos(pan * juce::MathConstants<float>::pi * 0.25f);
//            smoothedSideGain.setTargetValue(sideTarget);
//
//            auto* outL = buffer.getWritePointer(0);
//            auto* outR = buffer.getWritePointer(1);
//
//            for (int i = 0; i < numSamples; ++i)
//            {
//                float left = outL[i];
//                float right = outR[i];
//
//                float mid = (left + right) * 0.5f;
//                float side = (left - right) * 0.5f;
//
//                float curMidL = smoothedGainL.getNextValue();
//                float curMidR = smoothedGainR.getNextValue();
//                float curSide = smoothedSideGain.getNextValue();
//
//
//                outL[i] = (mid * curMidL) + (side * curSide);
//                outR[i] = (mid * curMidR) - (side * curSide);
//            }
//            return;
//        }
        
//        else { // Equal Power Mode (mode == 2) - just for fun haha it seems not good
//            float normPan = (pan + 1.0f) * 0.5f;
//            float angle = normPan * juce::MathConstants<float>::halfPi;
//            smoothedGainL.setTargetValue(std::cos(angle));
//            smoothedGainR.setTargetValue(std::sin(angle));
//        }
    }

   
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
   
        gLL.reset(sampleRate, 0.02);
        gLR.reset(sampleRate, 0.02);
        gRL.reset(sampleRate, 0.02);
        gRR.reset(sampleRate, 0.02);
    
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
