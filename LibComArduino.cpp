//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// FILE: libcomarduino.cpp
// VERSION: 0.1.00
// PURPOSE: Easy communication  Host <-> Arduino
// CREATED BY: Arnaud D. LINA
// DATE: June 2014
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

#include "libcomarduino.h"

#define DEFAULT_TIMEOUT_OPEN              10 // seconds
#define DEFAULT_TIMEOUT_OPEN_ACKNOWLEDGE  30 // seconds
#define DEFAULT_TIMEOUT_ACKNOWLEDGE        2 // seconds

/////////////////////////////////////////////////////
//
// PUBLIC
//

ComArduino::ComArduino()
   {
   verbose                    = false;
   timeout_open               = DEFAULT_TIMEOUT_OPEN;
   timeout_open_acknowledge   = DEFAULT_TIMEOUT_OPEN_ACKNOWLEDGE;
   timeout_acknowledge        = DEFAULT_TIMEOUT_ACKNOWLEDGE;
   funclevel                  = 0;
   }

boolean ComArduino::Open(bool Acknowledge)
   {
   FuncObj obj(&funclevel, &verbose);
   return funcOpen(Acknowledge);
   }

boolean ComArduino::IsConnected()
   {
   FuncObj obj(&funclevel, &verbose);
   return funcIsConnected();
   }

boolean ComArduino::IsDataIn()
   {
   FuncObj obj(&funclevel, &verbose);
   return funcIsDataIn();
   }


boolean ComArduino::ReadInt(int*    InPtr,
                            boolean Acknowledge)
   {
   FuncObj obj(&funclevel, &verbose);
   
   if(InPtr==NULL) {Serial.println("ERROR 0");};
   
   return funcReadInt(InPtr, Acknowledge);

   }

boolean ComArduino::ReadMultipleInt(int*    InPtr,
                                    int     Maxsize,
                                    int&    Realsize,
                                    boolean Acknowledge)
   {
   FuncObj obj(&funclevel, &verbose);
   
   if(InPtr==NULL)   {Serial.println("ERROR 1");};
   if(Maxsize<0)     {Serial.println("ERROR 2");};
   
   return funcReadMultipleInt(InPtr, Maxsize, Realsize, Acknowledge);
   }


boolean ComArduino::WriteInt(int     Out,
                             boolean Acknowledge)
   {
   FuncObj obj(&funclevel, &verbose);
   return funcWriteInt(Out, Acknowledge);
   }

boolean ComArduino::WriteMultipleInt(int*    OutPtr,
                                     int     Size,
                                     boolean Acknowledge)
{
   FuncObj obj(&funclevel, &verbose);
   
   if(OutPtr==NULL)  {Serial.println("ERROR 3");};
   if(Size<0)        {Serial.println("ERROR 4");};
   
   return funcWriteMultipleInt(OutPtr, Size, Acknowledge);
}

boolean ComArduino::WriteStr(String    Out,
                             boolean   Acknowledge)
{
   FuncObj obj(&funclevel, &verbose);
   return funcWriteStr(Out, Acknowledge);
}

/////////////////////////////////////////////////////
//
// PRIVATE
//

boolean ComArduino::funcOpen(bool Acknowledge)
   {
   Serial.begin(9600);
  
   int Inc=timeout_open;
   while(!Serial && Inc--)
      delay(1000);
  
   Dump("[[START:Open(");
   if(Serial)
      {
      Serial.flush();
      boolean connected = false;
      int Inc=timeout_open_acknowledge;
      while((Inc--)&&(!connected))
         {
         Serial.flush();
         connected = WriteStr(String(ARDUINO_ID), Acknowledge);
         delay(1000-DEFAULT_TIMEOUT_ACKNOWLEDGE);
         }
      Serial.flush();
      
      // !! here add read/write speed average.
      
      Dump("Done)");
      return connected;
      }
   else
      {
      Dump("Fail)");
      return false;
      }
   Dump(")");
   }

