
#include <LibShift595Arduino.h>
#include <LibComArduino.h>

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
  MyShiftReg.OnLowerHalfOnlyRegister(1);
  Connected = ComObj.Open(true);
  MyShiftReg.OffAllRegisters();
  MyShiftReg.OnUpperHalfOnlyRegister(0);
  delay(3000);
}  

void loop()
{
  MyShiftReg.OffAllRegisters();
  delay(100);
  MyShiftReg.OnSingleRegister(0, 0);
  delay(50);
  
  if(!Connected)
    {
    MyShiftReg.OffAllRegisters();
    MyShiftReg.OnUpperHalfOnlyRegister(1);
    Serial.println("No remote connection");
    return;
    }
  
  if(ComObj.IsDataIn())
    {
    MyShiftReg.OnSingleRegister(1, 0);
    ComObj.ReadInt(&DataIn, false);
    ComObj.WriteInt(104);
    MyShiftReg.OnLowerHalfRegister(1);
    MyShiftReg.BlinkRegister(DataIn, 250, 1);
    MyShiftReg.OffLowerHalfRegister(1);
    MyShiftReg.OffSingleRegister(1, 0);
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
       delay(100);
       }
    MyShiftReg.OffSingleRegister(4, 1);
    MyShiftReg.OffSingleRegister(0, 1);
    
    }
  
  delay(20);
  
}
