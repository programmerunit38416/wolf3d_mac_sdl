
### Wolfenstein 3D For Mac

This is a fix for the original 1994 source code to compile and run on modern systems
using SDL.
https://github.com/Blzut3/Wolf3D-Mac

There are some issues remaining:
- missing sound
- missing difficulty selection (it used Mac windows for this)
- missing iD software splash screen.
- missing Save/Load games
- lots more.

Putting this here in case anyone wants a working version for lower specced systems that uses the BSP renderer.

## Screenshot
![](screenshot.jpg)

## Instructions
Use make to build:
make

# Resources
The executable expects the assets in a folder called 'BRGR' in the same directory.
The files in this directory are named by number starting at 128 and are obtained by extracting them from the original 1994 Mac assets.

Getting the original 1994 Mac assets is difficult.
I used SheepShaver to install the original MACOS 9, from there, install Wolfenstein.
To extract the assets from the installed Wolfenstein, use Super Resedit.

