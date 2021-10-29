/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomParameter.h"
#include "Filters.h"

using namespace std;

//==============================================================================
/**
*/
class BasicReverbAudioProcessor  : public juce::AudioProcessor
{
    /// <summary>
    /// One filter per each channel
    /// </summary>
    vector<unique_ptr<Filters>> filters;
    friend class BasicReverbAudioProcessorEditor;
public:


    //==============================================================================
    BasicReverbAudioProcessor();
    ~BasicReverbAudioProcessor() override;

    float defaultSmoothness = 3.0f;
    float floatSafeZone = 0.1f;
    NormalisableRange<double> smoothnessRange{ 1.0, 10.0, 0.1 };
    CustomParameter* smoothness;

    float defaultMaxDelta = 0.12f;
    NormalisableRange<double> maxDeltaRange{ 0.0, 0.12, 0.00001 };
    CustomParameter* maxDelta;
    
    float defaultMax2ndDelta = 0.2f;
    NormalisableRange<double> max2ndDeltaRange{ 0.005, 0.2, 0.000001 };
    CustomParameter* max2ndDelta;
    

    ValueTree pluginSettingsState = ValueTree(Identifier("pluginSettingsState"));
    

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicReverbAudioProcessor)
};
