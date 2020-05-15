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

class SpectrogramComponent   : public Component,
                               private Timer
{
public:
    SpectrogramComponent()
		: forwardFFT(fftOrder),
		spectrogramImage(Image::RGB, 400, 300, true),
		fifoIndex(0),
		nextFFTBlockReady(false)
	{
		setOpaque(true);
		startTimerHz(60);
	}

	~SpectrogramComponent()
	{
		stopTimer();
	}

    //==============================================================================
  

  	void processAudioBlock(const AudioBuffer<float>& buffer)
	{
		if (buffer.getNumChannels() > 0)
		{
			const float* channelData = buffer.getReadPointer(0);
			for (int i = 0; i < buffer.getNumSamples(); ++i)
				pushNextSampleIntoFifo(channelData[i]);
		}
	}


    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);

        g.setOpacity (1.0f);
        g.drawImage (spectrogramImage, getLocalBounds().toFloat());
    }

    void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextLineOfSpectrogram();
            nextFFTBlockReady = false;
            repaint();
        }
    }

    void pushNextSampleIntoFifo (float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next line should now be rendered..
        if (fifoIndex == fftSize)
        {
            if (! nextFFTBlockReady)
            {
                zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;
    }

    void drawNextLineOfSpectrogram()
    {
        auto rightHandEdge = spectrogramImage.getWidth() - 1;
        auto imageHeight   = spectrogramImage.getHeight();

        // first, shuffle our image leftwards by 1 pixel..
        spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

        // then render our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform (fftData);

        // find the range of values produced, so we can scale our rendering to
        // show up the detail clearly
        auto maxLevel = FloatVectorOperations::findMinAndMax (fftData, fftSize / 2);

        for (auto y = 1; y < imageHeight; ++y)
        {
            auto skewedProportionY = 1.0f - std::exp (std::log (y / (float) imageHeight) * 0.2f);
            auto fftDataIndex = jlimit (0, fftSize / 2, (int) (skewedProportionY * fftSize / 2));
            auto level = jmap (fftData[fftDataIndex], 0.0f, jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

            spectrogramImage.setPixelAt (rightHandEdge, y, Colour::fromHSV (level, 1.0f, level, 1.0f));
        }
    }

    enum
    {
        fftOrder = 10,
        fftSize  = 1 << fftOrder
    };
    
    

private:
    dsp::FFT forwardFFT;
    Image spectrogramImage;

    float fifo [fftSize];
    float fftData [2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramComponent)
};

class Spectrumplugin2AudioProcessorEditor  : public AudioProcessorEditor
                                
{                                   
public:
    Spectrumplugin2AudioProcessorEditor (Spectrumplugin2AudioProcessor&);
    ~Spectrumplugin2AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    SpectrogramComponent m_SpectroGramComp;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Spectrumplugin2AudioProcessor& processor;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spectrumplugin2AudioProcessorEditor)
};
