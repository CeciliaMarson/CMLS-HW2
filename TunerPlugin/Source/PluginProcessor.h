/*
  ==============================================================================
    This file was auto-generated!
    It contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define SAMPLE_RATE  (44100)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif


#include <string.h>
#include <stdlib.h>
#include "math.h"
//==============================================================================
/**
*/


class TUNERAudioProcessor : public AudioProcessor, private Timer
{
public:
    //==============================================================================
    TUNERAudioProcessor();
    ~TUNERAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void pushNextSampleIntoFifo(float sample) noexcept;
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif
    
   //The same of the editor

    void timerCallback();
    void NextSampleIntoFifo(float sample) noexcept;
    void NextLineOfSpectrogram();

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;


    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // =============================================================================
    // List of controls parameters
    //the same of the editor, we will calculate the fft in the same way as the editor
    dsp::FFT forwardFFT1;
    static constexpr auto fftOrder = 10;            // [1]
    static constexpr auto fftSize = 1 << fftOrder; // [2]
    float fifo[fftSize];
    float* fftData;

    bool nextFFTBlockReady = false;
    
    //this function and variable are useful to estimate the pitch value

    void generateCost(float* X, float* f0Area, int L, int bufsize, int f0AreaSize, int fs, float* Cost);

    float estimatePitch(float* f0Area, int f0AreaSize);

    float f0Min, f0Max, f0Res, pitchEstimate, gain;
    float* f0Area;
    float* X;
    int f0AreaSize, numberOfHarmonics;
    float* Cost;

    String sign;
    
    float num;
    int midinum;

    String pitchText;


private:
    

    int fifoIndex = 0;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TUNERAudioProcessor)
};