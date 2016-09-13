/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 4.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_1A069B68023ACEEC__
#define __JUCE_HEADER_1A069B68023ACEEC__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "AudioPlayerPlugin.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    The Editor for the AudioPlayerPlugin which is used as InternalFilter for the Plugin Host.
    UI is created with the Projucer - only change code in user areas!!!
                                                                    //[/Comments]
*/
class AudioPlayerEditor  : public AudioProcessorEditor,
                           private Timer,
                           public ButtonListener,
                           public SliderListener
{
public:
    //==============================================================================
    AudioPlayerEditor (AudioPlayerPlugin &processor);
    ~AudioPlayerEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void updateLoopState(bool);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;

    // Binary resources:
    static const char* back_png;
    static const int back_pngSize;
    static const char* pause_png;
    static const int pause_pngSize;
    static const char* play_png;
    static const int play_pngSize;
    static const char* stop_png;
    static const int stop_pngSize;
    static const char* folder_png;
    static const int folder_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	const unsigned UI_TIMER_MS = 100; // milliseconds to update UI

    int64 previousPos = -1;
    AudioFormatManager formatManager;
	AudioPlayerPlugin& processor;
	void togglePlayPause(bool);
	void updateTime();
	void timerCallback() override;
	TransportState getTransportState() { return processor.getTransportState(); }
	void changeState(TransportState state) { processor.changeState(state); }
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<ImageButton> back;
    ScopedPointer<ImageButton> playPause;
    ScopedPointer<ImageButton> stop;
    ScopedPointer<ImageButton> openFile;
    ScopedPointer<Slider> timeSlider;
    ScopedPointer<ToggleButton> repeat;
    ScopedPointer<Label> timeLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayerEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_1A069B68023ACEEC__
