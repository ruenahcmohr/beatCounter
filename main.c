/*

avr @ 16Mhz


on pro mini:

0   rxd    rxd
1   txd    txd

2   pd2    LCD d7
3   pd3    LCD d6
4   pd4    LCD d5
5   pd5    LCD d4
6   pd6    
7   pd7    

8   pb0    Input signal  
9   pb1    
10  pb2    


11  pb3    LCD /E
12  pb4    LCD RS
13  pb5    


A0  pc0    
A1  pc1    
A2  pc2    
A3  pc3    

A4  pc4    
A5  pc5    


A6  adc6  
A7  adc7  

LCD R/W goes to ground
LCD Vlcd goes to 0-5V contrast pot.


position 	1 	2 	3 	4 	5 	6 	7 	8 	9 	10 	11 	12 	13 	14 	15 	16
line 1          0       1       2       3       4       5       6       7       40      41      42      43      44      45      46      47



(some other screen)
line 1 		0 	1 	2	3 	4 	5	6 	7 	8 	9 	10 	11 	12 	13 	14 	15
line 2 		64 	65 	66 	67 	68 	69 	70 	71 	72 	73 	74 	75 	76 	77 	78 	79


*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "avrcommon.h"

#define OUTPUT  1
#define INPUT   0


volatile unsigned int   T1OFC;
volatile unsigned long  timeValue;
volatile unsigned char  update; 

void Delay2(unsigned int delay) ;
void Delay(unsigned int delay) ;
void timer1Init() ;
void triggerInit() ;


int main( void ) {
 
  unsigned long t;
  unsigned long capture;
 
    // set up directions 
  DDRB = (INPUT << PB0 | INPUT << PB1 | INPUT << PB2 |OUTPUT << PB3 |OUTPUT << PB4 |INPUT << PB5 |INPUT << PB6 |INPUT << PB7);
  DDRD = (INPUT << PD0 | INPUT << PD1 | OUTPUT << PD2 |OUTPUT << PD3 |OUTPUT << PD4 |OUTPUT << PD5 |INPUT << PD6 |INPUT << PD7);        
  DDRC = (INPUT << PC0 | INPUT << PC1 | INPUT << PC2 |INPUT << PC3 |INPUT << PC4 |INPUT << PC5 |INPUT << PC6 ); 

  T1OFC = 0;
  TCNT1 = 0;
  
  lcd_init();  
  timer1Init();  
  triggerInit();
  
  sei();
    
  lcd_cls();
  lcd_home();    
   
  while(1) { 
  
     if (T1OFC > 0x100) {  // this is a more-than-1-second-since-pulse timeout
        lcd_goto(0);
	lcd_putstr("      0 ") ;
	lcd_goto(40);
        lcd_putstr(" BPM    ") ;
	T1OFC = 0x101;  // lock it so it can't roll over
	          
     } else if (update) {   
       /*
       
        16Mhz, 32 bit counter, rollover takes 268 secs.
	
	1 pulse     16M ticks     60 seconds    1 min
	-------  x  ---------- x ----------- x ------------
        n ticks      1 second     1 minute      100 display units     
	
	
	=    9600000	
		              
       */
          	  	  
       lcd_goto(0);   // probably one of the most annoying things is to have a 16 character screen split in half
       capture = timeValue;
       
       t = 9600000/capture;
       
       if (t > 65248) {
        lcd_putstr(" Over Ma");
	lcd_goto(40);
	lcd_putstr("ch 1?!!") ;
       }else {       
        lcd_putstr("   ") ;
        print5(t);
	lcd_putstr("   ") ;
	lcd_goto(40);
        lcd_putstr(" BPM     ") ;
        update = 0;
       }
     }
     Delay2(8); // limit update rate
  }
    
}

void timer1Init() {

   // turn on overflow interrupt
   //SetBit(TOIE1, TIMSK1);
   TIMSK1 |= (1<<TOIE1);

   // divide of 1024
   // TCCR1B =  (1<<CS12)|(1<<CS10) ;
 
   // divide of 256
   //TCCR1B =  (1<<CS12);
   
   // divide of 8
   //TCCR1B =  (1<<CS11);
   
   // divide of 1
   TCCR1B =  (1<<CS10);


}


void triggerInit() {

  SetBit(PCIE0, PCICR);    // enable pin change interrupt group 0 (0-7)
  SetBit(PCINT0, PCMSK0);  // enable pin change for pcint0

}

// timer1overflow
ISR(TIMER1_OVF_vect ) { 
   T1OFC++;
}

// input state change
// leave if the input is low (else its a rising edge)
// grab the time value (with overflow)
// reset timer
// set flag for detected pulse

ISR(PCINT0_vect) {
  unsigned int t;
  
  t = TCNT1; // quick grab the value;
  
  if (IsLow(0, PINB)) return; // if the input is low, ignore.
  // else we have a rising edge or a glitch (hahahah)
  TCNT1 = 12; // timing error correction
  timeValue = t + ((unsigned long)T1OFC << 16);
  T1OFC = 0;
  update = 1; // sawPulse ;)
  
}



void Delay(unsigned int delay) {
  unsigned int x;
  for (x = delay; x != 0; x--) {
    asm volatile ("nop"::); 
  }
}

void Delay2(unsigned int delay) {
  unsigned int x;
  for (x = delay; x != 0; x--) {
    Delay(65000);
  }
}















