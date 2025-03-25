#ifndef SOUNDMUSICSYSTEM_H
#define SOUNDMUSICSYSTEM_H

// Sound function prototypes 
#define BeginSound(theID, theRate)
#define EndSound(theID)
#define EndAllSound()
#define PurgeAllSounds(minMemory)

// Music command definitions
#define kBeginSongLooped    1
#define kEndSong            2
#define kFreeSong           16

// Music macros
#define BeginSongLooped(theSong)  
#define EndSong()                  
#define FreeSong()                 

#endif // SOUNDMUSICSYSTEM_H
