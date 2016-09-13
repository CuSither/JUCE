#include "AudioPlayerPlugin.h"
#include "AudioPlayerEditor.h"

//==============================================================================
AudioPlayerPlugin::AudioPlayerPlugin() : transportState(TransportState::NoFile), readAheadThread("AUDIO_READER")
{
	busArrangement.inputBuses.clear();
	busArrangement.outputBuses.clear();
	busArrangement.outputBuses.add(AudioProcessorBus("player", AudioChannelSet::stereo()));
	readAheadThread.startThread(0);
	formatManager.registerBasicFormats();
}

AudioPlayerPlugin::~AudioPlayerPlugin()
{
}

//==============================================================================
bool AudioPlayerPlugin::setPreferredBusArrangement(bool isInputBus, int busIdx, const AudioChannelSet& preferredLayout)
{
	const int numChannels = preferredLayout.size();

	// accept fixed channels only
	if (isInputBus && numChannels != getTotalNumInputChannels())
		return false;
	if (!isInputBus && numChannels != getTotalNumOutputChannels())
		return false;

	// when accepting a layout, always fall through to the base class
	return AudioProcessor::setPreferredBusArrangement(isInputBus, busIdx, preferredLayout);
}

bool AudioPlayerPlugin::openFile(File file) {
    String* filename = new String(file.getFullPathName());
    if (file.exists()) {
        Logger::outputDebugString(String("opening file for reading: ") += *filename);
        AudioFormatReader* bufferedReader;
        try {
            AudioFormatReader* reader = formatManager.createReaderFor(file);
            if (reader != nullptr)
                bufferedReader = new BufferingAudioReader(reader, readAheadThread, READ_AHEAD_SAMPLES);
            else
                return false;
        }
        catch (...) {
            return false;
        }

        if (bufferedReader != nullptr)
        {
            Logger::outputDebugString("create reader");
            ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(bufferedReader, true);
            newSource->prepareToPlay(getBlockSize(), getSampleRate());
            audioSource = newSource.release();
        }
        else
            return false;
    }
    else
        return false;

    return true;
}

void AudioPlayerPlugin::prepareToPlay(double samples, int estimatedSamplesPerBlock)
{
	if (audioSource)
		audioSource->prepareToPlay(estimatedSamplesPerBlock, samples);
}

void AudioPlayerPlugin::releaseResources()
{
	if (audioSource)
		audioSource->releaseResources();
}

void AudioPlayerPlugin::fillInPluginDescription(PluginDescription & d) const
{
	d.name = getName();
	d.uid = d.name.hashCode();
	d.category = "Input devices";
	d.pluginFormatName = "Internal";
	d.manufacturerName = "mycompany.com";
	d.version = "1.0";
	d.isInstrument = false;
	d.numInputChannels = getTotalNumInputChannels();
	d.numOutputChannels = getTotalNumOutputChannels();
}

void AudioPlayerPlugin::processBlock (AudioSampleBuffer& buffer, MidiBuffer&)
{
	// switch state requested from UI
	if (transportState == TransportState::Starting) {
		changeState(TransportState::Playing);
	}
	if (transportState == TransportState::Stopping) {
		const ScopedLock sl(stateLock); 
		changeState(TransportState::Stopped);
		audioSource->setNextReadPosition(0);
	}
	if (transportState == TransportState::Pausing) {
		const ScopedLock sl(stateLock); 
		changeState(TransportState::Paused);
	}

	// read block, if currently playing
	if (audioSource && transportState == TransportState::Playing) {
		const ScopedLock sl(stateLock);
		// play next block
		audioSource->getNextAudioBlock(AudioSourceChannelInfo(buffer));

		int64 len = audioSource->getTotalLength();
		int64 pos = audioSource->getNextReadPosition();

		// stop at the end
		if (pos >= len) {
			changeState(TransportState::Stopping);
		}

	} else 	{
		// clear buffer to avoid noise when not playing
		const ScopedLock sl(stateLock);
		for (int i = 0; i < getTotalNumOutputChannels(); ++i)
			buffer.clear(i, 0, buffer.getNumSamples());
	}
}

AudioProcessorEditor* AudioPlayerPlugin::createEditor()
{
    return new AudioPlayerEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPlayerPlugin();
}
