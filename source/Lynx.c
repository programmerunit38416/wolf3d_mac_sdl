/*********************

    SDL Platform-specific code for Wolf3D

*********************/

#include "wolfdef.h"
#include <SDL2/SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables
Boolean MouseHit = FALSE;   // True if mouse was pressed
Word MacWidth = 512;        // Default width
Word MacHeight = 384;       // Default height
Word MacViewHeight = 160;   // Default view height
Word TrueVideoWidth;        // Width of video in bytes
Byte *TrueVideoPointer;     // Pointer to video memory
Byte *GameVideoPointer;     // Pointer to start of 3D view
Boolean DoQuickDraw = TRUE; // Use direct rendering
Word IgnoreMessage = 0;     // Ignore system messages

// SDL specific globals (from Main.c)
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *screenTexture;

// Screen size options
Word VidXs[] = {320, 512, 640, 640};                          // Screen widths
Word VidYs[] = {200, 384, 400, 480};                          // Screen heights
Word VidVs[] = {160, 320, 320, 400};                          // View heights
Word VidPics[] = {rFaceShapes, rFace512, rFace640, rFace640}; /* Resource #'s for art */

// Psyched bar dimensions
#define PSYCHEDWIDE 184
#define PSYCHEDHIGH 5
#define PSYCHEDX 20
#define PSYCHEDY 46
#define MAXINDEX (66 + S_LASTONE)

// Static variables for psyched bar
static Rect PsychedRect;
static Word LeftMargin;

LongWord ScaleDiv[2048];

// Add these globals after the existing ones
Byte *SmallFontPtr; // Font pointer for automap

/**********************************

    Create the compiled scalers

**********************************/

Boolean SetupScalers(void)
{
    Word i;

    if (!ScaleDiv[1])
    { /* Divide table inited already? */
        i = 1;
        do
        {
            ScaleDiv[i] = 0x400000UL / (LongWord)i; /* Get the recipocal for the scaler */
        } while (++i < 2048);
    }
    MaxScaler = 2048;
    return TRUE;
}

void ReleaseScalers(void)
{
}

/**********************************

    Scale the system X coord

**********************************/

Word ScaleX(Word x)
{
    switch (MacVidSize)
    {
    case 1:
        return x * 8 / 5;
    case 2:
    case 3:
        return x * 2;
    }
    return x;
}

/**********************************

    Scale the system Y coord

**********************************/

Word ScaleY(Word y)
{
    switch (MacVidSize)
    {
    case 1: /* 512 resolution */
        y = (y * 8 / 5) + 64;
        if (y == 217)
        { /* This hack will line up the gun on 512 mode */
            ++y;
        }
        return y;
    case 2: /* 640 x 400 */
        return y * 2;
    case 3: /* 640 x 480 */
        return y * 2 + 80;
    }
    return y; /* 320 resolution */
}

/**********************************

    Load the map list into MapListPtr
    Data is BIG endian, so we need to convert it to little endian

    TODO: Convert to little endian and dont use Words (use bytes)

**********************************/
void LoadMapList()
{
    // Load the map list from resource
    MapListPtr = (maplist_t *)LoadAResource(rMapList);
    if (!MapListPtr)
    {
        return;
    }

    // Convert MaxMap and MapRezNum from big endian to little endian
    MapListPtr->MaxMap = (MapListPtr->MaxMap >> 8) | (MapListPtr->MaxMap << 8);
    MapListPtr->MapRezNum = (MapListPtr->MapRezNum >> 8) | (MapListPtr->MapRezNum << 8);

    // Convert each MapInfo_t entry in the InfoArray
    MapInfo_t *info = MapListPtr->InfoArray;
    for (Word i = 0; i < MapListPtr->MaxMap; i++)
    {
        info[i].NextLevel = (info[i].NextLevel >> 8) | (info[i].NextLevel << 8);
        info[i].SecretLevel = (info[i].SecretLevel >> 8) | (info[i].SecretLevel << 8);
        info[i].ParTime = (info[i].ParTime >> 8) | (info[i].ParTime << 8);
        info[i].ScenarioNum = (info[i].ScenarioNum >> 8) | (info[i].ScenarioNum << 8);
        info[i].FloorNum = (info[i].FloorNum >> 8) | (info[i].FloorNum << 8);
    }
}

/**********************************

    Initialize the system

**********************************/

