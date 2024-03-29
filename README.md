# ROMulator
An ATmega328 (Arduino) based tool for reading and writing to SRAM


Often, during bring-up of a new design, we find ourselves needing to initialise a RAM with a data file.

Back in 2015, I needed to load a 32K RAM with code for a Z80 SBC, I was working on.

I came up with a simple circuit that can be quite easily be breadboarded or put on a low cost PCB.

It is based on the ATmega328 "Arduino" but these days it could be any small MCU. I only chose the '328 because it was cheap, readily available and has 5V I/O.

So lets assume we have a 32K RAM we need to access.

We can use a pair of 74HC595 8-bit shift registers to generate a 15 or 16-bit address.  These can be tri-stated, so after loading the RAM with data, they won't interfere with normal cpu access to the RAM.

To access the RAM data bus, I used a bidirectional buffer 74HC245. This was connected to 8 consecutive port pins on the Arduino. It can also be tri-stated so it won't get in the way of regular RAM access cycles.

We also need a few control lines, to hold the target CPU in RESET, and access the /CE, /WE and /OE lines of the SRAM.

Back in 2015, I created an Arduino sketch to mechanise these operations.

I am revisiting it and cleaning it up so it can be general purpose.

I have created a Github repo "ROMulator" and put the 2015 Arduino sketch in there.
