/************
P-chan pin test ATTINY13 code based on https://www.electronics-lab.com/project/attiny85-push-button-power-switching-software-solution/
Watchdog timer and other delays depend on FOsC, using default 9.6MHz

*************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#define BTN   PB3
#define LED1  PB4
#define LED2  PB1
#define LED3  PB0
#define timer_btn_init() (TIMSK0 |= (1 << OCIE0A))
#define BTN_HOLD_MS 50  // Press button wait


enum Device_Status
{
    POWER_OFF,
    RUNNING
};
enum Btn_Status
{
    BTN_UP,
    BTN_DOWN,
    BTN_IGNORE
};

Device_Status status = RUNNING; // Set start ON or OFF when power is connected
volatile unsigned int timer_btn;  // milliseconds counter 
//volatile unsigned int wd_flash;  // milliseconds counter 
Btn_Status btn_status = BTN_UP;        // Status of the button

int loop_cnt = 0;


void setup()
{  
  DDRB |= (1 <<  LED1|1 <<  LED2| 1 << LED3); //Set PB4, PB1 and PB0 as outputs
  PORTB |= (1 << BTN);    // Enable PULL_UP resistor
  GIMSK |= (1 << PCIE);   // Enable Pin Change Interrupts
  PCMSK |= (1 << BTN);    // Use PCINTn as interrupt pin (Button I/O pin)
  ADCSRA &= ~(1<<ADEN);  //Disable ADC
  TCCR0A |= (1 << WGM01); // Set CTC mode on Timer 1
  TIMSK0 |= (1 << OCIE0A); // Enable the Timer/Counter0 Compare Match A interrupt
  TCCR0B |= (1 << CS01);  // Set prescaler to 8
  OCR0A = 125;            // Set the output compare reg so tops at 1 ms
  WDTCR |= (0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(1<<WDP0); //125ms
  //WDTCR |= (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0); //4s to measure current
  WDTCR |= (1<<WDTIE);
  sei();                  // Enable interrupts
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop()
{    
  if (btn_status == BTN_DOWN)
  {
    if (timer_btn > BTN_HOLD_MS) // Check if button has been pressed enough
    {
      if (status == RUNNING)
      {
        //status = POWER_OFF;
        loop_cnt = loop_cnt+1;
      }          
      else
      {
        status = RUNNING;          
        // setup of the device here if needed;
      }
      btn_status = BTN_IGNORE; // If status already changed don't swap it again
    }
  }
  else
  {
    if (status) // Is status RUNNING?
    {
      /* main code here */
      manage_routines();
      if (loop_cnt>3)
        loop_cnt = 0;
      /* -------------- */
    }
    else
    {
      power_off();
    }
  }
}


void manage_routines()
{
  switch(loop_cnt)
  {
    case(0):
      PORTB |= (1<<LED3);
      PORTB &= ~(1<<LED2);
      PORTB &= ~(1<<LED1);
      power_off();
      PORTB &= ~(1<<LED3);
      PORTB |= (1<<LED2);
      PORTB &= ~(1<<LED1);
      power_off();
      PORTB &= ~(1<<LED3);
      PORTB &= ~(1<<LED2);
      PORTB |= (1<<LED1);
      power_off();
     
      break;
    case(1):
      PORTB &= ~(1<<LED3);
      PORTB &= ~(1<<LED2);
      PORTB |= (1<<LED1);
      power_off();
      PORTB &= ~(1<<LED3);
      PORTB |= (1<<LED2);
      PORTB &= ~(1<<LED1);
      power_off();
      PORTB |= (1<<LED3);
      PORTB &= ~(1<<LED2);
      PORTB &= ~(1<<LED1);
      power_off();
      break;
    case(2):
      PORTB |= (1<<LED3);
      PORTB &= ~(1<<LED2);
      PORTB &= ~(1<<LED1);
      power_off();
      PORTB &= ~(1<<LED3);
      PORTB |= (1<<LED2);
      PORTB &= ~(1<<LED1);
      power_off();
      PORTB &= ~(1<<LED3);
      PORTB &= ~(1<<LED2);
      PORTB |= (1<<LED1);
      power_off();
      PORTB &= ~(1<<LED3);
      PORTB |= (1<<LED2);
      PORTB &= ~(1<<LED1);
      power_off();
      break;
    case(3):
      PORTB |= (1 <<  LED1|1 <<  LED2| 1 << LED3);
      power_off();
      PORTB &= ~(1 <<  LED1|1 <<  LED2| 1 << LED3);
      power_off();
      break; 
    default:
      PORTB &= ~(1 <<  LED1|1 <<  LED2| 1 << LED3);
      status = POWER_OFF;  
  } 
}

void power_off()
{
  cli();                               // Disable interrupts before next commands
  //wdt_disable();                       // Disable watch dog timer to save power
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode power down
  sleep_enable();
  sleep_bod_disable(); // Disable brown-out detector
  sei();               // Enable interrupts
  sleep_cpu();
  sleep_disable();
}

ISR(PCINT0_vect)
{
  if (!((PINB >> BTN) & 0x01)) // Check if button is down
  {
    btn_status = BTN_DOWN;
    timer_btn_init();
    timer_btn = 0;
  }
  else
    btn_status = BTN_UP;
}

ISR(TIM0_COMPA_vect)
{
  timer_btn++;
}

ISR(WDT_vect) {
 
}
