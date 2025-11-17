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
  
  // NATE : This is the button callback
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
}
