# p-chan

P-chan PCB KiCad project and Arduino sketch

The footprints of this project had been modified.

SW_SPDT_PCM12 removed mechanical pin 

BOM: 
0805 LED (recommend 40mcd or higher) x 3, 
ATTINY13 or ATTINY85 x 1, 
0.1uF 0805 cap x 1, 
button smd x 1,
slide switch smd x 1.

Arduino sketch: Basic watchdog timer and PCINT3 for button. NOTE: PCINTx triggers ANY change of the pin, use INT0 for rising/falling edge detection.
