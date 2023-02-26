/********************************************************************
 Software License Agreement:

 ToDo: Place an open license here so that this code can be shared.
 *******************************************************************/

#include <xc.h>
#include <stdbool.h>
#include <matrix.h>

/*** MATRIX Definitions *********************************************/
// 16F145x RA0, RA1, and RA3 are input only
#define PORT_ROW1 PORTCbits.RC4
#define PORT_ROW2 PORTCbits.RC5
#define PORT_ROW3 PORTAbits.RA5

#define TRIS_ROW1  TRISCbits.TRISC4
#define TRIS_ROW2  TRISCbits.TRISC5
#define TRIS_ROW3  TRISAbits.TRISA5

#define PORT_COL1  PORTCbits.RC0
#define PORT_COL2  PORTCbits.RC1
#define PORT_COL3  PORTCbits.RC2
#define PORT_COL4  PORTCbits.RC3

#define TRIS_COL1  TRISCbits.TRISC0
#define TRIS_COL2  TRISCbits.TRISC1
#define TRIS_COL3  TRISCbits.TRISC2
#define TRIS_COL4  TRISCbits.TRISC3

typedef enum {
   ROW1, 
   ROW2, 
   ROW3,
   ROW_NONE
} ROWID;

#define MATRIX_PRESSED      1
#define MATRIX_NOT_PRESSED  0

#define PIN_INPUT           1
#define PIN_OUTPUT          0

#define ROW_ENABLE        1
#define ROW_DISABLE       0

// --------------------
// Exported Variables
// --------------------
uint8_t MATRIX_PinMap[2];

// ---------------------
// Internal Variables
// ---------------------

bool   MATRIX_bchanged;       // Indicates if the READ vs PREVIOUS is different
static UINT8_t ChangedMap[2];  // History of Previous Keypresses


void MATRIX_SetRow(ROWID row_id) {
   if (row_id==ROW1) {
     PORT_ROW1 = ROW_ENABLE;
     PORT_ROW2 = ROW_DISABLE;
     PORT_ROW3 = ROW_DISABLE;
   } else if (row_id == ROW2) {
     PORT_ROW1 = ROW_DISABLE;
     PORT_ROW2 = ROW_ENABLE;
     PORT_ROW3 = ROW_DISABLE;
   } else if (row_id == ROW3) {
     PORT_ROW1 = ROW_DISABLE;
     PORT_ROW2 = ROW_DISABLE;
     PORT_ROW3 = ROW_ENABLE;
   } else { // ROW_NONE or out of range
     PORT_ROW1 = ROW_DISABLE;
     PORT_ROW2 = ROW_DISABLE;
     PORT_ROW3 = ROW_DISABLE;
   }
}

/*********************************************************************
* Function: void MATRIX_Enable( void );
*
* Overview: Configures TRIS registers for the MATRIX
*
* PreCondition: none
* Input: none
* Output: None
*
********************************************************************/
void MATRIX_Enable(void)
{

   // Disable all rows
   MATRIX_SetRow(ROW_NONE);

   // Configure TRIS registers for row/colums
   TRIS_ROW1 = PIN_OUTPUT;
   TRIS_ROW2 = PIN_OUTPUT;
   TRIS_ROW3 = PIN_OUTPUT;

   TRIS_COL1 = PIN_INPUT;
   TRIS_COL2 = PIN_INPUT;
   TRIS_COL3 = PIN_INPUT;
   TRIS_COL4 = PIN_INPUT; 

   // Might need to disable Weak Pullups here.

   MATRIX_bchanged = false; // No data pending transmission
   changedMap[0] = 0;
   changedMap[1] = 0;
}

#define MATRIX_ReadCols() (PORTC&0x0F)

/*********************************************************************
* Function: bool MATRIX_Poll( void );
*
* Overview: Attempt to read data (indicats if we have a consistent read)
*
* PreCondition: none
* Input: none
* Output: populates the Changed array and returns if the read was clean
*
********************************************************************/
static  uint8_t currentRead[2];
static  uint8_t lastRead[2]; 

bool MATRIX_Poll(void)
{
   ROWID i;
   uint8_t temp;

   // Using OR below, so need to zero these out ahead of time.
   currentRead[0] = 0;
   currentRead[1] = 0;
   lastRead[0] = 0;
   lastRead[1] = 0;
   repeat {
      for (i=ROW1;i<ROW_NONE;i++) {
         MATRIX_SetRow(i);
         temp = MATRIX_ReadCols();
         if (i==ROW1) {
            currentRead[0] |= temp;
         } else if (i==ROW2) {
            currentRead[0] |= (temp<<4);
         } else if (i==ROW3) {
            currentRead[1] |= temp;
         }

         // Might need a pause in here.. *or* update to ASYNC, requiring two 
         // reads to this routine. Only when we have two successive reads that 
         // match, do we report OK.
         temp = MATRIX_ReadCols();  // Read a second time and compare
         if (i==ROW1) {
            lastRead[0] |= temp;
         } else if (i==ROW2) {
            lastRead[0] |= (temp<<4);
         } else if (i==ROW3) {
            lastRead[1] |= temp;
         }
      } 
   } until ((currentRead[0]==lastRead[0]) && (currentRead[1]==lastRead[1]));

   if (changedMap[0] != currentRead[0]) {
      changedMap[0] = currentRead[0];
      MATRIX_bchanged = true;
   }

   if (changedMap[1] != currentRead[1]) {
      changedMap[1] = currentRead[1];
      MATRIX_bchanged = true;
   }
   return true; // We have had a successfull READ of the matrix
}   