boolean ComArduino::funcIsConnected()
   {
   Dump("IsConnected(");
   
   if(Serial)
      {
      Dump("Yes)");
      return true;
      }
   else
      {
      Dump("No)");
      return false;
      }
   Dump(")");
   }

boolean ComArduino::funcIsDataIn()
   {
   Dump("IsDataIn(");
   
   if(!IsConnected()) {return false;}
   
   if(Serial.available())
      {
      Dump("Yes)");
      return true;
      }
   else
      {
      Dump("No)");
      return false;
      }
   Dump(")");
   }


boolean ComArduino::funcReadInt(int*    InPtr,
                                boolean Acknowledge)
   {
   Dump("ReadInt(");
   
   if(!IsConnected()) {Dump(")"); return false;}
   
   int Realsize=0;
   if(IsDataIn())
      {
      *InPtr = Serial.parseInt();
      Realsize+=1;
      delay(20);
      Dump("+");
      }
   
   WriteAcknowledge(Acknowledge, Realsize);
   Dump(")");
   return true;
   }

boolean ComArduino::funcReadMultipleInt(int*    InPtr,
                                        int     Maxsize,
                                        int&    Realsize,
                                        boolean Acknowledge)
   {
   Dump("ReadMultipleInt(");
   
   if(!IsConnected()) {Dump(")"); return false;}
   
   Realsize=0;
   while((Realsize<Maxsize) && Serial.available())
      {
      *(InPtr+Realsize) = Serial.parseInt();
      Realsize+=1;
      Dump("+");
      }
   
   WriteAcknowledge(Acknowledge, Realsize);
   Dump(")");
   return true;
   }


boolean ComArduino::funcWriteInt(int     Out,
                                 boolean Acknowledge)
   {
   Dump("WriteInt(");
   
   if(!IsConnected()) {Dump(")"); return false;}
   
   Serial.println(Out);
   delay(20);
   
   boolean check = ReadAcknowledge(Acknowledge);
   Dump(")");
   return check;
   }

boolean ComArduino::funcWriteMultipleInt(int*    OutPtr,
                                         int     Size,
                                         boolean Acknowledge)
{
   Dump("WriteMultipleInt(");
   
   if(!IsConnected()) {Dump(")"); return false;}
   
   String Output;
   for(int ii=0; ii<(Size-1);ii++)
      {
      Output+=*(OutPtr+ii);
      Output+=" ";
      }
   Output+=*(OutPtr+(Size-1));
   
   Serial.println(Output);
   delay(20);
   
   boolean check = ReadAcknowledge(Acknowledge);
   Dump(")");
   return check;
}

boolean ComArduino::funcWriteStr(String  Out,
                                 boolean Acknowledge)
{
   Dump("WriteStr(");
   
   if(!IsConnected()) {Dump(")"); return false;}
   
   Serial.println(Out);
   delay(20);
   
   boolean check = ReadAcknowledge(Acknowledge);
   Dump(")");
   return check;
}

/////////////////////////////////////////////////////
//
// PRIVATE UTIL
//
void ComArduino::Dump(char* text)
   {
   if(verbose)
      {
      Serial.print(text);
      delay(20);
      }
   }

void ComArduino::WriteAcknowledge(boolean doit,
                                  int     value)
   {
   Dump("[WriteAcknowledge(");
   if(!doit) {Dump(")]"); return;}
   
   String textout = String("104,");
   textout+=value;
   textout+="\n";
   Serial.print(textout);
   delay(20);
   Dump(")]");
   }

boolean ComArduino::ReadAcknowledge(boolean doit)
   {
   Dump("[ReadAcknowledge(");
   if(!doit) {Dump(")]"); return true;}
   
   int In=0;
   int Inc=timeout_acknowledge;
   while(Inc--)
      {
      In = Serial.parseInt();
      delay(20);
      
      if(In==104)
         {
         Dump("done)]");
         return true;
         }
      delay(1000);
      }
   Dump("timeout)]");
   return false;
   }


//
// END OF FILE
//