void InitTools(void)
{
    // Most initialization is done in InitSDL() in Main.c
    TrueVideoWidth = SCREENWIDTH;
    TrueVideoPointer = VideoPointer;
    GameVideoPointer = VideoPointer;

    // short int *SoundListPtr; /* Pointer to sound list for Halestorm driver */
    // SoundListPtr = (short int *)LoadAResource(MySoundList);
    ////RegisterSounds(SoundListPtr, FALSE);
    // ReleaseAResource(MySoundList);
    GetTableMemory(); /* Get memory for far tables math tables */
    // MapListPtr = (maplist_t *)LoadAResource(rMapList); /* Get the map list */
    LoadMapList();
    SongListPtr = (unsigned short *)LoadAResource(rSongList);
    WallListPtr = (unsigned short *)LoadAResource(MyWallList);

    NewGameWindow(1);      /* Create a game window at 512x384 */
    ClearTheScreen(BLACK); /* Force the offscreen memory blank */
    BlastScreen();
    InitYTable();

    MacVidSize = 1;
    Byte *DestPtr;
    LongWord *LongPtr = (LongWord *)LoadAResource(VidPics[MacVidSize]);
    Word i, j;
    if (!LongPtr)
    {
        goto OhShit;
    }
    LongWord size = BigToLittleEndian(LongPtr[0]);

    Byte *GameShapes2 = (Byte *)AllocSomeMem(size); /* All the permanent game shapes */
    if (!GameShapes2)
    {                                          /* Can't load in the shapes */
        ReleaseAResource(VidPics[MacVidSize]); /* Release it NOW! */
        goto OhShit;
    }
    DLZSS((Byte *)GameShapes2, (Byte *)&LongPtr[1], size);
    ReleaseAResource(VidPics[MacVidSize]);
    i = 0;
    j = (MacVidSize == 1) ? 47 + 10 : 47; /* 512 mode has 10 shapes more */

    DestPtr = (Byte *)GameShapes2;
    LongPtr = (LongWord *)GameShapes2;
    do
    {
        GameShapes[i] = DestPtr + BigToLittleEndian(LongPtr[i]);
    } while (++i < j);

OhShit:
    return;

    return;
}

/**********************************

    Update a portion of the screen

**********************************/

void BlastScreen2(Rect *BlastRect)
{
    (void)BlastRect;
    // PWW:
    // TODO: implmenent rect region blitting
    BlastScreen();
}

/**********************************

    Handle system events

**********************************/

void HandleEvents2(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            QuitFlag = 1;
            break;
        case SDL_KEYDOWN:
            // TODO: Map SDL keys to Wolf3D keys
            break;
        case SDL_KEYUP:
            // TODO: Map SDL keys to Wolf3D keys
            break;
        }
    }
}

/**********************************

    Emergency exit

**********************************/

void BailOut(void)
{
    SDL_Quit();
    exit(1);
}

/**********************************

    Clean exit

**********************************/

void GoodBye(void)
{
    SDL_Quit();
    exit(0);
}

/**********************************

    Read joystick/keyboard input

**********************************/

void ReadSystemJoystick(void)
{
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    // Map SDL keys to Wolf3D controls
    // This is a basic mapping, adjust as needed
    joystick1 = 0;

    if (keystate[SDL_SCANCODE_UP])
        joystick1 |= JOYPAD_UP;
    if (keystate[SDL_SCANCODE_DOWN])
        joystick1 |= JOYPAD_DN;
    if (keystate[SDL_SCANCODE_LEFT])
        joystick1 |= JOYPAD_LFT;
    if (keystate[SDL_SCANCODE_RIGHT])
        joystick1 |= JOYPAD_RGT;
    if (keystate[SDL_SCANCODE_SPACE])
        joystick1 |= JOYPAD_A;
    if (keystate[SDL_SCANCODE_LCTRL])
        joystick1 |= JOYPAD_B;
    if (keystate[SDL_SCANCODE_RETURN])
        joystick1 |= JOYPAD_START;
    if (keystate[SDL_SCANCODE_TAB])
        joystick1 |= JOYPAD_SELECT;
}

/**********************************

    Get the current joystick state

**********************************/

Word GetJoystick(void)
{
    ReadSystemJoystick();
    return joystick1;
}

/**********************************

WaitJoystick.

**********************************/

Word WaitJoystick(void)
{
    Word JoyState;
    do
    {
        HandleEvents2();
        JoyState = GetJoystick();
    } while (JoyState == 0);
    
    return JoyState;
}

/**********************************

    Change video mode

**********************************/

Word NewGameWindow(Word NewVidSize)
{
    if (NewVidSize >= 4)
    {
        NewVidSize = 0;
    }

    MacWidth = VidXs[NewVidSize];
    MacHeight = VidYs[NewVidSize];
    MacViewHeight = VidVs[NewVidSize];

    SetAPalette(rBlackPal);
    ClearTheScreen(BLACK);
    BlastScreen();

    return NewVidSize;
}

/**********************************

    Show "Get Psyched" screen

**********************************/

