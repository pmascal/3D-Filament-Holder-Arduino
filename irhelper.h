//
//  Why ifndef?  So errors are not issued if header included more than once.
//
// Check if variabled defined
#ifndef IRHELPER_H
//  If not defined, then define symbol
#define IRHELPER_H
// Library for sending and and receiving IR data.
#include <IRremote.h>

#define KEY_POWER      0xFFA25D
#define KEY_MENU       0xFFE21D
#define KEY_TEST       0xFF22DD
#define KEY_PLUS       0xFF02FD
#define KEY_RETURN     0xFFC23D 
#define KEY_BACK       0xFFE01F
#define KEY_PLAY       0xFFA857
#define KEY_FORWARD    0xFF906F
#define KEY_0          0xFF6897
#define KEY_MINUS      0xFF9867
#define KEY_C          0xFFB04F
#define KEY_1          0xFF30CF
#define KEY_2          0xFF18E7
#define KEY_3          0xFF7A85
#define KEY_4          0xFF10EF
#define KEY_5          0xFF38C7
#define KEY_6          0xFF5AA5
#define KEY_7          0xFF42BD
#define KEY_8          0xFF4AB5
#define KEY_9          0xFF52AD

// Set the Arduino PIN on which data will be recieved.
const int RECV_PIN = 12;
// Create a receiver object using a name of your choice (i.e., IRRecv). 
IRrecv myIRObj(RECV_PIN);
//
//  Defines data structure for data received from IR code.
//   results.decode_type: Will be one of the following: NEC, SONY, RC5, RC6, or UNKNOWN. 
//   results.value: The actual IR code (0 if type is UNKNOWN) 
//   results.bits: The number of bits used by this code 
//   results.rawbuf: An array of IR pulse times 
//   results.rawlen: The number of items stored in the array 
//
decode_results objResults;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IRInitialize()
{
  // Begin the receiving process. This will enable the timer interrupt
  // which consumes a small amount of CPU every 50 µs.
  myIRObj.enableIRIn();
  // Enable blinking the LED when during reception.
  // Because you can't see infrared light, blinking the LED
  // can be useful while troubleshooting, or just to give
  // visual feedback.
  myIRObj.blink13(true);
}

#endif /* IRHELPER_H */
