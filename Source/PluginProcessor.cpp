/*
  ==============================================================================

    Kompressor Plugin
    This file processes the incoming audio

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KompressorAudioProcessor::KompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
}

KompressorAudioProcessor::~KompressorAudioProcessor()
{
}

//==============================================================================
const juce::String KompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KompressorAudioProcessor::acceptsMidi() const
{
    return false;
}

bool KompressorAudioProcessor::producesMidi() const
{
    return false;
}

bool KompressorAudioProcessor::isMidiEffect() const
{
    return false;
}

double KompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String KompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void KompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void KompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void KompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void KompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels(); // will always be 1 for this plugin
    auto totalNumOutputChannels = getTotalNumOutputChannels(); // will always be 1 for this plugin
  
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
  
    if (bypass)
      return; // no need to process anything.
    
    // NATE: Here we take the input parameters (in the .h file) and
    // compute some intermediate values we need to process the samples.
    // Calculate linear values and LPF coefficients
    float threshold_lin = std::pow(10.0f, threshold_dB / 20.0f);
    float makeup_gain_lin = std::pow(10.0f, makeup_gain_dB / 20.0f);

    // Attack/Release coefficients for a 48kHz sample rate (adjust based on sample rate)
    float alpha_attack = 0.999f;  // Fast: ~2ms time constant
    float alpha_release = 0.9999f; // Slow: ~20ms time constant (longer release for guitar usually)

    // NATE: ok, now lets process the this buffer of audio samples!
  
    float* channelData = buffer.getWritePointer (0); // <-- '0' to access first channel.
    // since we marked this plugin as being MONO in and MONO out we can assume
    // there is only one channenl of audio to process.
  
    // NATE: channelData is a 'pointer' to an array of float samples,
    // the size how large this buffer is (buffer.getNumSamples()) let's
    int numSamplesToProcess = buffer.getNumSamples();
  
    // compression algorithm
    for (int i = 0; i < numSamplesToProcess; ++i)
    {
        float current_sample = *channelData;
        float abs_sample = std::fabs(current_sample);

        // 1. Detection (Envelope Follower)
        if (abs_sample > envelope_level)
        {
            // Attack (when level increases)
            envelope_level = (alpha_attack * envelope_level) + ((1.0f - alpha_attack) * abs_sample);
        }
        else
        {
            // Release (when level decreases)
            envelope_level = (alpha_release * envelope_level) + ((1.0f - alpha_release) * abs_sample);
        }

        // --- Clamp the minimum level to prevent log(0) and noise floor issues ---
        if (envelope_level < 1e-6f)
            envelope_level = 1e-6f;

        // 2. Gain Calculation (dB Domain)
        float linear_gain = 1.0f;
        
        // Only apply compression if the signal is above the threshold
        if (envelope_level > threshold_lin)
        {
            // Convert level to dB (logarithmic)
            float level_dB = 20.0f * std::log10(envelope_level);
            
            // Calculate compression gain reduction in dB: G_dB = L_out_dB - L_in_dB
            float gain_reduction_dB = (level_dB - threshold_dB) * (1.0f / ratio - 1.0f);
            
            // Convert gain back to linear
            linear_gain = std::pow(10.0f, gain_reduction_dB / 20.0f);
        }
        
        // Apply final gain, including static make-up gain
        linear_gain *= makeup_gain_lin;

        // 3. Gain Application (overwrite the sample)
        *channelData = current_sample * linear_gain;
        
        // Move to the next sample in the buffer
        channelData++;
      
    } // loop over each sample
}

//==============================================================================
bool KompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* KompressorAudioProcessor::createEditor()
{
    return new KompressorAudioProcessorEditor (*this);
}

//==============================================================================
void KompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // NATE : This is where you would SAVE the plugin parameters. The DAW stores
    // these with the project.
  
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void KompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // NATE : This is where you would load the plugin parameters. The DAW stores
    // these with the project.

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KompressorAudioProcessor();
}