void ShowGetPsyched(void)
{
    Byte *ShapePtr;
    Word X, Y;

    ClearTheScreen(BLACK);
    BlastScreen();
    ShapePtr = LoadAndUnpackPic(rGetPsychPic);
    X = (MacWidth - 224) / 2;
    Y = (MacViewHeight - 56) / 2;
    DrawShape(X, Y, ShapePtr);
    FreeSomeMem(ShapePtr);
    ReleaseAResource(rGetPsychPic);
    BlastScreen();
    SetAPalette(rGamePal);

    // Initialize the psyched bar rectangle
    PsychedRect.top = Y + PSYCHEDY;
    PsychedRect.bottom = PsychedRect.top + PSYCHEDHIGH;
    PsychedRect.left = X + PSYCHEDX;
    PsychedRect.right = PsychedRect.left;
    LeftMargin = PsychedRect.left;

    BlastScreen();
    //WaitKey();
}

void EndGetPsyched(void)
{
    // ForeColor(blackColor);        /* Reset the color to black for high speed */
    SetAPalette(rBlackPal); /* Zap the palette */
}

/**********************************

    Draw psyched animation frame


**********************************/

void DrawPsyched(Word Index)
{
    Word Factor;
    Word x, y, width, height;

    // Calculate the relative width of the progress bar
    Factor = (Index * PSYCHEDWIDE) >> 8; // Approximate division by using shift

    // Update rectangle dimensions
    PsychedRect.left = PsychedRect.right;
    PsychedRect.right = Factor + LeftMargin;

    // Calculate dimensions
    x = PsychedRect.left;
    y = PsychedRect.top;
    width = PsychedRect.right - PsychedRect.left;
    height = PsychedRect.bottom - PsychedRect.top;

    // Draw the red bar directly to VideoPointer
    for (Word dy = 0; dy < height; dy++) {
        for (Word dx = 0; dx < width; dx++) {
            VideoPointer[YTable[y + dy] + (x + dx)] = 15; 
        }
    }

    BlastScreen();
}

/**********************************

    Choose game difficulty

**********************************/

Word ChooseGameDiff(void)
{
    // TODO: Implement proper difficulty selection UI
    // For now, just return medium difficulty
    return 1;
}

/**********************************

    Show shareware message

**********************************/

void ShareWareEnd(void)
{
    // TODO: Implement shareware end screen
    // For now, just clear the screen
    memset(VideoPointer, 0, SCREENWIDTH * SCREENHEIGHT * sizeof(Uint32));
    BlastScreen();
}

/**********************************

    Finish loading a game

**********************************/

void FinishLoadGame(void)
{
    // TODO: Implement game loading
    // For now, just initialize a new game
    NewGame();
}

/**********************************

    Draw a vertical line with a scaler
    (Used for WALL drawing)

**********************************/

void IO_ScaleWallColumn(Word x, Word scale, Word tile, Word column)
{
    if (scale == 0) 
    {
        return;  
    }

    scale *= 2;  
    LongWord TheFrac = ScaleDiv[scale] << 1;  // Shift to 16.16 

    Byte *ArtStart = ArtData[tile] + (column * 128);  // Match tile/column addressing
    Byte *ScreenPtr = VideoPointer + x;  // Base screen pointer with x offset
    
    if (scale < MacViewHeight) 
    {
        // No clipping case
        Word y = (MacViewHeight - scale) / 2;  // Center vertically
        ScreenPtr += y * VideoWidth;  // Adjust screen pointer for y offset
        Word Integer = TheFrac >> 16;  // Match integer isolation
        LongWord Frac = (TheFrac & 0xFFFF) << 15;  // Match fraction isolation
        LongWord Delta = 0;
        
        while (scale--)  
        {
            *ScreenPtr = *ArtStart;  // Direct byte copy
            Delta += Frac;  // Fractional accumulator
            ArtStart += Integer + (Delta >> 31);  // Integer step + carry
            Delta &= 0x7FFFFFFF;  // Clear carry
            ScreenPtr += VideoWidth;  // Next line
        }
    }
    else
    {
        // Clipping case
        Word lostLines = (scale - MacViewHeight) / 2;  // Match vertical centering
        Word Integer = TheFrac >> 16;
        LongWord temp = (LongWord)lostLines * TheFrac;  // Match lost lines calc
        Byte *startOffset = ArtStart + (temp >> 16);  // Match source pointer adjustment
        LongWord Delta = temp << 15;  // Match delta initialization
        LongWord Frac = (LongWord)(TheFrac << 15);
        
        Word lines = MacViewHeight;  // Match screen-height limited loop
        while (lines--)
        {
            *ScreenPtr = *startOffset;
            Delta += Frac;
            startOffset += Integer + (Delta >> 31);
            Delta &= 0x7FFFFFFF;
            ScreenPtr += VideoWidth;
        }
    }
}

