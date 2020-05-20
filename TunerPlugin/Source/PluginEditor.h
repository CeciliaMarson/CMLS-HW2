/*
  ==============================================================================
    This file was auto-generated!
    It contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class TUNERAudioProcessorEditor : public AudioProcessorEditor,
    private Timer, Slider::Listener

{
public:
    TUNERAudioProcessorEditor(TUNERAudioProcessor&);
    ~TUNERAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
    //SpectrogramComponent m_SpectroGramComp;
    void processAudioBlock(const AudioBuffer<float>& buffer);
    void timerCallback() override;
    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextLineOfSpectrogram();
    
    enum
    {
        fftOrder = 10,
        fftSize = 1 << fftOrder
    };


    dsp::FFT forwardFFT;
    Image spectrogramImage;

    float fifo[fftSize];
    float fftData[2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    
    String pitch;
    
    
    
private:
    TUNERAudioProcessor& processor;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    //Spectrumplugin2AudioProcessor& processor;

    Slider harmonics;
    Label labelHarmonics, labelPitch;

    void sliderValueChanged(Slider* slider) override;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TUNERAudioProcessorEditor)
};