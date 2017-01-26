#ifndef AUDIOSLAVERECORDERPLUGIN_H_INCLUDED
#define AUDIOSLAVERECORDERPLUGIN_H_INCLUDED

#include "JuceHeader.h"
#include "TransportState.h"

/**
    Recorder for the AudioPlayerPlugin which is used as InternalFilter for the Plugin Host.
    It can record a file and is controlled from the AudioPlayerPlugin (indirectly also from the AudioPlayerEditor).
    This recorder registers itself at a SharedResourcePointer<AudioPlayerPluginSharedData>.

    @see AudioPlayerPlugin, AudioPlayerEditor, AudioPlayerPluginSharedData
*/
class AudioSlaveRecorderPlugin : public AudioPluginInstance
{
public:
	AudioSlaveRecorderPlugin();
	~AudioSlaveRecorderPlugin();

	void processBlock(AudioSampleBuffer& buffer, MidiBuffer&) override;

    // does not have its own editor, but is controlled by AudioPlayerEditor
	AudioProcessorEditor* createEditor() override { return nullptr; }
	bool hasEditor() const override { return false; }

    const String getName() const override { return "Audio Recorder"; }

	// fixed layout: stereo in, no out
    int getNumInputChannels() const noexcept { return 2; }
	int getNumOutputChannels() const noexcept { return 0; }

    // unused methods
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }
	int getNumPrograms() override { return 1; } // some hosts don't cope well with 0 programs
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const String getProgramName(int) override { return String(); }
	void changeProgramName(int, const String&) override {}

	virtual void fillInPluginDescription(PluginDescription &) const override;
	void getStateInformation(MemoryBlock&) override;
	void setStateInformation(const void*, int) override;

    // custom methods

    /**
        Get the filename currently used.
        @returns the filename, including path
    */
	String *getFilename() { return filename; }
    /** 
        Get the TransportState value.
        @returns the transport state
        @see TransportState
    */
	TransportState getState() { return state; }
    /**
        @returns true if file is opened and recording can start immediately.
    */
    bool isReadyToRecord() { return state >= TransportState::Stopped; }
    /** 
        @returns true, if any input is currently recorded.
    */
    bool isRecording() { return state == TransportState::Recording; }

    void addAudioCallback(AudioIODeviceCallback *);
    void removeAudioCallback(AudioIODeviceCallback *);

protected:
	friend class AudioPlayerPlugin;

	bool changeState(TransportState);
    /**
        Set a new filename for the recording file (not yet opening it).
        New transport state will be NoFile (file named, but not opened).
        @param name the filename, including path
    */
    void setFilename(String* name) { filename = name; changeState(TransportState::NoFile); }
	bool openFileWriter();

private:
	const unsigned WRITE_BUFFER = 32768; // size in bytes for write buffer

	ScopedPointer<AudioFormatWriter::ThreadedWriter> threadedWriter;
	AudioFormatWriter::ThreadedWriter* volatile activeWriter;
	TimeSliceThread writeThread;

	ScopedPointer<String> filename = nullptr;
	AudioFormatManager formatManager;

	CriticalSection stateLock;
	TransportState state;

    Array<AudioIODeviceCallback*> audioCallbacks;
    
    AudioSlaveRecorderPlugin **externalReference = nullptr;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    // unused methods
    virtual void prepareToPlay(double, int) override {}
    virtual void releaseResources() override {}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSlaveRecorderPlugin)
};

#endif  // AUDIOSLAVERECORDERPLUGIN_H_INCLUDED