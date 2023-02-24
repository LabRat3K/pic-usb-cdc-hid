//; vim:noet:sw=8:ts=8:ai:syn=pic
#include <xc.h>
#include "log.h"

// Constants
#define FOSC		48000000
#define BAUD		250000
#define BAUDVAL		(FOSC/(16*BAUD))-1	// BRG16=0, BRGH=1

void uart_init(void);
void log_init(void);
// Linear address of the 256-byte buffer.
//; Must be aligned to a 256-byte boundary.
unsigned char LOG_BUFFER[256] __at(0x2200);

// Banked RAM locations used by logging system
// (directly before the log buffer)
volatile unsigned char LOG_HEAD;
volatile unsigned char LOG_TAIL;
volatile unsigned char LOG_FMT_FLAGS;

void uart_init(void) {
   SPBRG = BAUDVAL;
   TXSTAbits.BRGH=1; //high speed
   RCSTAbits.SPEN=1; //enable serial port
   TXSTAbits.TXEN=1; //enable transmission

   TRISCbits.TRISC4 = 0; // UART Rx as input
   TRISCbits.TRISC5 = 1; // UART Tx as output
}

void log_init(void) {
   int i;
   LOG_HEAD = 0;
   LOG_TAIL = 0;
   LOG_FMT_FLAGS = 0;
   for (i=0;i<255;i++) {
      LOG_BUFFER[i]=0;
   }
}

void log_service(void) {

   if (isEmpty()) return; // if Head==Tail, buffer is empty
   if (PIR1bits.TXIF==0) return; // Serial output is busy come back later

   TXREG  = LOG_BUFFER[LOG_TAIL];
   LOG_TAIL++;
}
	

void log_byte(char data) {
   LOG_BUFFER[LOG_HEAD] = data;
   LOG_HEAD++;
}

const unsigned char HEXMAP[16] = {'0','1','2','3','4','5','6','7',
                            '8','9','A','B','C','D','E','F'};

void set_format(unsigned char FMT) {
   LOG_FMT_FLAGS = (LOG_FMT_FLAGS & 0X0F) | (FMT&0xF0);
}

void set_multi(unsigned char number, unsigned char FMT) {
   LOG_FMT_FLAGS = (FMT&0xF0) | (number & 0x0F);
}

void log_hex(char data,unsigned char FMT) {
   if (FMT&FMT_SPACE) {
      log_byte(' ');   
   }
   log_byte(HEXMAP[data>>4]);
   log_byte(HEXMAP[data&0x0F]);
   if (FMT&FMT_EOL) {
      log_byte('\n');
   }
}

void log_char(char data, unsigned char FMT) {
   if (FMT&FMT_HEX) {
      log_hex(data,FMT);
   } else {
      if (FMT&FMT_SPACE) log_byte(' ');
      log_byte(data);
      if (FMT&FMT_EOL) log_byte('\n');
   }
}

void mlog_hex(char data) {
   log_hex(data,LOG_FMT_FLAGS);
   LOG_FMT_FLAGS = LOG_FMT_FLAGS-1;
   if ((LOG_FMT_FLAGS&0x0F) ==0) {
      LOG_FMT_FLAGS = 0;
   }
} 
