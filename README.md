http://hackaday.com/2017/05/31/malduino-elite/

15+3+3+4+2+2+1

interrupt add to event queue
main loop get rid of queue item

tangentbord

TODO: TEST 5V LEVEL TRANSLATION OF 74LV125

oh shit the switches might be upside down

card_present low when card is inserted


need to copy hid.c and 2 functions in ff.c

cache profile filename and key filename during load profile

save last profile number to sd card
each key has its own color?

pin 7 should connect to GND
pin 11 BS1 should be high with a resistor
pin 13 15 to GND
add pullup on I2C lines
reset pulled low with resistor

dont forget to add idwg back after everything's done
delay 100ms then reset when card_present changes
tin the under side of the oled ribbon cable

looks like all the probroems were from inconsistent connection between ribbon cable and pcb, tin the pcb thinly as well as the ribbon cable before soldering, use plenty of flux
line up the holes on the pcb with the holes on the ribbon cable, use a small pcb to hold it in place, like exixe 12