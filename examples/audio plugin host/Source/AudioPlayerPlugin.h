/*
  ==============================================================================

  An AudioPlayerPlugin for simple playback of audio files.
  Used as InternalFilter for the Plugin Host.

  ==============================================================================
*/

#ifndef AUDIOPLAYERPLUGIN_H_INCLUDED
#define AUDIOPLAYERPLUGIN_H_INCLUDED

#include "JuceHeader.h"

/**
*/
enum class TransportState
{
	// order matters, operators like > are used!
	NoFile,
	Stopped,
	Stopping,
	Paused,
	Pausing,
	Starting,
	Playing
};

class AudioPlayerPlugin : public AudioPluginInstance
{
public:
    AudioPlayerPlugin();
    ~AudioPlayerPlugin();

	TransportState getTransportState() { return transportState; }
	bool isPlaying() { return transportState == TransportState::Playing; }

	AudioFormatReaderSource* getAudioSource() { return audioSource.get(); }

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const String getName() const override {	return "Audiofile Player"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; } // some hosts don't cope well with 0 programs
    int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
    const String getProgramName (int) override { return String(); }
	void changeProgramName(int, const String&) override {}

	void getStateInformation(MemoryBlock&) override{};
	void setStateInformation(const void*, int) override{};

	virtual void fillInPluginDescription(PluginDescription &) const override;

protected:
	friend class AudioPlayerEditor;
	void changeState(TransportState newState) { const ScopedLock sl(stateLock); transportState = newState; }
	bool openFile(File);

private:
	const unsigned READ_AHEAD_SAMPLES = 65536; // bytes to read ahead - avoids distortions by UI update

	TimeSliceThread readAheadThread;
	TransportState transportState;
	ScopedPointer<AudioFormatReaderSource> audioSource = nullptr;
	AudioFormatManager formatManager;
	CriticalSection stateLock;

	bool setPreferredBusArrangement(bool, int, const AudioChannelSet&);
	virtual void prepareToPlay(double, int) override;
	virtual void releaseResources() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayerPlugin)
};


#endif  // AUDIOPLAYERPLUGIN_H_INCLUDED
