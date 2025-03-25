#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wolfdef.h"
#include "Burger.h"
#include <string.h>
#include <setjmp.h>
#include <ctype.h>

// SDL specific globals
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *screenTexture = NULL;
SDL_Surface *screenSurface = NULL;

// Global RGB buffer for conversion
Uint32* rgbBuffer = NULL;

#define WINDOW_WIDTH (512)
#define WINDOW_HEIGHT (384)

Word QuitFlag;
jmp_buf ResetJmp;
Boolean JumpOK;
extern Word NumberIndex;

void HandleEvents(void);

/**********************************

    Initialize SDL and create window

**********************************/

SignedLongWord InitSDL(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Wolf3D Mac",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // Create texture for rendering
    screenTexture = SDL_CreateTexture(renderer,
                                      SDL_PIXELFORMAT_RGB888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      SCREENWIDTH,
                                      SCREENHEIGHT);
    if (screenTexture == NULL)
    {
        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // Initialize video memory
    VideoPointer = (unsigned char *)malloc(SCREENWIDTH * SCREENHEIGHT); // 8-bit indexed color buffer
    if (!VideoPointer)
    {
        printf("Failed to allocate video memory!\n");
        // CleanupSDL();
        return 1;
    }

    // Initialize video width for proper screen handling
    VideoWidth = SCREENWIDTH;

    return 1;
}

/**********************************

    Clean up SDL resources

**********************************/

void CleanupSDL(void)
{
    if (screenTexture)
        SDL_DestroyTexture(screenTexture);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    if (VideoPointer)
        free(VideoPointer);
    SDL_Quit();
}

/**********************************

    Update the screen with the current frame buffer

**********************************/

void BlastScreen(void)
{
    // Ensure RGB buffer exists
    if (!rgbBuffer)
    {
        rgbBuffer = (Uint32 *)malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(Uint32));
        if (!rgbBuffer)
        {
            printf("Failed to allocate RGB buffer!\n");
            return;
        }
    }

    //// Set up a greyscale palette
    //for (int i = 0; i < 256; i++) {
    //    CurrentPal[i * 3] = i;     // Red component
    //    CurrentPal[i * 3 + 1] = i; // Green component 
    //    CurrentPal[i * 3 + 2] = i; // Blue component
    //}


    // Convert indexed colors to RGB using current palette
    for (SignedLongWord i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++)
    {
        Byte index = VideoPointer[i];
        Uint32 color = (Uint32)CurrentPal[index * 3] << 16 | (Uint32)CurrentPal[index * 3 + 1] << 8 | (Uint32)CurrentPal[index * 3 + 2];
        rgbBuffer[i] = color; 
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); 
    SDL_UpdateTexture(screenTexture, NULL, rgbBuffer, SCREENWIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer);
    
    // Set up source and destination rectangles for proper scaling
    SDL_Rect srcRect = { 0, 0, SCREENWIDTH, SCREENHEIGHT };
    SDL_Rect dstRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    
    SDL_RenderCopy(renderer, screenTexture, &srcRect, &dstRect);
    SDL_RenderPresent(renderer);
}

/**********************************

    Handle SDL events and convert to Wolf3D events

**********************************/

void HandleEvents(void)
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

    Prepare the screen for game

**********************************/

void SetupPlayScreen(void)
{
    SetAPalette(rBlackPal); /* Force black palette */
    ClearTheScreen(BLACK);  /* Clear the screen to black */
    BlastScreen();
    firstframe = 1; /* fade in after drawing first frame */
    StartupRendering(GameViewSize);
}

/**********************************

    Display the automap

**********************************/

