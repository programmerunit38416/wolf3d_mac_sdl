#include <stdint.h>

typedef uint16_t Word;
typedef int16_t SignedWord;
typedef uint32_t LongWord;
typedef int32_t SignedLongWord;

#define TRUE 1
#define FALSE 0

#ifndef __MACTYPES__
typedef unsigned char Byte;
typedef unsigned char Boolean;
#endif

#define BLACK 255
#define DARKGREY 250
#define BROWN 101
#define PURPLE 133
#define BLUE 210
#define DARKGREEN 229
#define ORANGE 23
#define RED 216
#define BEIGE 14
#define YELLOW 5
#define GREEN 225
#define LIGHTBLUE 150
#define LILAC 48
#define PERIWINKLE 120
#define LIGHTGREY 43
#define WHITE 0

#define __MAC__
//#define __BIGENDIAN__
#define SfxActive 1
#define MusicActive 2

#define VideoSize 64000
#define SetAuxType(x,y)
#define SetFileType(x,y)

extern unsigned char *VideoPointer;
extern Word KeyModifiers;
extern Word ScanCode;
extern Word KilledSong;
extern Word SystemState;
extern Word VideoWidth;
extern LongWord LastTick;
extern LongWord YTable[480];
void DLZSS(Byte *Dest, Byte *Src,LongWord Length);
void DLZB(Byte *Dest, Byte *Src,LongWord Length);
LongWord SwapLong(LongWord Val);
LongWord BigToLittleEndian(LongWord Val);  /* Convert from big endian to system native endian */
unsigned short BigToLittleEndianShort(unsigned short Val);
unsigned short SwapUShort(unsigned short Val);
short SwapShort(short Val);

void WaitTicks(Word TickCount);
Word WaitTicksEvent(Word TickCount);
Word WaitEvent(void);
LongWord ReadTick(void);
void *AllocSomeMem(LongWord Size);
void FreeSomeMem(void *MemPtr);
void DisposePtr(void *ptr);

Word GetAKey(void);
Word WaitKey(void);
void FlushKeys(void);

void SoundOff(void);
void PlaySound(Word SndNum);
void StopSound(Word SndNum);
void PlaySong(Word SongNum);

void ClearTheScreen(Word Color);
void ShowPic(Word PicNum);

void InitYTable(void);
void InstallAFont(Word FontNum);
void FontUseMask(void);
void FontUseZero(void);
void SetFontXY(Word x,Word y);
void FontSetColor(Word Index,Word Color);
void DrawAString(char *TextPtr);
void DrawAChar(Word Letter);
void ultoa(LongWord Val,char *TextPtr);
Word GetRandom(Word Range);
void Randomize(void);
void DrawShape(Word x,Word y,void *ShapePtr);
void DrawXMShape(Word x,Word y,void *ShapePtr);
void DrawMShape(Word x,Word y,void *ShapePtr);
void EraseMBShape(Word x,Word y, void *ShapePtr,void *BackPtr);
Word TestMShape(Word x,Word y,void *ShapePtr);
Word TestMBShape(Word x,Word y,void *ShapePtr,void *BackPtr);

void SetAPalette(Word PalNum);
void SetAPalettePtr(unsigned char *PalPtr);
void FadeTo(Word PalNum);
void FadeToBlack(void);
void FadeToPtr(unsigned char *PalPtr);


Byte* LoadAndUnpackPic(Word id);
void *LoadAResource(Word RezNum);
void ReleaseAResource(Word RezNum);
void KillAResource(Word RezNum);
void *LoadAResource2(Word RezNum);
void ReleaseAResource2(Word RezNum);
void KillAResource2(Word RezNum);
void SaveJunk(void *AckPtr,Word Length);

Byte* LoadAndDecompressResource(Word RezNum);
