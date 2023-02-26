/********************************************************************
 Keypad Matrix polling routines
 *******************************************************************/

#include <stdbool.h>

#ifndef MATRIX_H
#define MATRIX_H

/*** MATRIX Definitions *********************************************/

// Each bit represents the state of a key in the MATRIX 
//    1 = pressed
//    0 = released

extern uint8_t MATRIX_PinMap[2]; 
extern bool    MATRIX_Changed;

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
* Overview: Compares MATRIX_PinMap with previous data, and returns
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