void IO_ScaleMaskedColumn(Word x, Word scale, unsigned short *CharPtr, Word column)
{
    Byte *CharPtr2;
    SignedLongWord Y1, Y2;
    Byte *Screenad;
    SpriteRun *RunPtr;
    LongWord TheFrac;
    Word RunCount;
    SignedLongWord TopY;
    Word Index;
    LongWord Delta;

    if (!scale)
    {
        return;
    }

    CharPtr2 = (Byte *)CharPtr;
    TheFrac = ScaleDiv[scale];                               // Get the scale fraction
    RunPtr = (SpriteRun *)&CharPtr[CharPtr[column + 1] / 2]; // Get the offset to the RunPtr data
    Screenad = &VideoPointer[x];                             // Set the base screen address
    TopY = (VIEWHEIGHT / 2) - scale;                         // Number of pixels for 128 pixel shape

    while (RunPtr->Topy != (unsigned short)-1)
    { // Not end of record?
        Y1 = (LongWord)scale * RunPtr->Topy / 128 + TopY;
        if (Y1 < (SignedLongWord)VIEWHEIGHT)
        { // Clip top?
            Y2 = (LongWord)scale * RunPtr->Boty / 128 + TopY;
            if (Y2 > 0)
            {
                if (Y2 > (SignedLongWord)VIEWHEIGHT)
                {
                    Y2 = VIEWHEIGHT;
                }
                Index = RunPtr->Shape + (RunPtr->Topy / 2);
                Delta = 0;
                if (Y1 < 0)
                {
                    Delta = (LongWord)(0 - Y1) * TheFrac;
                    Index += (Delta >> 16);
                    Y1 = 0;
                }
                RunCount = Y2 - Y1; // How many lines to draw?
                if (RunCount)
                {
                    // Draw the scaled sprite segment
                    Byte *SrcPtr = &CharPtr2[Index];
                    Byte *DestPtr = &Screenad[YTable[Y1]];
                    Word LineHeight = RunCount;
                    LongWord FracStep = TheFrac;
                    LongWord CurFrac = Delta;

                    while (LineHeight--)
                    {
                        Byte pixel = SrcPtr[CurFrac >> 16];
                        if (pixel)
                        { // Only draw non-zero pixels (masked)
                            *DestPtr = pixel;
                        }
                        CurFrac += FracStep;
                        DestPtr += VideoWidth;
                    }
                }
            }
        }
        RunPtr++; // Next record
    }
}

/**********************************

    Draw an automap tile

**********************************/

void DrawSmall(Word x, Word y, Word tile)
{
    Byte *Screenad;
    Byte *ArtStart;
    Word Width, Height;

    if (!SmallFontPtr)
    {
        return;
    }

    x *= 16;
    y *= 16;
    Screenad = &VideoPointer[YTable[y] + x];
    ArtStart = &SmallFontPtr[tile * (16 * 16)];
    Height = 0;
    do
    {
        Width = 16;
        do
        {
            Screenad[0] = ArtStart[0];
            ++Screenad;
            ++ArtStart;
        } while (--Width);
        Screenad += VideoWidth - 16;
    } while (++Height < 16);
}

/**********************************

    Create the small font for automap

**********************************/

void MakeSmallFont(void)
{
    Word i, j, Width, Height;
    Byte *DestPtr, *ArtStart;
    Byte *TempPtr;

    SmallFontPtr = AllocSomeMem(16 * 16 * 65);
    if (!SmallFontPtr)
    {
        return;
    }

    memset(SmallFontPtr, 0, 16 * 16 * 65); // Erase the font
    i = 0;
    DestPtr = SmallFontPtr;
    do
    {
        ArtStart = &ArtData[i][0];
        if (!ArtStart)
        {
            DestPtr += (16 * 16);
        }
        else
        {
            Height = 0;
            do
            {
                Width = 16;
                j = Height * 8;
                do
                {
                    DestPtr[0] = ArtStart[j];
                    ++DestPtr;
                    j += (WALLHEIGHT * 8);
                } while (--Width);
            } while (++Height < 16);
        }
    } while (++i < 64);

    TempPtr = LoadAResource(MyBJFace);
    memcpy(DestPtr, TempPtr, 16 * 16);
    ReleaseAResource(MyBJFace);
}

/**********************************

    Release the small font memory

**********************************/

void KillSmallFont(void)
{
    if (SmallFontPtr)
    {
        FreeSomeMem(SmallFontPtr);
        SmallFontPtr = 0;
    }
}