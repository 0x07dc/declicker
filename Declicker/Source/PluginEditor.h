/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace juce;

//==============================================================================
/**
*/
class BasicReverbAudioProcessorEditor  : public juce::AudioProcessorEditor, public Slider::Listener
{
public:
    BasicReverbAudioProcessorEditor (BasicReverbAudioProcessor&);
    ~BasicReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    unique_ptr<Label> arrowLabel1;
    unique_ptr<Label> arrowLabel2;

    unique_ptr<Label> maxDeltaLabel;
    unique_ptr<Label> smoothnessLabel;
    unique_ptr<Label> max2ndDeltaLabel;
    unique_ptr<Label> manufacturerLabel;

    unique_ptr<Slider> maxDeltaSlider;
    unique_ptr<Slider> smoothnessSlider;
    unique_ptr<Slider> max2ndDeltaSlider;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicReverbAudioProcessor& audioProcessor;

    // Inherited via Listener
    virtual void sliderValueChanged(Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicReverbAudioProcessorEditor)

};
