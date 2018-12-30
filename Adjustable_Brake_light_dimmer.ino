/*
 * Purpose of this project is to dim a pair of silly brightness RED LEDs designed to fit into a P21 housing but lack a sensible tail light dimness setting
 * the circuit uses an ATTINY85 8pin DIP chip and a ASSR-1611 6pin DIP solid state relay and a TO92 78L05, one resistor and one capacitor and one diode to
 * block the brake signals +12V from flowing back through the SSR chips protection diodes into the side light circuits and illuminating the front side lights
 * by mistake. My circuit uses a proper push button sub miniature PCB mount switch but can be modified to use a pair of jumper pins that are shorted to 
 * customise the tail light brightness and save the desired PWM brightness value in EEPROM when the jumper is removed for next cold boot up.
 * 
 * To make the board as compact as possible on the 2.54mm pitch board around 0.5mm needed to be filed off the ASSR-1611 chip and the casting seam filed off 
 * the ATTIN85 chip which then allows the two chips to adjoin each other without wasting a row of holes between them.
 * All the components are available in SMD package variants so the entire board could be shrunk further if a commercial PCB fabricator is used.
 * Other possible space saving optimizations are cut two legs off the switch and glue the switch to the ATTINY85 with the two remaining pins soldered to the
 * pins 2 and 4 of the ATTINY85 and mount the 78L05 on the underside of the board and using the capacitors PCB holes for its GND and +5 out wires and once its 
 * all soldered in place fold the 78L05 back under the ATTINY85 chip between its pins. This allows the +12V track to pin4 of the ASSR chip to move one track
 * in towards the chips and it is possible to mount the 680R resistor on the underside of the PCB directly under the two ICs using the adjacent holes next to 
 * the specific pins ATTINY85 pin 6 and the ASSR pin 1. I plan to encapsulate it in silicone potting compound leaving just the top of the button exposed
 */
/*
                               ######
                               / |  \
                        +------+ |  |
                        |        +C-+
                        |      )------(
                        |    1[|0|  +=|]8
                        |      | |    |   ATTINY85
                   =)-------(=[| |    |]
                    |  ,-.  | #| |    |
                    | ( | ) | [| |    |]-+
                    |  `-'  |  | |    | )+(
                   =)-------(=[|-+--+ |]| |
                        |      )------( | |
                        |      )------( | | 680R
                        +-----[|-+  | |]| |
                               | |  | | | |
                            +-[| |  +-|]| |
                           )+( | |    | )+(
                           | |[|-+   0|]-+
                       D1  | |6)------(1
                           )+(     ASSR-1611
                          + |
                            +
*/
#include "avr/eeprom.h"
#include <avr/power.h>
#define ButtonPin PB3
#define brightnessPin PB1
#define brightup 1
#define debounce 100
byte brightness;
unsigned long timp = 0;
boolean changed = 0;

void setup() {
/*  
 *  Read in the saved preset tail light brightness value
 *  from chips EEPROM address 1 into the static variable brightness
 */
    brightness = eeprom_read_byte ((uint8_t *)1);
/* change PWM counter 0 prescaler from default 3 to 4 to slow down the default PWM 0 frequency by 4 to better suit solid state relay chip when running at 16MHz */
    TCCR0B = 4;
    analogWrite(brightnessPin, brightness); // why wait for loop() all the rest is user interface, power reduction and potential clock fiddling
    pinMode(ButtonPin, INPUT_PULLUP);
/* 
 * adjacent input pin will be tied to ButtonPin on production PCB leaving it pulled to logic 1 so that the cpu is down clocked to reduce EMI and pointless fast processing
 * unless the chip is in a dev boards socket OR in the production board with the button pressed during power up, in that case it stays at 16MHz so default ISP settings 
 * work flawlessly while debugging or upgrading firmware with IC pin clips wired to an ISP.
 */
    pinMode(ButtonPin + 1,INPUT);
    ADCSRA = 0;  // disable ADC
    PRR = 3; // power down ADC and USI peripherals
/*
 * This following section is to allow development using an ISP and not micronucleus firmware programmer in socketed dev board 
 * Or in protype board or final circuit without custom avardude settings if button is pressed during power up.
 */
    if (digitalRead(ButtonPin + 1) == 1) // If pin 2 linked to pin 3 otherwise called PB3 and PB4 are connected together and not pulled down to logic zero using the button
    { // downclock to 0.0625MHz and tweak Counter0 and Counter1 to set desired PWM frequency
      cli(); // Disable interrupts (not sure if any interupts are used by the runtimes but just in case these are included to avoid unforseeable problems)
      CLKPR = 128; //  (1<<CLKPCE) // Prescaler enable
      CLKPR = 8; // Clock division factor 256 (1000) divisor code n is encoded 2 the power of n with max divisor code of 8
      sei(); // Enable interrupts
/* change PWM counter 0 prescaler register from 3 to 1 to increase PWM 0 operating frequency by 16 to better suit sold state relay chip when running at 0.0625MHz */
      TCCR0B = 1; // 1 is optimal 122Hz
      TCCR1 = 65;
    } // Otherwise leave at fuse defined clock speed and default Counter1 and Full CPU speed optimised Counter0 TCCR0B for best PWM frequency.
}

void loop() {
/* 
 * Adjusts and saves brightness level to EEPROM to preserve tail light brightness setting for next cold boot
 */
        if ((digitalRead(ButtonPin) == 0) && (millis() - timp > debounce)) 
        {
            brightness = brightness + brightup;
            changed = 1;
            timp = millis();
            analogWrite(brightnessPin, brightness);
        } 
        else if (digitalRead(ButtonPin) == 1 && changed == 1)
        { // only when button is released AND brightness was changed write eeprom address 1 if there is a new value of brightness to preset new brightness on next boot up
          cli();
          eeprom_update_byte ((uint8_t *)1, brightness);
          sei();
          changed = 0;
        }
        delay(50);
}
