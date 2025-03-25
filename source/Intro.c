#include "wolfdef.h"
#include <ctype.h>
#include <stdlib.h>


void ShowSplashImage(Word ResourcePicNum, Word ResourcePalNum)
{
	Byte *ShapePtr;

	ShapePtr = LoadAndDecompressResource(ResourcePicNum);
	if (!ShapePtr) {
		return;
	}

	SetAPalette(ResourcePalNum);
	DrawShape(0,0,ShapePtr);
	FreeSomeMem(ShapePtr);
	ReleaseAResource(ResourcePicNum);
	BlastScreen();
	WaitJoystick();
}

/**********************************

	Main game introduction

**********************************/
void Intro(void)
{
	NewGameWindow(1);	/* Set to 512 mode */
	FadeToBlack();		/* Fade out the video */
	ShowSplashImage(rMacPlayPic, rMacPlayPal);
	//ShowSplashImage(rIdLogoPic, rIdLogoPal);

	return;
}
