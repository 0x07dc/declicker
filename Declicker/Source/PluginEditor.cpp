/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
BasicReverbAudioProcessorEditor::BasicReverbAudioProcessorEditor (BasicReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    arrowLabel1 = make_unique<Label>("arrowLabel1");
    arrowLabel1->setText("|\nV", NotificationType::dontSendNotification);
    arrowLabel1->setJustificationType(Justification::centred);
    arrowLabel1->setColour(Label::ColourIds::textColourId, Colours::yellow);
    arrowLabel1->setFont(juce::Font(8.0f, juce::Font::bold));
    addAndMakeVisible(*arrowLabel1);

    arrowLabel2 = make_unique<Label>("arrowLabel2");
    arrowLabel2->setText("|\nV", NotificationType::dontSendNotification);
    arrowLabel2->setJustificationType(Justification::centred);
    arrowLabel2->setColour(Label::ColourIds::textColourId, Colours::yellow);
    arrowLabel2->setFont(juce::Font(8.0f, juce::Font::bold));
    addAndMakeVisible(*arrowLabel2);

    smoothnessLabel = make_unique<Label>("smoothnessLabel");
    smoothnessLabel->setText(juce::CharPointer_UTF8("Smoothness (\xce\x94\ / x)"), NotificationType::dontSendNotification);
    addAndMakeVisible(*smoothnessLabel);

    maxDeltaLabel = make_unique<Label>("maxDeltaLabel");
    maxDeltaLabel->setText(juce::CharPointer_UTF8("Max \xce\x94"), NotificationType::dontSendNotification);
    addAndMakeVisible(*maxDeltaLabel);

    max2ndDeltaLabel = make_unique<Label>("max2ndDeltaLabel");
    max2ndDeltaLabel->setText(juce::CharPointer_UTF8("Max \xce\x94\xce\x94"), NotificationType::dontSendNotification);
    addAndMakeVisible(*max2ndDeltaLabel);


    manufacturerLabel = make_unique<Label>("manufacturerLabel");
    manufacturerLabel->setText("Paradigm E", NotificationType::dontSendNotification);
    manufacturerLabel->setColour(Label::ColourIds::textColourId, Colour::fromRGB(0, 62, 63));
    manufacturerLabel->setFont(getLookAndFeel().getLabelFont(*manufacturerLabel).withPointHeight(16.0f));
    addAndMakeVisible(*manufacturerLabel);


    smoothnessSlider = make_unique<Slider>("smoothnessSlider");
    smoothnessSlider->setNormalisableRange(audioProcessor.smoothnessRange);
    smoothnessSlider->getValueObject().referTo(audioProcessor.pluginSettingsState.getPropertyAsValue("smoothness", nullptr));
    smoothnessSlider->addListener(this);
    addAndMakeVisible(*smoothnessSlider);

    maxDeltaSlider = make_unique<Slider>("maxDeltaSlider");
    maxDeltaSlider->setNormalisableRange(audioProcessor.maxDeltaRange);
    maxDeltaSlider->getValueObject().referTo(audioProcessor.pluginSettingsState.getPropertyAsValue("maxDelta", nullptr));
    maxDeltaSlider->setSkewFactor(0.2);
    maxDeltaSlider->addListener(this);
    addAndMakeVisible(*maxDeltaSlider);

    max2ndDeltaSlider = make_unique<Slider>("max2ndDeltaSlider");
    max2ndDeltaSlider->setNormalisableRange(audioProcessor.max2ndDeltaRange);
    max2ndDeltaSlider->getValueObject().referTo(audioProcessor.pluginSettingsState.getPropertyAsValue("max2ndDelta", nullptr));
    maxDeltaSlider->setSkewFactor(0.2);
    max2ndDeltaSlider->addListener(this);
    addAndMakeVisible(*max2ndDeltaSlider);


    getLookAndFeel().setColour(ResizableWindow::backgroundColourId, Colour::fromRGB(0, 21, 21));

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (260, 190 + 20);
}

BasicReverbAudioProcessorEditor::~BasicReverbAudioProcessorEditor()
{
}

//==============================================================================
void BasicReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void BasicReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int fromTop = 14;
    

    smoothnessLabel->setBounds(30, fromTop, 200, 15);
    smoothnessSlider->setBounds(30, fromTop, 200, 60);

    arrowLabel1->setBounds(this->getWidth()/2 - 4, fromTop + 41, 30, 30);

    maxDeltaLabel->setBounds(30, fromTop + 60, 200, 15);
    maxDeltaSlider->setBounds(30, fromTop + 60, 200, 60);

    arrowLabel2->setBounds(this->getWidth() / 2 - 4, fromTop + 60 + 41, 30, 30);

    max2ndDeltaLabel->setBounds(30, fromTop + 60 * 2, 200, 15);
    max2ndDeltaSlider->setBounds(30, fromTop + 60 * 2, 200, 60);


    manufacturerLabel->setBounds(5, fromTop + 60 * 3 - 5, 200, 20);
}

void BasicReverbAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    float floatSafeSpace = 0.0000002f;
    float value = slider->getValue() > slider->getRange().getStart() + floatSafeSpace
        ? slider->getValue() < slider->getRange().getEnd() - floatSafeSpace
        ? slider->getValue()
        : slider->getRange().getEnd() - floatSafeSpace
        : slider->getRange().getStart() + floatSafeSpace;

    String sliderName = slider->getName();
    if (sliderName == "smoothnessSlider") {
        audioProcessor.smoothness->beginChangeGesture();
        audioProcessor.smoothness->setValueNotifyingHost(value, audioProcessor.pluginSettingsState);
        for_each(audioProcessor.filters.begin(), audioProcessor.filters.end(),
            [value](unique_ptr<Filters>& f) {
                f->smootheningFilter->setRatio(1.0f / value);
            });
        audioProcessor.smoothness->endChangeGesture();
    }
    if (sliderName == "maxDeltaSlider") {
        audioProcessor.maxDelta->beginChangeGesture();
        audioProcessor.maxDelta->setValueNotifyingHost(value, audioProcessor.pluginSettingsState);
        for_each(audioProcessor.filters.begin(), audioProcessor.filters.end(),
            [value](unique_ptr<Filters>& f) {
                f->maxDeltaFilter->setMaxDelta(value);
            });
        audioProcessor.maxDelta->endChangeGesture();
    }
    if (sliderName == "max2ndDeltaSlider") {
        audioProcessor.max2ndDelta->beginChangeGesture();
        audioProcessor.max2ndDelta->setValueNotifyingHost(value, audioProcessor.pluginSettingsState);
        for_each(audioProcessor.filters.begin(), audioProcessor.filters.end(),
            [value](unique_ptr<Filters>& f) {
                f->max2ndDeltaFilter->setMax2ndDelta(value);
            });
        audioProcessor.max2ndDelta->endChangeGesture();
    }
}
