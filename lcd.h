 /***********************************************************
  *   Program:    LCD driver header
  *   Created:    27.8.99 20:26
  *   Author:     Brajer Vlado
  *                 vlado.brajer@kks.s-net.net
  *   Comments:   functions & definitions
  ************************************************************/
// LCD is connected on PORTC in 4bit mode
#include <avr/io.h>
#include <stdio.h>
#include <stdarg.h>
#include "avrcommon.h"

#define E_PORT  PORTB
#define E_PIN   3

#define RSPORT  PORTB
#define RSPIN   4

// data bits are on port C0-3

typedef char byte;

void lcd_delay(unsigned int p);
void toggle_E();
void sendNibble(byte n);
void sendDataByte(byte c);
void sendControlByte(byte c);
void lcd_cls();
void lcd_home();
void lcd_control(byte disonoff, byte curonoff, byte curblink);
void lcd_goto(byte mesto);
void lcd_putch(byte data);
void lcd_putstr(char *data);
void printbin(byte x);
void printhex(byte i);
void print2(unsigned int x);
void print3(unsigned int x);
void print5(unsigned int x);
void print10(unsigned long x);
void lcd_init(void);
int printf(const char *format, ...);
