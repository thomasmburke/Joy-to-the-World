# Joy-to-the-World
MSP430 micro controller plays Joy to the World song
Functionality should include a speed up/down button, pause/play, change song, and restart song button.

# Implementation Steps
First I initialize my buttons and speaker to P1 ports that way I can allow for interrupts to have the microcontroller react even when the CPU is off. In order to generate the notes, I used the half period equation and multiplied .5*(1/f)*1000000. The notes and note lengths are put into arrays in which I will index the notes and note lengths in my watchdog interrupt. I set up a couple global variables to index the array in the watchdog and also a tempo floating point variable to change the speed of the song. I simply multiply the tempo to the duration of a note and the extends or shortens the note length for my speed up slow down buttons. The sound is produced by TimerA in up mode with a SMCLK source. To implement the reset button all that had to be done was to set my global variables pertaining to the watchdog to zero except for tempo and also to turn the speaker off. I set up 3 states in my design A,B, and C. A=Joy to the World, B=fakesong, C=play nothing. Switch song button literally just changes the states and resets necessary globals.

# Schematic
This was just more of a drawing exploration that took a weird turn
