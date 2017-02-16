/*
==============================================================================

This file is part of JUCE examples from https://github.com/harry-g.
Copyright (c) 2017 Harry G.

Permission is granted to use this software under the terms of the GPL v2 (or any later version)

Details of these licenses can be found at: www.gnu.org/licenses

JUCE examples are distributed in the hope that they will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

==============================================================================
*/
#ifndef PLUGINSHAREDDATA_H_INCLUDED
#define PLUGINSHAREDDATA_H_INCLUDED

#include "AudioPlayerPlugin.h"
#include "AudioSlaveRecorderPlugin.h"

/**
    A class to exchange data between the two plugins AudioPlayerPlugin and 
    AudioSlaveRecorderPlugin via a SharedResourcePointer.
    Used to let the player always know if it has a recorder or not.
    
    Notify every player&reorder creation and deletion here!
*/
class AudioPlayerPluginSharedData {
    
//public:
//    AudioPlayerPluginSharedData() {};

protected:
    friend class AudioPlayerPlugin;
    friend class AudioSlaveRecorderPlugin;

    /**
        Call this if the recorder is created.
    */
    void setRecorder(AudioSlaveRecorderPlugin* rec) {
        recorder = rec;
        notifyPlayer();
    }

    /**
        Call this if the recorder is deleted.
    */
    void recorderKilled() {
        setRecorder(nullptr);
    }

    /**
        Check if a recorder is registered
        @returns true if there is a recorder
    */
    bool hasRecorder() {
        return recorder != nullptr;
    }

    /**
        Call this if the player is created.
    */
    void setPlayer(AudioPlayerPlugin* pl) {
        // new player, but possibly existing recorder?
        if (pl != nullptr) {
            player = pl;
            // notify, if recorder exists
            notifyPlayer();
        } else
            player = nullptr;
    }

    /** 
        Call this if the player is deleted.
    */
    void playerKilled() {
        setPlayer(nullptr);
    }

    /**
        Check if a player is registered
        @returns true if there is a player
    */
    bool hasPlayer() {
        return player != nullptr;
    }

private:
    AudioPlayerPlugin* player = nullptr;
    AudioSlaveRecorderPlugin* recorder = nullptr;

    /**
        Notifies the player once there is any news about a recorder.
    */
    void notifyPlayer() {
        if (player != nullptr) {
            if (recorder != nullptr)
                player->setRecorder(recorder);
            else
                player->recorderKilled();
        }
    }
};

#endif  // PLUGINSHAREDDATA_H_INCLUDED
