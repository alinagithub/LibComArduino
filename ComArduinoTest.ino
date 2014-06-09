#include <LibShift595Arduino.h>
#include <LibComArduino.h>

using namespace libshift595arduino;
using namespace libcomarduino;

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// FILE: testlibcomarduino.ino
// VERSION: 0.1.00
// PURPOSE: Test for the LibComArduino.
// CREATED BY: Arnaud D. LINA
// DATE: June 2014
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


#define NumberOfRegisters 2
#define LatchPin          4
#define ClockPin          3
#define DataPin           2

Shift595Arduino MyShiftReg(DataPin, ClockPin, LatchPin, NumberOfRegisters);

/* Instantiate the communication object */
ComArduino ComObj;

int  DataIn=0;
int  AllDataIn[10];
int  Realsize;
int  AllDataOut[5]={2,5,3,7,8};
bool Connected = false;

void setup()
{
  MyShiftReg.Setup();
  
  ComObj.verbose=false;
  
  MyShiftReg.OffAllRegisters();
  MyShiftReg.OnSingleRegister(0, 1);
  
  MyShiftReg.OnSingleRegister(0, 0);
  Connected = ComObj.Open(true);
  MyShiftReg.OnSingleRegister(1, 0);
}  

void loop()
{
  MyShiftReg.OffAllRegisters();
  
  if(!Connected)
    {
    MyShiftReg.OffAllRegisters();
    MyShiftReg.OnSingleRegister(4, 1);
    MyShiftReg.OnSingleRegister(5, 1);
    MyShiftReg.OnSingleRegister(6, 1);
    MyShiftReg.OnSingleRegister(7, 1);
    Serial.println("No remote connection");
    return;
    }
  MyShiftReg.OnSingleRegister(4, 0);
  
  if(ComObj.IsDataIn())
    {
    MyShiftReg.OnSingleRegister(2, 0);
    ComObj.ReadInt(&DataIn, false);
    MyShiftReg.OnSingleRegister(6, 0);
    ComObj.WriteInt(104);
    MyShiftReg.OffSingleRegister(2, 0);
    MyShiftReg.OffSingleRegister(6, 0);
    }
   else
    {
    MyShiftReg.OnSingleRegister(0, 1);
    boolean OkSend = ComObj.WriteInt(999, true);
    if(OkSend)
       {
       MyShiftReg.OffSingleRegister(0, 1);
       }
     else
       {
       MyShiftReg.OnSingleRegister(4, 1);
       delay(1000);
       }
    MyShiftReg.OffSingleRegister(4, 1);
    MyShiftReg.OffSingleRegister(0, 1);
    
    }
  
  delay(20);
  
}
