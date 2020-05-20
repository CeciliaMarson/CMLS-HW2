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

    
    //This function are useful to process the fft and its spectrum
    void processAudioBlock(const AudioBuffer<float>& buffer);
    void timerCallback() override;
    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawNextLineOfSpectrogram();
    
    //This are the variables related to the fft
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
    
   
    
    
    
private:
    TUNERAudioProcessor& processor; 
    
    //Inizialisation of the GUI element: the slider and a label that shows the pitch
    //and the relative function that change the number of harmonics.
    Slider harmonics;
    Label labelHarmonics, labelPitch;

    void sliderValueChanged(Slider* slider) override;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TUNERAudioProcessorEditor)
};