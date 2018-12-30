# Adjustable_Brake_light_dimmer
An adjustable PWM dimmer to provide stop and tail brightness functionality for ultrabright Red LED lamps

This project grew from a Chinese made P21/5 LED replacement that looked like it was all that is needed to replace an incandescent 1157 socketed stop and tail bulb but on delivery they simply bridged the two filaments terminals together so the LED would be max brightness regardless of stop or tail terminal was energised.

I've been playing with Attmel ATTINY85 processors and chinese clones of the digistump development boards and solid state relay chips and could envision a very simple project that would combine the two to plus a very few passives and a 78L05 to make a PWM dimmer to dim the LED when the tail light is required and allow full brightness when the stop light is required.

I have prototyped this on a breadboard and added the ability to preset the brightness the tail light defaults to with a push button and store the chosen value in the ATTINY85 built in EEPROM.

I also play with the runtime clock speed dynamically in the hopes of reducing EMI.

I chose using a Solid State Relay chip over that of a discrete switching MOS-FET because of component count rather than component cost... for me the hassle of driving a P channel MOS-FET was more tiresome than the cost of buying a 6-pin DIL package SSR.

Currently there is no good schematic diagram or PCB layout just a very bad ASCII drawing in the INO file.

The current project can be loaded via digistump micronucleus for development purposes but once the major dev work is completed and one needs a near instant ON for the LED lights it can be programmed using a suitable In Circuit Programmer board.

My current PCB is made from a double sided project board and the essential dimensions will be once I cut the board down to size 13mm x 29mm and I use a triple component layer technique where some components are on the underside of the board, some are on the top and the switch is mounted directly on top of the ATTINY85
