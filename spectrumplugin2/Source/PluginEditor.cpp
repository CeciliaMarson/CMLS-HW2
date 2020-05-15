/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================

//==============================================================================
Spectrumplugin2AudioProcessorEditor::Spectrumplugin2AudioProcessorEditor (Spectrumplugin2AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
    
{
    addAndMakeVisible(&m_SpectroGramComp);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    
    
}

Spectrumplugin2AudioProcessorEditor::~Spectrumplugin2AudioProcessorEditor()
{
}

//==============================================================================
void Spectrumplugin2AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::green);
    
    
}

void Spectrumplugin2AudioProcessorEditor::resized()
{
    m_SpectroGramComp.setBounds(0, 0, getWidth(), getHeight());

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

