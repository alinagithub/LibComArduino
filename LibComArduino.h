//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// FILE: libcomarduino.h
// VERSION: 0.1.00
// PURPOSE: Easy communication  Host <-> Arduino
// CREATED BY: Arnaud D. LINA
// DATE: June 2014
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

#ifndef libcomarduino_h
#define libcomarduino_h

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define LIBCOMARDUINO_LIB_VERSION "0.1.00"

#define ARDUINO_ID "Arduino_Board"

namespace libcomarduino
{

class ComArduino
{
public:
   ComArduino();
   
   boolean Open(bool Acknowledge=false);
   
   boolean IsConnected();
   boolean IsDataIn();
   
   boolean ReadInt(int* In, boolean Acknowledge=false);
   boolean ReadMultipleInt(int* In, int Maxsize, int& Realsize, boolean Acknowledge=false);
   
   boolean WriteInt(int Out, boolean Acknowledge=false);
   boolean WriteMultipleInt(int* OutPtr, int Size, boolean Acknowledge=false);
   boolean WriteStr(String Out, boolean Acknowledge=false);
   
   boolean verbose;
   int     timeout_open;
   int     timeout_acknowledge;
   int     timeout_open_acknowledge;
   
private:
   boolean funcOpen(bool Acknowledge);
   boolean funcIsConnected();
   boolean funcIsDataIn();
   boolean funcReadInt(int* In, boolean Acknowledge);
   boolean funcReadMultipleInt(int* In, int Maxsize, int& Realsize, boolean Acknowledge);
   boolean funcWriteInt(int Out, boolean Acknowledge);
   boolean funcWriteMultipleInt(int* OutPtr, int Size, boolean Acknowledge);
   boolean funcWriteStr(String Out, boolean Acknowledge);
   
private:
   boolean ReadAcknowledge(boolean doit, int timeout);
   void    WriteAcknowledge(boolean doit, int value);
   void    Dump(char* text);
   
private:
   int funclevel;
};



class FuncObj
{
public:
   FuncObj(int* levelptr, boolean* verboseptr)
   {
   funcverboseptr = verboseptr;
   funclevelptr   = levelptr;
   
   (*funclevelptr)+=1;
   
   if(*funcverboseptr)
      {
      if((*funclevelptr)==1)
         Serial.print("[[START:");
      else
         Serial.print("[");
      }
   }
   
   ~FuncObj()
   {
   (*funclevelptr)-=1;
   
   if(*funcverboseptr)
      {
      if((*funclevelptr)==0)
         Serial.println(":END]]");
      else
         Serial.print("]");
      }
   }
   
   int*     funclevelptr;
   boolean* funcverboseptr;
};

} // end namespace
#endif

using namespace libcomarduino;
//
// END OF FILE
//
