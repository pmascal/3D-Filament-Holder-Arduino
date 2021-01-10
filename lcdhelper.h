//
//  Why ifndef?  So errors are not issued if header included more than once.
//
// Check if variabled defined
#ifndef LCDHELPER_H
//  If not defined, then define symbol
#define LCDHELPER_H
// Library for sending and and receiving IR data.
//#include <UTFT.h>

//UTFT lcd(ILI9163_4L,3,2,9,10,7);

// Declare which fonts we will be using
// extern dlcare this variable without 
// allocating memory to the variable.  The
// memory is actually allocated in the header.
//extern uint8_t SmallFont[]; //SmallFont[]
//extern uint8_t BigFont[]; //BigFont[]
//  Declare screens
enum screen
{
    SCROLL1 = 1,
    SCROLL2 = 2,
    SCROLL3 = 3,
    SCROLL4 = 4,
    SCROLL_PROJECTS1 = 24,
    SCROLL_PROJECTS2 = 5,
    SCROLL_PROJECTS3 = 6,
    SCROLL_UNITS1 = 7,
    SCROLL_UNITS2 = 8,
    SCROLL_UNITS3 = 9,
    MAIN2_1 = 11,
    MAIN2_2 = 12,
    MAIN2_SUB1_1 = 13,
    MAIN2_SUB1_2 = 14,
    MAIN2_SUB2 = 15,
    MAIN2_SUB3 = 16,
    MAIN3 = 17,
    SCROLL_PRINTS1_1 = 18,
    SCROLL_PRINTS1_2=19,
    SCROLL_PRINTS2_1=20,
    SCROLL_PRINTS2_2=21,
    SCROLL_PRINTS3_1=22,
    SCROLL_PRINTS3_2=23
    
};
// Declare variable to hold screen state
short int SCREEN_STATE=0;
//
//  Variable to store the current state of a screen.
//  For instance, a menu screen might display 
//  different information depending on the state
//  of the input
//
String OPTION_STATE="";


#endif /* IRHELPER_H */