void RunAutoMap(void)
{
	Word vx,vy;
	Word Width,Height;
	Word CenterX,CenterY;
	Word oldjoy,newjoy;
	
	MakeSmallFont();				/* Make the tiny font */
	playstate = EX_AUTOMAP;
	vx = viewx>>8;					/* Get my center x,y */
	vy = viewy>>8;
	Width = (SCREENWIDTH/16);		/* Width of map in tiles */
	Height = (VIEWHEIGHT/16);		/* Height of map in tiles */
	CenterX = Width/2;
	CenterY = Height/2;
	if (vx>=CenterX) {
		vx -= CenterX;
	} else {
		vx = 0;
	}
	if (vy>=CenterY) {
		vy -= CenterY;
	} else {
		vy = 0;
	}
	oldjoy = joystick1;
	do {
		ClearTheScreen(BLACK);
		DrawAutomap(vx,vy);
		do {
            HandleEvents();
			ReadSystemJoystick();
            SDL_Delay(16);
		} while (joystick1==oldjoy);
		oldjoy &= joystick1;
		newjoy = joystick1 ^ oldjoy;
		if (newjoy & (JOYPAD_START|JOYPAD_SELECT|JOYPAD_A|JOYPAD_B|JOYPAD_X|JOYPAD_Y)) {
			playstate = EX_STILLPLAYING;
		} 
		if (newjoy & JOYPAD_UP && vy) {
			--vy;
		}
		if (newjoy & JOYPAD_LFT && vx) {
			--vx;
		}
		if (newjoy & JOYPAD_RGT && vx<(MAPSIZE-1)) {
			++vx;
		}
		if (newjoy & JOYPAD_DN && vy <(MAPSIZE-1)) {
			++vy;
		}
	} while (playstate==EX_AUTOMAP);

	playstate = EX_STILLPLAYING;
/* let the player scroll around until the start button is pressed again */
	KillSmallFont();			/* Release the tiny font */
	RedrawStatusBar();
	ReadSystemJoystick();
	mousex = 0;
	mousey = 0;
	mouseturn = 0;
}

/**********************************

    Begin a new game

**********************************/

void StartGame(void)
{
    if (playstate != EX_LOADGAME)
    {              /* Variables already preset */
        NewGame(); /* init basic game stuff */
    }
    SetupPlayScreen();
    GameLoop(); /* Play the game */
    StopSong(); /* Make SURE music is off */
}

/**********************************

    Show the game logo

**********************************/

Boolean TitleScreen(void)
{
    Byte *ShapePtr;

    playstate = EX_LIMBO; /* Game is not in progress */
    NewGameWindow(1);     /* Set to 512 mode */
    FadeToBlack();        /* Fade out the video */
    ShapePtr = LoadAndUnpackPic(rTitlePic);
    DrawShape(0, 0, ShapePtr);
    ReleaseAResource(rTitlePic);
    FreeSomeMem(ShapePtr);
    BlastScreen();
    StartSong(SongListPtr[0]);
    FadeTo(rTitlePal); /* Fade in the picture */
    BlastScreen();

    // Wait for any key or quit
    WaitKey();

    playstate = EX_COMPLETED;
    return TRUE; /* Return True if canceled */
}

SignedLongWord main(SignedLongWord argc, char *argv[])
{

    for (SignedLongWord i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-auto") == 0)
        {
        }
    }

    // Initialize SDL
    if (!InitSDL())
    {
        return 1;
    }

    InitTools();   /* Init the system environment */

    // Initialize game resources
    VideoPointer = (unsigned char *)malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(Uint32));
    if (!VideoPointer)
    {
        printf("Failed to allocate video memory!\n");
        CleanupSDL();
        return 1;
    }

    // Initialize video width for proper screen handling
    VideoWidth = SCREENWIDTH;

    // Initialize game
    playstate = (exit_t)setjmp(ResetJmp);
    NumberIndex = 36; /* Force the score to redraw properly */
    IntermissionHack = FALSE;

    if (playstate)
    {
        goto DoGame; /* Begin a new game or saved game */
    }

    JumpOK = TRUE; /* Jump vector is VALID */
    FlushKeys();   /* Allow a system event */
    Intro();       /* Do the game intro */

    // Main game loop
    QuitFlag = 0;
    while (!QuitFlag)
    {
        if (TitleScreen())
        { /* Show the game logo */
            StartSong(SongListPtr[0]);
            ClearTheScreen(BLACK); /* Blank out the title page */
            BlastScreen();
            SetAPalette(rBlackPal);
            if (ChooseGameDiff())
            {                           /* Choose your difficulty */
                playstate = EX_NEWGAME; /* Start a new game */
            DoGame:
                FadeToBlack(); /* Fade the screen */
                StartGame();   /* Play the game */
            }
        }
        HandleEvents();
        SDL_Delay(16); // Cap to ~60 FPS
    }

    // Cleanup
    CleanupSDL();
    return 0;
}