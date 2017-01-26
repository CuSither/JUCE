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

private:
    AudioPlayerPlugin* player = nullptr;
    AudioSlaveRecorderPlugin* recorder = nullptr;

    /**
        Notifies the player if there is any news about a recorder.
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
