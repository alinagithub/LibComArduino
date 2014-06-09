#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
# FILE: libcomarduino.py
# VERSION: 0.1.00
# PURPOSE: Easy communication Host <-> Arduino
# CREATED BY: Arnaud D. LINA
# DATE: June 2014
#/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\

import os
import serial
import time

DEFAULT_DEVICE_NAME         = "Arduino_Board"
DEFAULT_COM_PORT            = "/dev/tty.usbmodem1451"
DEFAULT_VERBOSE             = True
DEFAULT_TIMEOUT_ACKNOWLEDGE = 5   # seconds
DEFAULT_TIMEOUT_OPEN        = 120 # seconds
DEFAULT_TIMEOUT_READ        = 1   # seconds

#---------------------------------------------------------------------
class ComArduino:
   
   #---------------------------------------------------------------------
   def __init__(self, com_name= DEFAULT_DEVICE_NAME, com_port=DEFAULT_COM_PORT):
      
      self.com_port             = com_port
      self.com_name             = com_name
      self.com_serial           = None
      self.verbose              = DEFAULT_VERBOSE
      self.timeout_acknowledge  = DEFAULT_TIMEOUT_ACKNOWLEDGE
      self.timeout_read         = DEFAULT_TIMEOUT_READ
      self.timeout_open         = DEFAULT_TIMEOUT_OPEN
      self.com_unit_speed       = None
      self.connected            = False
   
   #---------------------------------------------------------------------
   def Open(self, com_baudrate=9600):

      # Open serial port
      if(self.verbose==True):
         print("Opening the port...")
         
      self.com_serial = serial.Serial(self.com_port)

      # Set serial port properties
      self.com_serial.baudrate = com_baudrate
      self.com_serial.bytesize = serial.EIGHTBITS
      self.com_serial.parity   = serial.PARITY_NONE
      self.com_serial.stopbits = serial.STOPBITS_ONE
      self.com_serial.timeout  = 2
      self.com_serial.xonxoff  = 0
      self.com_serial.rtscts   = 0

      # Pause
      time.sleep(2.0)

      if(self.com_serial.isOpen()==False):
         if(self.verbose==True):
            print(" -Open fail...")
         return False

      if(self.verbose==True):
         print(" -Open pass...")
      
      # Check device identity
      if(self.verbose==True):
         print("Checking device identity...")
         
      self.Identify=None
      initime=time.time()
      while((self.__GetWord(self.Identify, 0)!=self.com_name) and ((time.time()-initime)<self.timeout_open)):
         self.Identify = self.ReadData()
         if(self.verbose==True):
            print(" -[Read|Ref::"+self.__GetWord(self.Identify, 0)+"|"+self.com_name+"]")

      # Measure communication Rx speed, then Acknowledge
      if(self.__GetWord(self.Identify,0)==self.com_name):

         if(self.verbose==True):
            print(" -Connection opened...")
         
         self.connected = True
         self.__EvalComSpeed();
         time.sleep(2.0);
         self.WriteData(104);

         if(self.verbose==True):
            self.Print()
         
         return True         
         
      if(self.verbose==True):
         print(" -Connection fail...")

      return False
   
   #---------------------------------------------------------------------
   def WriteData(self, data, acknowledge=False):
      
      if(self.com_serial.isOpen()):
         self.com_serial.write(str(data))
         
         while True:
            try:
               time.sleep(0.01)
               break
            except:
               pass
            
         time.sleep(0.1)
      
      
      if(acknowledge==True):
         data = ''
         initime=time.time()
         while((data!='104') and ((time.time()-initime)<self.timeout_acknowledge)):
            data=self.__GetWord(str(self.ReadData()), 0)
         return (data=='104')

      ### MODIFICATION: IF ACKNOWLEDGEMENT, WE SHOULD TRY TO WRITE & FLUSH UNITL WE GET ANSWER
      ### OR SLEEP LONGER TO SEE
      
      return True
   
   #---------------------------------------------------------------------
   def ReadData(self, acknowledge=False):

      initime=time.time()
      data=''
      while((data=='') and ((time.time()-initime)<self.timeout_read)):
         try:
            data=str(self.com_serial.readline())
         except:
               pass
 
      if((data!='') and (acknowledge)):
         self.WriteData(104)
         
      if(self.verbose==True):
         print("Read: "+data)

      return data

   #---------------------------------------------------------------------
   def __EvalComSpeed(self):

      initime=time.time()

      for x in range(0, 10):
         self.WriteData(1)

      self.com_unit_speed = (time.time()-initime)/10.0
      self.com_serial.flush()

      if(self.verbose==True):
         print("ComSpeed: {0:.3f}msec".format(self.com_unit_speed*1000))

   #---------------------------------------------------------------------
   def Print(self):

      if(self.com_serial and self.com_serial.isOpen() and self.connected):
         print("==========================================================")
         print "SERIAL COM INFO"
         print " - Status   : opened and connected"
         print " - Name     : "  + self.__GetWord(self.Identify, 0)
         print " - Port     : "  + str(self.com_port)
         print " - Baudrate : "  + str(self.com_serial.baudrate)
         print " - Bytesize : "  + str(self.com_serial.bytesize)
         print " - Parity   : "  + str(self.com_serial.parity)
         print " - Stopbits : "  + str(self.com_serial.stopbits)
         print " - Timeout  : "  + str(self.com_serial.timeout)
         print " - Xonxoff  : "  + str(self.com_serial.xonxoff)
         print " - Rtscts   : "  + str(self.com_serial.rtscts)
         print(" - Tx speed : {0:.3f}msec".format(self.com_unit_speed*1000))
         print("==========================================================")
      else:
         print("==========================================================")
         print "SERIAL COM INFO"
         print " - Status: closed/not_connected"
         print("==========================================================")
         
#---------------------------------------------------------------------
   def __GetWord(self, string, index):
      if(string!=None):
         if(len(string)>0):
            return string.split()[index]
      return ''
