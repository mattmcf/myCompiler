From Sean Smith (Dartmouth College) -- instructions on downloading the simulator

To install:

Download it and put it somewhere
Uncompress it:

`tar -xvzf sim2.taz`

cd down into sim2 and edit the beginning of the Makefile, as noted.
SIMPATH should be absolute pathname of where this directory lives
BINPATH should be the path to where you would like your executable binaries to go

`make`

to build.
then

`make install`

to copy the binaries to your bin dir, if that's how you like to do things. (That's what I like to do, so that I can reach the tools from the command line no matter what dir I'm in). You might want to tell your shell that there are new things in bin; on csh/tcsh, you can do this with "rehash"; on bash, with "hash -r"
You get:

yas, the assembler
yis, an instruction-level simulator
ssim, a simulator for the sequential Y86 datapath "SEQ" from the textbook
plus lots of sample assembler code down in sim2y86-code/