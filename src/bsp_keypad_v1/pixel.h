/********************************************************************
 Software License Agreement:
 *******************************************************************/

#ifndef PIXEL_H
#define PIXEL_H

#include <stdbool.h>

typedef unsigned char uint8_t;
/** Type defintions *********************************/
#define PIXEL_COUNT 21

#define BASE_EFFECT_SOLID  0
#define BASE_EFFECT_LARSON 1
#define BASE_EFFECT_ALERT  2
#define BASE_EFFECT_NONE   3


void PIXEL_Enable(void);
void PIXEL_Animate(void);
void PIXEL_setBaseEffect(uint8_t effect);
void PIXEL_sendArray (void);
void PIXEL_setPixel(uint8_t index, uint8_t r,uint8_t g, uint8_t b);
void PIXEL_floodBase(uint8_t r, uint8_t g, uint8_t b);
void PIXEL_flashKey(char bid);
#endif //PIXEL_H
