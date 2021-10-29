/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicReverbAudioProcessor::BasicReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    pluginSettingsState.setProperty("name", "pluginSettingsState", nullptr);

    addParameter(smoothness = new CustomParameter());
    smoothness->setName("smoothness");
    smoothness->setValue(defaultSmoothness, pluginSettingsState);

    addParameter(maxDelta = new CustomParameter());
    maxDelta->setName("maxDelta");
    maxDelta->setValue(defaultMaxDelta, pluginSettingsState);

    addParameter(max2ndDelta = new CustomParameter());
    max2ndDelta->setName("max2ndDelta");
    max2ndDelta->setValue(defaultMax2ndDelta, pluginSettingsState);
    


}

BasicReverbAudioProcessor::~BasicReverbAudioProcessor()
{

}

//==============================================================================
const juce::String BasicReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BasicReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int sampleRate = getSampleRate();



    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    int pastSize = filters.size();
    filters.resize(totalNumInputChannels);
    for (int i = pastSize; i < totalNumInputChannels; ++i) {
        filters[i] = move(make_unique<Filters>());
    }
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        filters[channel]->setSampleRate(sampleRate);
        
        const float* readChannelData = buffer.getReadPointer(channel);
        const int numSamples = buffer.getNumSamples();
        vector<float> newData(numSamples);
        copy(readChannelData, readChannelData + numSamples, newData.begin());

        // ..do something to the data...
        float* writeChannelData = buffer.getWritePointer(channel);
        for (int i = 0; i < numSamples; ++i) {
            newData[i] = filters[channel]->smootheningFilter->getNextSample(newData[i]);
            newData[i] = filters[channel]->maxDeltaFilter->getNextSample(newData[i]);
            newData[i] = filters[channel]->max2ndDeltaFilter->getNextSample(newData[i]);
        }
        copy(newData.begin(), newData.end(), writeChannelData);

        
    }
}

//==============================================================================
bool BasicReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicReverbAudioProcessor::createEditor()
{
    return new BasicReverbAudioProcessorEditor (*this);
}

//==============================================================================
void BasicReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    unique_ptr<juce::XmlElement> xml(pluginSettingsState.createXml());
    String testXML = xml->toString();
    copyXmlToBinary(*xml, destData);
}

void BasicReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName("pluginSettingsState")) {
            ValueTree valueTree = ValueTree::fromXml(*xmlState);

            maxDelta->setValue(valueTree.getProperty("maxDelta"), pluginSettingsState);
            smoothness->setValue(valueTree.getProperty("smoothness"), pluginSettingsState);
            max2ndDelta->setValue(valueTree.getProperty("max2ndDelta"), pluginSettingsState);
        }
        
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicReverbAudioProcessor();
}
