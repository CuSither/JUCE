#ifndef TRANSPORTSTATE_H_INCLUDED
#define TRANSPORTSTATE_H_INCLUDED

/**
    Transport state enum for Audio players, recorders and similar
*/
enum TransportState : int
{
    // order matters, operators like > are used!
    NoFile,
    Unloading,
    Stopped,
    Stopping,
    Paused,
    Pausing,
    Starting,
    Playing,
    Recording
};
#endif  // TRANSPORTSTATE_H_INCLUDED
