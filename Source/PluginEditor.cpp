/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KompressorAudioProcessorEditor::KompressorAudioProcessorEditor (KompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
  
  // Threashold knob + label setup
  thresholdSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
  thresholdSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 50, 20); // Show value below knob
  thresholdSlider.setRange (-60.0, 0.0, 0.1); // -60 dB to 0 dB, steps of 0.1
  addAndMakeVisible (thresholdSlider);
  thresholdSlider.onValueChange = [this]
  {
    // Retrieve the current value from the slider
    float currentValue = (float) thresholdSlider.getValue();
    audioProcessor.setTreshold(currentValue);
  };
  
  // Configure the Threshold Label
  thresholdLabel.setText ("Threshold (dB)", juce::dontSendNotification);
  thresholdLabel.attachToComponent (&thresholdSlider, false);
  addAndMakeVisible (thresholdLabel);
  
  // Bypass button setup
  bypassButton.onClick = [this] {
    bool isBypassed = bypassButton.getToggleState();
    
    audioProcessor.setBypass(isBypassed); // set the value in the processor
    
    juce::String logMessage;
            if (isBypassed)
            {
                logMessage = "Bypass Button Toggled ON. Compressor is now BYPASSED.";
            }
            else
            {
                logMessage = "Bypass Button Toggled OFF. Compressor is now ACTIVE.";
            }

            // Output the message to the JUCE logging system (usually the IDE console)
            juce::Logger::writeToLog (logMessage);
    };
    bypassButton.setClickingTogglesState (true); // Important: Make it a latching/toggle button
    bypassButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red.withAlpha(0.8f));
    addAndMakeVisible (bypassButton);

    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

KompressorAudioProcessorEditor::~KompressorAudioProcessorEditor()
{
}

//==============================================================================
void KompressorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("TODO : Draw some cool UI to adjust KOMPRESSOR", getLocalBounds(), juce::Justification::centredTop, 1);
    g.drawFittedText ("Another value plugin from Pixel Rocket", getLocalBounds(), juce::Justification::centredBottom, 1);
}

void KompressorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
  
    // Simple layout: center the button
    bypassButton.setBounds (
        getWidth() / 2 - 50,
        getHeight() / 2 - 15,
        100,
        30
    );
  
  thresholdSlider.setBounds (
          10, //  X position
          getHeight() / 2 - 80, // Position above the bypass button
          100, // Width
          100 // Height (Makes it a square area for the knob and text box)
    );
}
