#include "irhelper.h"
#include "lcdhelper.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

// Creates arrow symbols to direct user on display
byte uparrow[8] = {
  0b00100,
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000
};

byte downarrow[8] = {
  0b00000,
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00100
};

byte returnarrow[8] = {
  0b00100,
  0b01000,
  0b11111,
  0b01001,
  0b00101,
  0b00001,
  0b00000,
  0b00000
};

byte rightarrow[8] = {
  0b00000,
  0b10100,
  0b01010,
  0b00101,
  0b00101,
  0b01010,
  0b10100,
  0b00000
};

byte leftarrow[8] = {
  0b00000,
  0b00101,
  0b01010,
  0b10100,
  0b10100,
  0b01010,
  0b00101,
  0b00000
};

String scr;
float calib = 388.2;
int tempmass[4] = {0, 0, 0, 0};
int temp_mass;
int spool_mass = 1;
int mass1 = 1;
int mass2 = 1;
int mass3 = 1;
int project1[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
int project2[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
int project3[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
int print_mass = 0;
unsigned long print_time;
const int filament = 1000;
float val;
int val_mass;
int val_perc;
int val_volume;
int i = 0;
int y = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Show's list of saved spools used for a few different situations
void spool_list(String title)
{
  lcd.print(title);
  lcd.setCursor(0, 1);
  lcd.print("1");
  lcd.setCursor(3, 1);
  lcd.print("2");
  lcd.setCursor(6, 1);
  lcd.print("3");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calc_spool_mass()
{
  if (spool_mass == mass1)
  {
    mass1 -= print_mass;
    project1[3][0] = project1[2][0];
    project1[3][1] = project1[2][1];
    project1[2][0] = project1[1][0];
    project1[2][1] = project1[1][1];
    project1[1][0] = project1[0][0];
    project1[1][1] = project1[0][1];
    project1[0][0] = print_mass;
    project1[0][1] = print_time;
  }
  else if (spool_mass == mass2)
  {
    mass2 -= print_mass;
    project2[3][0] = project2[2][0];
    project2[3][1] = project2[2][1];
    project2[2][0] = project2[1][0];
    project2[2][1] = project2[1][1];
    project2[1][0] = project2[0][0];
    project2[1][1] = project2[0][1];
    project2[0][0] = print_mass;
    project2[0][1] = print_time;
  }
  else if (spool_mass == mass3)
  {
    mass3 -= print_mass;
    project3[3][0] = project3[2][0];
    project3[3][1] = project3[2][1];
    project3[2][0] = project3[1][0];
    project3[2][1] = project3[1][1];
    project3[1][0] = project3[0][0];
    project3[1][1] = project3[0][1];
    project3[0][0] = print_mass;
    project3[0][1] = print_time;
  }
  spool_mass -= print_mass;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calc_offset(int filwt)
{
  lcd.setCursor(0, 1);
  lcd.print(" Calculating...");
  float missing = filament - filwt;
  float total = scale.get_units(50) + missing;
  float offset1 = total - filament;
  scale.set_offset((offset1 * calib) + scale.get_offset());
  ShowDisplay(SCROLL1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// interprets the remote key pressed as an integer
int TranslateKey(unsigned long int result)
{
  if (result == KEY_0) {
    return 0;
  }
  else if (result == KEY_1) {
    return 1;
  }
  else if (result == KEY_2) {
    return 2;
  }
  else if (result == KEY_3) {
    return 3;
  }
  else if (result == KEY_4) {
    return 4;
  }
  else if (result == KEY_5) {
    return 5;
  }
  else if (result == KEY_6) {
    return 6;
  }
  else if (result == KEY_7) {
    return 7;
  }
  else if (result == KEY_8) {
    return 8;
  }
  else if (result == KEY_9) {
    return 9;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessKeyPress(unsigned long int result)
{
  // Return to the main menu any time the
  // menu key is pressed.
  if (result == KEY_MENU)
  {
    if (SCREEN_STATE == MAIN2_SUB1_1 || SCREEN_STATE == MAIN2_SUB1_2 || SCREEN_STATE == MAIN2_SUB2)
    {
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("INPUT");
      lcd.setCursor(4, 1);
      lcd.print("CANCELLED");
      delay(3000);
      ShowDisplay(MAIN2_1);
    }
    ShowDisplay(SCROLL1);
  }
  // if "clear" key was pressed, tare the scale
  else if (result == KEY_C)
  {
    scale.tare(25);
    spool_mass = 1;
  }
  else if (result == KEY_POWER)
  {
    scale.power_down();
    lcd.noBacklight();
    ShowDisplay(SCROLL1);
    myIRObj.resume();
    while (y < 1)
    {
      if (myIRObj.decode(&objResults))
      {
        if (objResults.value != 0XFFFFFFFF)
        {
          if (objResults.value == KEY_POWER || objResults.value == KEY_MENU || objResults.value == KEY_TEST || objResults.value == KEY_PLUS || objResults.value == KEY_RETURN || objResults.value == KEY_BACK || objResults.value == KEY_PLAY ||
              objResults.value == KEY_FORWARD || objResults.value == KEY_0 || objResults.value == KEY_MINUS || objResults.value == KEY_C || objResults.value == KEY_1 || objResults.value == KEY_2 ||
              objResults.value == KEY_3 || objResults.value == KEY_4 || objResults.value == KEY_5 || objResults.value == KEY_6 || objResults.value == KEY_7 || objResults.value == KEY_8 || objResults.value == KEY_9)
          {
            scale.power_up();
            lcd.backlight();
            y = 1;
          }
        }
      }
    }
    y = 0;
  }
  else
  {
    if (SCREEN_STATE == SCROLL1) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        action();
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(SCROLL2);
      }
      else
      {
        ShowDisplay(SCROLL1);
      }
    }
    else if (SCREEN_STATE == SCROLL2) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        ShowDisplay(MAIN2_1);
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(SCROLL1);
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(SCROLL3);
      }
      else
      {
        ShowDisplay(SCROLL2);
      }
    }
    else if (SCREEN_STATE == SCROLL3) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        ShowDisplay(SCROLL_PROJECTS1);
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(SCROLL2);
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(SCROLL4);
      }
      else
      {
        ShowDisplay(SCROLL3);
      }
    }
    else if (SCREEN_STATE == SCROLL4) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        ShowDisplay(SCROLL_UNITS1);
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(SCROLL3);
      }
      else
      {
        ShowDisplay(SCROLL4);
      }
    }
    else if (SCREEN_STATE == SCROLL_PROJECTS1) //////////////////////////////////////////////
    {
      lcd.clear();
      if (result == KEY_PLAY) // Start project process
      {
        if (spool_mass == 1)
        {
          lcd.print("No spool set");
          delay(4000);
          ShowDisplay(SCROLL2);
        }
        else
        {
          lcd.print("Project started");
          print_mass = scale.get_units(25);
          print_time = millis();
          delay(2000);
          ShowDisplay(SCROLL1);
        }
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(SCROLL_PROJECTS2);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL3);
      }
      else
      {
        ShowDisplay(SCROLL_PROJECTS1);
      }
    }
    else if (SCREEN_STATE == SCROLL_PROJECTS2) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY) // End project process
      {
        lcd.clear();
        if (print_mass == 0)
        {
          lcd.clear();
          lcd.print("No project");
          lcd.setCursor(0, 1);
          lcd.print("started");
          delay(4000);
          ShowDisplay(SCROLL_PROJECTS1);
        }
        else
        {
          lcd.print("Project ended");
          print_mass -= scale.get_units(25);
          print_time = ((millis() - print_time) / 1000) / 60;

          delay(2000);

          lcd.clear();
          lcd.print("Project info:");
          lcd.setCursor(0, 1);
          lcd.print(print_mass);
          lcd.print("g");
          lcd.setCursor(8, 1);
          lcd.print(print_time);
          lcd.print("min");
          calc_spool_mass();
          print_time = 0;
          print_mass = 0;
          delay(3000);
          ShowDisplay(SCROLL1);
        }
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(SCROLL_PROJECTS1);
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(SCROLL_PROJECTS3);
      }
      else
      {
        ShowDisplay(SCROLL_PROJECTS2);
      }
    }
    else if (SCREEN_STATE == SCROLL_PROJECTS3) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        ShowDisplay(MAIN3);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL3);
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(SCROLL_PROJECTS2);
      }
      else
      {
        ShowDisplay(SCROLL_PROJECTS3);
      }
    }
    else if (SCREEN_STATE == SCROLL_UNITS1) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        scr = "Mass";
        lcd.clear();
        lcd.print(" Display is in");
        lcd.setCursor(5, 1);
        lcd.print(scr);
        delay(2000);
        ShowDisplay(SCROLL1);
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(SCROLL_UNITS2);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL4);
      }
      else
      {
        ShowDisplay(SCROLL_UNITS1);
      }
    }
    else if (SCREEN_STATE == SCROLL_UNITS2) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        scr = "Volume";
        lcd.clear();
        lcd.print(" Display is in");
        lcd.setCursor(5, 1);
        lcd.print(scr);
        delay(2000);
        ShowDisplay(SCROLL1);
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(SCROLL_UNITS1);
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(SCROLL_UNITS3);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL4);
      }
      else
      {
        ShowDisplay(SCROLL_UNITS2);
      }
    }
    else if (SCREEN_STATE == SCROLL_UNITS3) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        scr = "Percentage";
        lcd.clear();
        lcd.print(" Display is in");
        lcd.setCursor(3, 1);
        lcd.print(scr);
        delay(2000);
        ShowDisplay(SCROLL1);
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(SCROLL_UNITS2);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL4);
      }
      else
      {
        ShowDisplay(SCROLL_UNITS3);
      }
    }
    else if (SCREEN_STATE == MAIN2_1) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        scr = "New spool";
        action();
      }
      else if (result == KEY_MINUS)
      {
        ShowDisplay(MAIN2_2);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL2);
      }
      else
      {
        ShowDisplay(MAIN2_1);
      }
    }
    else if (SCREEN_STATE == MAIN2_2) //////////////////////////////////////////////
    {
      if (result == KEY_PLAY)
      {
        ShowDisplay(MAIN2_SUB3);
      }
      else if (result == KEY_PLUS)
      {
        ShowDisplay(MAIN2_1);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL2);
      }
      else
      {
        ShowDisplay(MAIN2_2);
      }
    }
    else if (SCREEN_STATE == MAIN2_SUB1_1) //////////////////////////////////////////////
    {
      // Asking the user if they want to save their input to a spool (yes option)
      if (result == KEY_PLAY)
      {
        ShowDisplay(MAIN2_SUB2);
      }
      else if (result == KEY_FORWARD)
      {
        ShowDisplay(MAIN2_SUB1_2);
      }
      else
      {
        ShowDisplay(MAIN2_SUB1_1);
      }
    }
    else if (SCREEN_STATE == MAIN2_SUB1_2) //////////////////////////////////////////////
    {
      // Asking the user if they want to save their input to a spool (no option)
      if (result == KEY_PLAY)
      {
        lcd.clear();
        calc_offset(temp_mass);
      }
      else if (result == KEY_BACK)
      {
        ShowDisplay(MAIN2_SUB1_1);
      }
      else
      {
        ShowDisplay(MAIN2_SUB1_2);
      }

    }
    else if (SCREEN_STATE == MAIN2_SUB2) //////////////////////////////////////////////
    {
      // When the user chose to save previous input to a spool
      lcd.clear();
      lcd.setCursor(4, 1);

      if (result == KEY_1)
      {
        mass1 = temp_mass;
        spool_mass = mass1;
        lcd.print("spool 1");
      }
      else if (result == KEY_2)
      {
        mass2 = temp_mass;
        spool_mass = mass2;
        lcd.print("spool 2");
      }
      else if (result == KEY_3)
      {
        mass3 = temp_mass;
        spool_mass = mass3;
        lcd.print("spool 3");
      }

      if (result == KEY_1 || result == KEY_2 || result == KEY_3)
      {
        lcd.setCursor(4, 0);
        lcd.print("Saved as");
        delay(3000);
        lcd.clear();
        calc_offset(spool_mass);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN2_SUB1_1);
      }
      else
      {
        ShowDisplay(MAIN2_SUB2);
      }
    }
    else if (SCREEN_STATE == MAIN2_SUB3) //////////////////////////////////////////////
    {
      // When the user wants to recall a past saved spool
      lcd.clear();
      if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN2_1);
      }
      else if (result == KEY_1)
      {
        if (mass1 == 1)
        {
          lcd.print("No mass saved");
          lcd.setCursor(0, 1);
          lcd.print("to this spool");
          delay(4000);
          ShowDisplay(MAIN2_SUB3);
        }
        else
        {
          spool_mass = mass1;
          lcd.print("Spool 1 saved");
          lcd.setCursor(0, 1);
          lcd.print("mass:");
          lcd.setCursor(6, 1);
          lcd.print(spool_mass);
          delay(4000);
          calc_offset(spool_mass);
        }
      }
      else if (result == KEY_2)
      {
        if (mass2 == 1)
        {
          lcd.print("No mass saved");
          lcd.setCursor(0, 1);
          lcd.print("to this spool");
          delay(4000);
          ShowDisplay(MAIN2_SUB3);
        }
        else
        {
          spool_mass = mass2;
          lcd.print("Spool 2 saved");
          lcd.setCursor(0, 1);
          lcd.print("mass:");
          lcd.setCursor(6, 1);
          lcd.print(spool_mass);
          delay(4000);
          calc_offset(spool_mass);
        }
      }
      else if (result == KEY_3)
      {
        if (mass3 == 1)
        {
          lcd.print("No mass saved");
          lcd.setCursor(0, 1);
          lcd.print("to this spool");
          delay(4000);
          ShowDisplay(MAIN2_SUB3);
        }
        else
        {
          spool_mass = mass3;
          lcd.print("Spool 3 saved");
          lcd.setCursor(0, 1);
          lcd.print("mass:");
          lcd.setCursor(6, 1);
          lcd.print(spool_mass);
          delay(4000);
          calc_offset(spool_mass);
        }
      }
      else
      {
        ShowDisplay(MAIN2_SUB3);
      }
    }
    else if (SCREEN_STATE == MAIN3) //////////////////////////////////////////////
    {
      if ((result == KEY_1 && project1[0][0] != 0) || (result == KEY_2 && project2[0][0] != 0) || (result == KEY_3 && project3[0][0] != 0))
      {
        lcd.clear();
        lcd.print("1 is most recent");
        delay(3000);
        lcd.clear();
        if (result == KEY_1)
        {
          ShowDisplay(SCROLL_PRINTS1_1);
        }
        else if (result == KEY_2)
        {
          ShowDisplay(SCROLL_PRINTS2_1);
        }
        else if (result == KEY_3)
        {
          ShowDisplay(SCROLL_PRINTS3_1);
        }
      }
      else if ((result == KEY_1 && project1[0][0] == 0) || (result == KEY_2 && project2[0][0] == 0) || (result == KEY_3 && project3[0][0] == 0))
      {
        lcd.setCursor(2, 0);
        lcd.print("No projects");
        lcd.setCursor(5, 1);
        lcd.print("saved");
        delay(3000);
        ShowDisplay(MAIN3);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(SCROLL_PROJECTS3);
      }
      else
      {
        ShowDisplay(MAIN3);
      }

    }
    else if (SCREEN_STATE == SCROLL_PRINTS1_1) //////////////////////////////////////////////
    {
      if ((result == KEY_FORWARD) && (project1[2][0] != 0))
      {
        ShowDisplay(SCROLL_PRINTS1_2);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN3);
      }
      else
      {
        ShowDisplay(SCROLL_PRINTS1_1);
      }
    }
    else if (SCREEN_STATE == SCROLL_PRINTS1_2) //////////////////////////////////////////////
    {
      if (result == KEY_BACK)
      {
        ShowDisplay(SCROLL_PRINTS1_1);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN3);
      }
      else
      {
        ShowDisplay(SCROLL_PRINTS1_2);
      }
    }
    else if (SCREEN_STATE == SCROLL_PRINTS2_1) //////////////////////////////////////////////
    {
      if ((result == KEY_FORWARD) && (project2[2][0] != 0))
      {
        ShowDisplay(SCROLL_PRINTS2_2);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN3);
      }
      else
      {
        ShowDisplay(SCROLL_PRINTS2_1);
      }
    }
    else if (SCREEN_STATE == SCROLL_PRINTS2_2) //////////////////////////////////////////////
    {
      if (result == KEY_BACK)
      {
        ShowDisplay(SCROLL_PRINTS2_1);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN3);
      }
      else
      {
        ShowDisplay(SCROLL_PRINTS2_2);
      }
    }
    else if (SCREEN_STATE == SCROLL_PRINTS3_1) //////////////////////////////////////////////
    {
      if ((result == KEY_FORWARD) && (project3[2][0] != 0))
      {
        ShowDisplay(SCROLL_PRINTS3_2);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN3);
      }
      else
      {
        ShowDisplay(SCROLL_PRINTS3_1);
      }
    }
    else if (SCREEN_STATE == SCROLL_PRINTS3_2) //////////////////////////////////////////////
    {
      if (result == KEY_BACK)
      {
        ShowDisplay(SCROLL_PRINTS3_1);
      }
      else if (result == KEY_RETURN)
      {
        ShowDisplay(MAIN3);
      }
      else
      {
        ShowDisplay(SCROLL_PRINTS3_2);
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ShowDisplay(screen val)
{
  //Store screen state.
  SCREEN_STATE = val;
  lcd.clear();
  if (val == SCROLL1) //////////////////////////////////////////////
  {
    lcd.print("> Main Display");
    lcd.setCursor(0, 1);
    lcd.print("  Set Spool");
    lcd.setCursor(15, 1);
    lcd.printByte(1);
  }
  else if (val == SCROLL2) //////////////////////////////////////////////
  {
    lcd.print("  Main Display");
    lcd.setCursor(0, 1);
    lcd.print("> Set Spool");
    lcd.setCursor(15, 1);
    lcd.printByte(1);
  }
  else if (val == SCROLL3) //////////////////////////////////////////////
  {
    lcd.print("> Projects");
    lcd.setCursor(0, 1);
    lcd.print("  Select Units");
    lcd.setCursor(15, 0);
    lcd.printByte(0);
  }
  else if (val == SCROLL4) //////////////////////////////////////////////
  {
    lcd.print("  Projects");
    lcd.setCursor(15, 0);
    lcd.printByte(0);
    lcd.setCursor(0, 1);
    lcd.print("> Select Units");
  }
  else if (val == SCROLL_PROJECTS1) //////////////////////////////////////////////
  {
    lcd.print("> Start Project");
    lcd.setCursor(0, 1);
    lcd.print("  End Project");
    lcd.setCursor(15, 1);
    lcd.printByte(1);
  }
  else if (val == SCROLL_PROJECTS2) //////////////////////////////////////////////
  {
    lcd.print("  Start Project");
    lcd.setCursor(0, 1);
    lcd.print("> End Project");
    lcd.setCursor(15, 1);
    lcd.printByte(1);
  }
  else if (val == SCROLL_PROJECTS3) //////////////////////////////////////////////
  {
    lcd.print("> Past Prints");
    lcd.setCursor(15, 0);
    lcd.printByte(0);
  }
  else if (val == SCROLL_UNITS1) //////////////////////////////////////////////
  {
    lcd.print("> Mass");
    lcd.setCursor(0, 1);
    lcd.print("  Volume"); //8
    lcd.setCursor(15, 1);
    lcd.printByte(1);
  }
  else if (val == SCROLL_UNITS2) //////////////////////////////////////////////
  {
    lcd.print("  Mass");
    lcd.setCursor(0, 1);
    lcd.print("> Volume"); //8
    lcd.setCursor(15, 1);
    lcd.printByte(1);
  }
  else if (val == SCROLL_UNITS3) //////////////////////////////////////////////
  {
    lcd.print("> Percentage");
    lcd.setCursor(15, 0);
    lcd.printByte(0);
  }
  else if (val == MAIN2_1) //////////////////////////////////////////////
  {
    lcd.setCursor(15, 0);
    lcd.printByte(2);
    lcd.home();
    lcd.print("> New Spool");
    lcd.setCursor(0, 1);
    lcd.print("  Saved Spool");
  }
  else if (val == MAIN2_2) //////////////////////////////////////////////
  {
    lcd.setCursor(15, 0);
    lcd.printByte(2);
    lcd.home();
    lcd.print("  New Spool");
    lcd.setCursor(0, 1);
    lcd.print("> Saved Spool");
  }
  else if (val == MAIN2_SUB1_1) //////////////////////////////////////////////
  {
    // After user inputs their spool mass, asks if want to save it to a spool #
    lcd.home();
    lcd.print("Save Spool?");
    lcd.setCursor(0, 1);
    lcd.print("> Yes");
    lcd.setCursor(9, 1);
    lcd.print("  No");
  }
  else if (val == MAIN2_SUB1_2) //////////////////////////////////////////////
  {
    // After user inputs their spool mass, asks if want to save it to a spool #
    lcd.home();
    lcd.print("Save Spool?");
    lcd.setCursor(0, 1);
    lcd.print("  Yes");
    lcd.setCursor(9, 1);
    lcd.print("> No");
  }
  else if (val == MAIN2_SUB2) //////////////////////////////////////////////
  {
    // When the user wants to save previous input to a spool
    spool_list("Pick Spool #");
    lcd.setCursor(15, 0);
    lcd.printByte(2);
  }
  else if (val == MAIN2_SUB3) //////////////////////////////////////////////
  {
    // When the user wants to recall a past saved spool
    spool_list("Pick Spool #");
    lcd.setCursor(15, 0);
    lcd.printByte(2);
  }
  else if (val == MAIN3) //////////////////////////////////////////////
  {
    spool_list("View Past Prints");
    lcd.setCursor(15, 1);
    lcd.printByte(2);
  }
  else if (val == SCROLL_PRINTS1_1) //////////////////////////////////////////////
  {
    lcd.print("1: ");
    lcd.print(project1[0][0]);
    lcd.print("g");
    lcd.setCursor(9, 0);
    lcd.print(project1[0][1]);
    lcd.print("min");
    if (project1[1][0] != 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("2: ");
      lcd.print(project1[1][0]);
      lcd.print("g");
      lcd.setCursor(9, 1);
      lcd.print(project1[1][1]);
      lcd.print("min");
      if (project1[2][0] != 0)
      {
        lcd.setCursor(15, 0);
        lcd.printByte(3);
        lcd.setCursor(15, 1);
        lcd.printByte(3);
      }
    }
  }
  else if (val == SCROLL_PRINTS1_2) //////////////////////////////////////////////
  {
    lcd.print("3: ");
    lcd.print(project1[2][0]);
    lcd.print("g");
    lcd.setCursor(9, 0);
    lcd.print(project1[2][1]);
    lcd.print("min");
    lcd.setCursor(15, 0);
    lcd.printByte(4);
    lcd.setCursor(15, 1);
    lcd.printByte(4);
    if (project1[3][0] != 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("4: ");
      lcd.print(project1[3][0]);
      lcd.print("g");
      lcd.setCursor(9, 1);
      lcd.print(project1[3][1]);
      lcd.print("min");
    }
  }
  else if (val == SCROLL_PRINTS2_1) //////////////////////////////////////////////
  {
    lcd.print("1: ");
    lcd.print(project2[0][0]);
    lcd.print("g");
    lcd.setCursor(9, 0);
    lcd.print(project2[0][1]);
    lcd.print("min");
    if (project2[1][0] != 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("2: ");
      lcd.print(project2[1][0]);
      lcd.print("g");
      lcd.setCursor(9, 1);
      lcd.print(project2[1][1]);
      lcd.print("min");
      if (project2[2][0] != 0)
      {
        lcd.setCursor(15, 0);
        lcd.printByte(3);
        lcd.setCursor(15, 1);
        lcd.printByte(3);
      }
    }
  }
  else if (val == SCROLL_PRINTS2_2) //////////////////////////////////////////////
  {
    lcd.print("3: ");
    lcd.print(project2[2][0]);
    lcd.print("g");
    lcd.setCursor(9, 0);
    lcd.print(project2[2][1]);
    lcd.print("min");
    lcd.setCursor(15, 0);
    lcd.printByte(4);
    lcd.setCursor(15, 1);
    lcd.printByte(4);
    if (project2[3][0] != 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("4: ");
      lcd.print(project2[3][0]);
      lcd.print("g");
      lcd.setCursor(9, 1);
      lcd.print(project2[3][1]);
      lcd.print("min");
    }
  }
  else if (val == SCROLL_PRINTS3_1) //////////////////////////////////////////////
  {
    lcd.print("1: ");
    lcd.print(project3[0][0]);
    lcd.print("g");
    lcd.setCursor(9, 0);
    lcd.print(project3[0][1]);
    lcd.print("min");
    if (project3[1][0] != 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("2: ");
      lcd.print(project3[1][0]);
      lcd.print("g");
      lcd.setCursor(9, 1);
      lcd.print(project3[1][1]);
      lcd.print("min");
      if (project3[2][0] != 0)
      {
        lcd.setCursor(15, 0);
        lcd.printByte(3);
        lcd.setCursor(15, 1);
        lcd.printByte(3);
      }
    }
  }
  else if (val == SCROLL_PRINTS3_2) //////////////////////////////////////////////
  {
    lcd.print("3: ");
    lcd.print(project3[2][0]);
    lcd.print("g");
    lcd.setCursor(9, 0);
    lcd.print(project3[2][1]);
    lcd.print("min");
    lcd.setCursor(15, 0);
    lcd.printByte(4);
    lcd.setCursor(15, 1);
    lcd.printByte(4);
    if (project3[3][0] != 0)
    {
      lcd.setCursor(0, 1);
      lcd.print("4: ");
      lcd.print(project3[3][0]);
      lcd.print("g");
      lcd.setCursor(9, 1);
      lcd.print(project3[3][1]);
      lcd.print("min");
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void action()
{
  lcd.clear();
  if (scr == "Mass" || scr == "Percentage" || scr == "Volume") /////////////////////////////////////
  {
    myIRObj.resume();
    lcd.setCursor(15, 0);
    lcd.printByte(2);
    if (scr == "Mass")
    {
      lcd.setCursor(6, 0);
    }
    else if (scr == "Percentage")
    {
      lcd.setCursor(3, 0);
    }
    else if (scr == "Volume")
    {
      lcd.setCursor(4, 0);
    }
    lcd.print(scr);
    val_mass;
    val_perc;
    val_volume;
    while (i < 1)
    {
      lcd.setCursor(3, 1);
      lcd.print("             ");
      val = scale.get_units();
      if (scr == "Mass")
      {
        val_mass = val;
        lcd.setCursor(4, 1);
        lcd.print(val_mass);
        lcd.print("  Grams");
      }
      else if (scr == "Percentage")
      {
        val_perc = 100 * (val / 1000);
        lcd.setCursor(6, 1);
        lcd.print(val_perc);
        lcd.print("  %");
      }
      else if (scr == "Volume")
      {
        val_volume = val / 1.24;
        lcd.setCursor(4, 1);
        lcd.print(val_volume);
        lcd.print("  cm^3");
      }
      delay(500);

      if (myIRObj.decode(&objResults))
      {
        if (objResults.value != 0XFFFFFFFF)
        {
          if (objResults.value == KEY_MENU || objResults.value == KEY_RETURN)
          {
            i++;
          }
          else if (objResults.value == KEY_POWER)
          {
            i++;
            ProcessKeyPress(KEY_POWER);
          }
          else if (objResults.value == KEY_C)
          {
            scale.tare(25);
            spool_mass = 1;
            myIRObj.resume();
          }
          else
          {
            i = 0;
            myIRObj.resume();
          }
        }
      }
    }
    i = 0;
    ShowDisplay(SCROLL1);
  }
  else if (scr == "New spool") /////////////////////////////////////
  {
    lcd.setCursor(3, 0);
    lcd.print("Place spool");
    lcd.setCursor(6, 1);
    lcd.print("now");
    delay(4000);
    lcd.clear();
    lcd.print("Enter mass (g)");
    lcd.setCursor(0, 1);
    lcd.print("----");
    lcd.blink();
    lcd.setCursor(0, 1);

    while (i <= 3)
    {
      if (myIRObj.decode(&objResults))
      {
        if (objResults.value != 0XFFFFFFFF)
        {
          if (objResults.value == KEY_MENU)
          {
            lcd.clear();
            lcd.noBlink();
            lcd.setCursor(6, 0);
            lcd.print("INPUT");
            lcd.setCursor(4, 1);
            lcd.print("CANCELLED");
            for (int u = 0; u < 4; u++)
            {
              tempmass[u] = 0;
            }
            i = 0;
            delay(4000);
            i = 10;
          }
          else if (objResults.value == KEY_RETURN || objResults.value == KEY_C)
          {
            if (i == 0)
            {
              i = 0;
            }
            else
            {
              i--;
              lcd.setCursor(i, 1);
              lcd.print("-");
              lcd.setCursor(i, 1);
              tempmass[i] = 0;
            }
          }
          else if (objResults.value == KEY_0 || objResults.value == KEY_1 || objResults.value == KEY_2 || objResults.value == KEY_3 || objResults.value == KEY_4 || objResults.value == KEY_5 || objResults.value == KEY_6 || objResults.value == KEY_7 || objResults.value == KEY_8 || objResults.value == KEY_9)
          {
            lcd.setCursor(i, 1);
            tempmass[i] = TranslateKey(objResults.value);
            lcd.print(tempmass[i]);
            i++;
            delay(250);
          }
        }
        myIRObj.resume();
      }
    }
    if (i == 10)
    {
      ShowDisplay(MAIN2_1);
    }
    else
    {
      lcd.noBlink();
      temp_mass = 1000 * tempmass[0] + 100 * tempmass[1] + 10 * tempmass[2] + tempmass[3];
      ShowDisplay(MAIN2_SUB1_1);
    }
    i = 0;
    scr = "Mass";
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  IRInitialize();

  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
  lcd.createChar(0, uparrow);
  lcd.createChar(1, downarrow);
  lcd.createChar(2, returnarrow);
  lcd.createChar(3, rightarrow);
  lcd.createChar(4, leftarrow);
  lcd.clear();

  scale.begin(6, 4, 128); // Init of library
  scale.set_scale(calib);
  scale.tare(30);

  Serial.begin(9600);

  scr = "Mass";
  ShowDisplay(SCROLL1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  // Keep track of the last key pressed
  unsigned long last_key_processed = 0;
  // Attempt to receive a IR code.
  //   Returns true if a code was received
  //   Returns false if nothing received yet
  // When a code is received, information is stored into "results".
  if (myIRObj.decode(&objResults))
  {
    if (objResults.value == 0XFFFFFFFF)
    {
      objResults.value = last_key_processed;
    }
    // Process key press to activate appropriate functionality.
    ProcessKeyPress(objResults.value);


    last_key_processed = objResults.value;
    // After receiving, this must be called to reset the receiver
    // and prepare it to receive another code.
    myIRObj.resume();
  }

  delay(500);

}
