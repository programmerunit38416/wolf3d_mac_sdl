History of Wolfenstein 3D: First & Second Encounter
Programming and Project Lead:  Bill Heineman
Additional programming: Chris DeSalvo
Producer:  Bill Dugan

Wolfenstein 3D for MacOS was released on October 1st, 1994 by Interplay 
Productions under license from id software. The code was based not on the 
PC version but of a Super NES version that was done at id software. This 
version of the game differs greatly from the PC version since it used BSP 
trees to help determine which walls were to be drawn instead of the 
ray-casting method.

The fixed point number system was 8.8 format so that it could fit in a 
65816 register for the SNES and Apple IIgs version of Wolf 3D. (8 bits of 
integer and 8 bits of fraction)

The mac version also has two modes of drawing. In the 68000 version, 68000 
code is generated at runtime to draw the vertical lines very quickly. The 
source file SetupScalers68k.c creates the 68000 code and then issues calls 
to it via a little 68000 assembly glue code. This is how the game got its 
speed. The powerpc version originally had this method as well but the 
performance sucked. So it was changed to a simple assembly loop and the 
game ran fine. This is why there is SetupScalersPPC.c and SetupScalers68k.c.

I took the code from Codewarrion DR/4 which was the development system at 
the time and updated the project to Codewarrior PRO 5 (Which is what I use 
today). I've compiled, tested and ran the game and the code runs fine. The 
sound code is copyright Steve Hales and Jim Nitchals. You cannot use the 
music driver in your own programs unless you get a license from Steve 
Hales. (Jim Nitchals sadly has passed on, may he rest in peace).

Yes, there is a level editor that I wrote. It sucks. I suggest you get 
WolfEdit that is available on the web from WolfAddict software instead. It 
doesn't suck.

Here it is 1/21/2000. Over 5 years since I did the mac version of Wolf 3D. 
Seems like yesterday.  I hope you enjoy looking over this code and making 
little changes for your own pleasure and learning. If someone makes any 
improvements to this code like adding Sprocket support or GL support, 
please send me the new source. I can be found at burger@logicware.com

I want to thank those who helped make this project a reality. John Carmack, 
Jay Wilbur, John Romero, Brian Luzietti and my wife Lorenza.

Bill Heineman
Logicware Inc.
20628 E. Arrow Hwy. #6
burger@logicware.com