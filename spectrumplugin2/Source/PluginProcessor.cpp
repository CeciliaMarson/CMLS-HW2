/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "math.h"

//==============================================================================
Spectrumplugin2AudioProcessor::Spectrumplugin2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
,forwardFFT1(fftOrder)
{
  

}

Spectrumplugin2AudioProcessor::~Spectrumplugin2AudioProcessor()
{
 
}


//==============================================================================
const String Spectrumplugin2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}



bool Spectrumplugin2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Spectrumplugin2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Spectrumplugin2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}


double Spectrumplugin2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Spectrumplugin2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Spectrumplugin2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Spectrumplugin2AudioProcessor::setCurrentProgram (int index)
{
}

const String Spectrumplugin2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Spectrumplugin2AudioProcessor::changeProgramName(int index, const String& newName)
{

}

//==============================================================================
void Spectrumplugin2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    numberOfHarmonics = 5;
    gain = 1.0f;
    X = (double *)malloc(sizeof(double)*fftSize);
    fftData=(float *)malloc(sizeof(float)*2*fftSize);
    Cost = (float *)malloc(f0AreaSize*sizeof(float));
    
    // Create Search Grid
    f0Res=0.05f;
    f0Max=670.0f;
    f0Min=80.0f;
    
    f0AreaSize = round((f0Max-f0Min)/f0Res+1);
    f0Area = (float *)malloc(f0AreaSize*sizeof(float));
    f0Area[0] = f0Min;
    for (int i = 1; i < f0AreaSize; i++) {
        f0Area[i] = f0Area[i-1] + f0Res;
    }

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Spectrumplugin2AudioProcessor::releaseResources()
{
    
    free(X);
    free(f0Area);
    free(Cost);
}



    //==============================================================================

void Spectrumplugin2AudioProcessor::timerCallback() 
{
        if (nextFFTBlockReady)
        {
            NextLineOfSpectrogram();
            nextFFTBlockReady = false; 
              for (int i = 0; i < fftSize; i++) {
                    X[i] = (20 * log10(fftData[i]));
                }
            generateCost(X, f0Area, numberOfHarmonics, fftSize, f0AreaSize, SAMPLE_RATE);
            pitchEstimate = estimatePitch(f0Area, f0AreaSize);
    
            pitchText = String (pitchEstimate, 1);
              
                
        }
    }



void Spectrumplugin2AudioProcessor::NextSampleIntoFifo (float sample) noexcept
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
    
void Spectrumplugin2AudioProcessor::NextLineOfSpectrogram()
{
    forwardFFT1.performFrequencyOnlyForwardTransform (fftData);
}

void Spectrumplugin2AudioProcessor::generateCost(double* X, float* f0Area, int L, int bufsize, int f0AreaSize, int fs){
    int fIndex;     // One sample of F0Area
    
    for (int n = 0; n < f0AreaSize; n++)
    {
        fIndex = (int)floor(f0Area[n]*(1.f* bufsize /fs)+1);
        Cost[n] = X[fIndex];

        for (int l = 2; l <= L; l++)
        {
            fIndex = (int)floor(f0Area[n]*l*(1.f* bufsize /fs)+1);
            Cost[n] += X[fIndex];
        }
    }
}

float Spectrumplugin2AudioProcessor::estimatePitch (float* f0Area, int f0AreaSize) {
    int argument = 0;
    float maxVal = -32000;
    float pitchEstimate;
    
    for (int i = 0; i < f0AreaSize; i++)
    {
        if (maxVal < Cost[i]) {
            maxVal = Cost[i];
            argument = i;
        }
    }
    pitchEstimate = f0Area[argument];

    return pitchEstimate;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Spectrumplugin2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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
void Spectrumplugin2AudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    int bufsize = buffer.getNumSamples();
    
    if (buffer.getNumChannels() > 0)
		{
	
			const float* channelData = buffer.getReadPointer(0);
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                NextSampleIntoFifo(channelData[i]);
                
            }
		}
		
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
	auto myeditor = dynamic_cast<Spectrumplugin2AudioProcessorEditor*>(getActiveEditor());
	if (myeditor != nullptr)
	{
		myeditor->m_SpectroGramComp.processAudioBlock(buffer);
	}
   
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        buffer.applyGain (channel, 0, bufsize, gain);
    }
   
    


        // ..do something to the data...
    }


//==============================================================================
bool Spectrumplugin2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Spectrumplugin2AudioProcessor::createEditor()
{
    return new Spectrumplugin2AudioProcessorEditor (*this);
}

//==============================================================================
void Spectrumplugin2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Spectrumplugin2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Spectrumplugin2AudioProcessor();
}
