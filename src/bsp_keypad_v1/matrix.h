/********************************************************************
 Keypad Matrix polling routines
 *******************************************************************/

#include <stdbool.h>

#ifndef MATRIX_H
#define MATRIX_H

/*** MATRIX Definitions *********************************************/

typedef unsigned char uint8_t;

// Each bit represents the state of a key in the MATRIX
//    1 = pressed
//    0 = released

extern uint8_t MATRIX_pinMap[2];
extern uint8_t MATRIX_changeMap[2];
extern bool    MATRIX_bChanged;

/*********************************************************************
* Function: bool MATRIX_Poll(void);
*
* Overview: Returns indication if we have consistent data read from the Matrix
*
* PreCondition: matrix configured via MATRIX_Enable()
*
* Input: none
*
* Output: TRUE if pressed; FALSE if not pressed.
*
********************************************************************/
bool MATRIX_Poll(void);

/*********************************************************************
* Function: void MATRIX_Enable(void);
*
* Overview: Configures TRIS registers for the MATRIX
*
* PreCondition: none
*
* Input: none
*
* Output: None
*
********************************************************************/
void MATRIX_Enable(void);

/*********************************************************************
* Function: void MATRIX_Changed(void);
*
* Overview: Compares MATRIX_pinMap with previous data, and returns
*           TRUE if the data has changed.
*
* PreCondition: none
*
* Input: none
*
* Output: None
*
********************************************************************/
#define MATRIX_Changed()  (MATRIX_bChanged)

#endif //MATRIX_H
