/*
  ==============================================================================

 Kompressor Plugin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// this class is instantiated ONCE per plugin instance. We use it
// to store variables for example knobs and buttons in the user interface
// that would affect the audio processing.

class KompressorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    KompressorAudioProcessor();
    ~KompressorAudioProcessor() override;

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
    // NATE: these methods will load and store the plugin-state from the DAW. 
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
  
    void setBypass(bool status) {
      bypass = status;
    }

private:

    // Compression parameters -
    // NATE : we would want to create UI to adjust these probably?
    float threshold_dB = -18.0f; // Typical value for guitar
    float ratio = 4.0f;          // 4:1 ratio
    float makeup_gain_dB = 6.0f; // 6 dB of make-up
    float envelope_level = 0.0f; // L[n-1], stored state
    
    bool bypass = false;  // pass audio through without processing.
    
  
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KompressorAudioProcessor)
};
