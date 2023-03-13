
/********************************************************************
 Software License Agreement:
 *******************************************************************/
#include <system.h>
#include <pixel.h>

/** Type defintions *********************************/
#define DATA LATAbits.LATA4

static uint8_t PIXEL_ARRAY[3*PIXEL_COUNT];
static uint8_t base_effect;

static uint8_t animate_count;
static signed char larsen_index;
static uint8_t larsen_count;
static signed char larsen_dir;  // Animation Direction


// Minimal Color Map, these get shifted Left toa animate a flash, 
// and then dimming when pressed
// -------------
const uint8_t ColorMap[12*3]={
           0x08,    0,       0 ,
           0x04, 0x01, 0,
           0x02, 0x02, 0,
           0x01, 0x04, 0,
           0     , 0x08    , 0,
           0     , 0x04 , 0x01,
           0     , 0x02 , 0x02,
           0     , 0x01 , 0x04,
           0     , 0       , 0x08,
           0x01  , 0      , 0x04,
           0x02  , 0      , 0x02,
           0x04  , 0      , 0x01
};

uint8_t KeyFlash[12];

void PIXEL_flashKey(char bid) {
    KeyFlash[bid] = 5;
    animate_count = 1;
}

void PIXEL_setBase(char bid, uint8_t r, uint8_t g, uint8_t b) {
   if (bid<0)
     return;

   if (bid>8)
     return;

   uint8_t offset = (12+bid)*3;  // Normallize then add to BASE offset

   PIXEL_ARRAY[offset] = g;
   PIXEL_ARRAY[offset+1] = r;
   PIXEL_ARRAY[offset+2] = b;
}

void PIXEL_floodBase(uint8_t r, uint8_t g, uint8_t b) {
  char j;
  for (j=0;j<9;j++) {
    PIXEL_setBase(j,r,g,b);
  }
}

void PIXEL_default(void) {
  uint8_t i;
  for (i=0;i<36;i++) {
     PIXEL_ARRAY[i]=ColorMap[i];
  }
  PIXEL_floodBase(0x40,0,0x30);
}

void PIXEL_Enable(void){
    TRISA &= ~(1<<4); // LATA4 should be an output
    PIXEL_default();

    larsen_index = -5;
    larsen_dir = 1;
    larsen_count = 63;
};


// macro to send the bit 'b' (can be either 0 or 1)
static void send_0() {
   asm("BANKSEL LATA");
   asm("BSF LATA,4");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("BCF LATA,4");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("BCF LATA,4");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("nop");
};

static void send_1() {
   asm("BANKSEL LATA");
   asm("BSF LATA,4");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("BSF LATA,4");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("BCF LATA,4");
   asm("nop");
   asm("nop");
   asm("nop");
   asm("nop");
};

// auxiliary functions to control the WS2812 NeoPixel LEDs

// send out a byte b in WS2812 protocol
void static sendByte (unsigned char b) {

    if (b & 0b10000000) { send_1(); } else { send_0(); }
    if (b & 0b01000000) { send_1(); } else { send_0(); }
    if (b & 0b00100000) { send_1(); } else { send_0(); }
    if (b & 0b00010000) { send_1(); } else { send_0(); }
    if (b & 0b00001000) { send_1(); } else { send_0(); }
    if (b & 0b00000100) { send_1(); } else { send_0(); }
    if (b & 0b00000010) { send_1(); } else { send_0(); }
    if (b & 0b00000001) { send_1(); } else { send_0(); }
}

void PIXEL_sendArray (void) {
  uint8_t i;
  asm("bcf INTCON,7"); // GIE = 7

  for (i=0;i<sizeof(PIXEL_ARRAY);i++) {
     sendByte(PIXEL_ARRAY[i]);
  }
  asm("bsf INTCON,7"); // GIE = 7
}

void PIXEL_setPixel(uint8_t index, uint8_t r,uint8_t g, uint8_t b) {
   uint8_t offset = (index%12)*3;

   if (index<21) {
      PIXEL_ARRAY[offset] = g;
      PIXEL_ARRAY[offset+1] =r;
      PIXEL_ARRAY[offset+2] =b;
   }
}


void PIXEL_setBaseEffect(uint8_t effect) {
   base_effect = effect;
}

static void DrawLarsen(signed char index) {
   PIXEL_setBase(index,   0x10, 0, 0);
   PIXEL_setBase(index+1, 0x20, 0, 0);
   PIXEL_setBase(index+2, 0x40, 0x10, 0);
   PIXEL_setBase(index+3, 0x20, 0, 0);
   PIXEL_setBase(index+4, 0x10, 0, 0);
}

#define LARSEN_SPEED (96)

static void larsen_tick(void) {
   larsen_count--;
   if (larsen_count==0) {
      larsen_index += larsen_dir;
      larsen_count = LARSEN_SPEED;

      PIXEL_floodBase(0x0,0x0,0x00);
      DrawLarsen(larsen_index);

      if (larsen_index>0x08) {
         larsen_dir = -1;
      } else if (larsen_index == -5) {
         larsen_dir = 1;
      }

   }
}

static void alert_base(void) {
   larsen_count++;
   switch(larsen_count) {
      case 0:   PIXEL_floodBase(0,0,0); break;
      case 32:  PIXEL_floodBase(0,0x40,0); break;
      case 96:  PIXEL_floodBase(0,0,0); break;
      case 160: PIXEL_floodBase(0,0x40,0); break;
      default: break;
   }
};


void PIXEL_Animate() {
   // Chase ... 
   uint8_t i;
   animate_count--;
   if (animate_count == 0) {
      animate_count = 48;
      for (i=0;i<12;i++) {
         if (KeyFlash[i]) {
            uint8_t offset = 0;
            KeyFlash[i]--;
            offset = KeyFlash[i];
            PIXEL_ARRAY[i*3] =  ColorMap[(i*3)]<<offset;
            PIXEL_ARRAY[(i*3)+1] = ColorMap[(i*3)+1]<< offset;
            PIXEL_ARRAY[(i*3)+2] = ColorMap[(i*3)+2]<< offset;
         }
      }
   }

   // BASE Animation
   switch(base_effect) {
      case BASE_EFFECT_LARSON:
         larsen_tick();
         break;
      case BASE_EFFECT_SOLID:
         PIXEL_floodBase(0x40,0x00,0x0);
         break;
      case BASE_EFFECT_ALERT:
         alert_base();
         break;
      default: // BASE_EFFECT_NONE
         PIXEL_floodBase(0x0,0x0,0x0);
         break;
   }

}
