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
TUNERAudioProcessorEditor::TUNERAudioProcessorEditor(TUNERAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), forwardFFT(fftOrder),
    spectrogramImage(Image::RGB, 400, 300, true), fifoIndex(0), nextFFTBlockReady(false)

{
    //addAndMakeVisible(&m_SpectroGramComp);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 400);

    labelHarmonics.setText("Harmonics", dontSendNotification);
    
    

    harmonics.setRange(1, 15, 1);
    harmonics.setValue(5);
    harmonics.setSliderStyle(Slider::Rotary);
    harmonics.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    harmonics.addListener(this);
    labelPitch.setText((processor.pitchText + " " + processor.sign), sendNotification);
    
    addAndMakeVisible(harmonics);
    addAndMakeVisible(labelHarmonics);
    addAndMakeVisible(labelPitch);

    setOpaque(true);
    startTimerHz(60);
    //setAudioChannels(2, 0);  // we want a couple of input channels but no outputs   

}

TUNERAudioProcessorEditor::~TUNERAudioProcessorEditor()
{

}

void TUNERAudioProcessorEditor::processAudioBlock(const AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() > 0)
    {
        const float* channelData = buffer.getReadPointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            pushNextSampleIntoFifo(channelData[i]);
    }
}

//==============================================================================
void TUNERAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll();// Colours::black);

    g.setOpacity(1.0f);
    g.drawImage(spectrogramImage, getLocalBounds().toFloat());

    g.setColour(Colours::white);
    g.setFont(20.0f);
    //g.drawFittedText(processor.pitchText, juce::Rectangle<int>(500, 650), Justification::centred, 1);
}

void TUNERAudioProcessorEditor::resized()
{
    //m_SpectroGramComp.setBounds(0, 0, getWidth(), getHeight());

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    labelHarmonics.setBounds(30, 280, 130, 20);
    harmonics.setBounds(20, 290, 100, 100);

    labelPitch.setBounds(200, 310, 300, 20);

}

void TUNERAudioProcessorEditor::timerCallback()
{
    if (nextFFTBlockReady)
    {
        drawNextLineOfSpectrogram();
        
        nextFFTBlockReady = false;
        repaint();
    }
    labelPitch.setText((processor.pitchText + " " + processor.sign), sendNotification);
}

void TUNERAudioProcessorEditor::pushNextSampleIntoFifo(float sample) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next line should now be rendered..
    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;
}

void TUNERAudioProcessorEditor::drawNextLineOfSpectrogram()
{
    auto rightHandEdge = spectrogramImage.getWidth() - 1;
    auto imageHeight = spectrogramImage.getHeight() - 100;

    // first, shuffle our image leftwards by 1 pixel..
    spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

    // then render our FFT data..
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);

    // find the range of values produced, so we can scale our rendering to
    // show up the detail clearly
    auto maxLevel = FloatVectorOperations::findMinAndMax(fftData, fftSize / 2);

    for (auto y = 1; y < imageHeight; ++y)
    {
        auto skewedProportionY = 1.0f - std::exp(std::log(y / (float)imageHeight) * 0.2f);
        auto fftDataIndex = jlimit(0, fftSize / 2, (int)(skewedProportionY * fftSize / 2));
        auto level = jmap(fftData[fftDataIndex], 0.0f, jmax(maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);

        spectrogramImage.setPixelAt(rightHandEdge, y, Colour::fromHSV(level, 1.0f, level, 1.0f));
    }
}

void TUNERAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    processor.numberOfHarmonics = harmonics.getValue();
}
