/********************************************************************
 Software License Agreement:

 ToDo: Place an open license here so that this code can be shared.
 *******************************************************************/

#include <xc.h>
#include <stdbool.h>
#include <matrix.h>

/*** MATRIX Definitions *********************************************/
// 16F145x RA0, RA1, and RA3 are input only
// Inputs
#define LATCH_ROW1 LATCbits.LATC4
#define LATCH_ROW2 LATCbits.LATC5
#define LATCH_ROW3 LATAbits.LATA5

#define TRIS_ROW1  TRISCbits.TRISC4
#define TRIS_ROW2  TRISCbits.TRISC5
#define TRIS_ROW3  TRISAbits.TRISA5

// Outputs
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
uint8_t MATRIX_pinMap[2];
uint8_t MATRIX_changeMap[2];
bool    MATRIX_bChanged;

// ---------------------
// Internal Variables
// ---------------------

void MATRIX_SetRow(ROWID row_id) {
   if (row_id==ROW1) {
     LATCH_ROW1 = ROW_ENABLE;
     LATCH_ROW2 = ROW_DISABLE;
     LATCH_ROW3 = ROW_DISABLE;
     TRISC = 0x0F;
   } else if (row_id == ROW2) {
     LATCH_ROW1 = ROW_DISABLE;
     LATCH_ROW2 = ROW_ENABLE;
     LATCH_ROW3 = ROW_DISABLE;
     TRISC = 0x0F;
   } else if (row_id == ROW3) {
     LATCH_ROW1 = ROW_DISABLE;
     LATCH_ROW2 = ROW_DISABLE;
     LATCH_ROW3 = ROW_ENABLE;
     TRISC = 0x0F;
   } else { // ROW_NONE or out of range
     LATCH_ROW1 = ROW_DISABLE;
     LATCH_ROW2 = ROW_DISABLE;
     LATCH_ROW3 = ROW_DISABLE;
     TRISC = 0x00; // Set the lines as OUTPUT (pre-configured to LOW)
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
   MATRIX_bChanged = 0;

   ANSELA = 0;
   ANSELC = 0;

   // Disable all rows
   MATRIX_SetRow(ROW_NONE);

   // Configure TRIS registers for rows
   TRIS_ROW1 = PIN_OUTPUT;
   TRIS_ROW2 = PIN_OUTPUT;
   TRIS_ROW3 = PIN_OUTPUT;


   // Columns: Initially COLS are OUTPUT driven LOW.
   TRISC = 0x00;
   LATC = LATC&0xF0;

   MATRIX_bChanged = false; // No data pending transmission
   MATRIX_changeMap[0] = 0;
   MATRIX_changeMap[1] = 0;
   MATRIX_pinMap[0] = 0;
   MATRIX_pinMap[1] = 0;
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
   do {
      for (i=ROW1;i<ROW_NONE;i++) {
         MATRIX_SetRow(i);
         temp = MATRIX_ReadCols();
         MATRIX_SetRow(ROW_NONE);
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
         MATRIX_SetRow(i);
         temp = MATRIX_ReadCols();  // Read a second time and compare
         MATRIX_SetRow(ROW_NONE);
         if (i==ROW1) {
            lastRead[0] |= temp;
         } else if (i==ROW2) {
            lastRead[0] |= (temp<<4);
         } else if (i==ROW3) {
            lastRead[1] |= temp;
         }
      }
   } while ((currentRead[0]!=lastRead[0]) || (currentRead[1]!=lastRead[1]));


   //
   // XOR to find differences - & to limit difference for 0-->1 transitions
   // Want to flag any transitions from 0 --> 1  ..
   //
   if (MATRIX_pinMap[0] != currentRead[0]) {
      MATRIX_changeMap[0] = (MATRIX_pinMap[0] ^ currentRead[0])&currentRead[0];
      MATRIX_bChanged = true;
      MATRIX_pinMap[0] = currentRead[0];
   }

   if (MATRIX_pinMap[1] != currentRead[1]) {
      MATRIX_changeMap[1] = (MATRIX_pinMap[1] ^ currentRead[1])&currentRead[1];
      MATRIX_bChanged = true;
      MATRIX_pinMap[1] = currentRead[1];
   }
   return true; // We have had a successfull READ of the matrix
}
