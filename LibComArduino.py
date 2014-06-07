#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
# FILE: libcomarduino.py
# VERSION: 0.1.00
# PURPOSE: Easy communication Host <-> Arduino
# CREATED BY: Arnaud D. LINA
# DATE: June 2014
#/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\


# FAIRE UN OPERATION D'IDENTIFICATION DU ARDUINO!!!!

import os
import serial
import time

DEFAULT_DEVICE_NAME         = "Arduino_Board"
DEFAULT_COM_PORT            = "/dev/tty.usbmodem1451"
DEFAULT_VERBOSE             = True
DEFAULT_TIMEOUT_ACKNOWLEDGE = 5  # seconds
DEFAULT_TIMEOUT_OPEN        = 30 # seconds
DEFAULT_TIMEOUT_READ        = 1  # seconds

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
   
   #---------------------------------------------------------------------
   def Open(self, com_baudrate=9600):
      
      self.com_serial = serial.Serial(self.com_port)
      
      self.com_serial.baudrate = com_baudrate
      self.com_serial.bytesize = serial.EIGHTBITS
      self.com_serial.parity   = serial.PARITY_NONE
      self.com_serial.stopbits = serial.STOPBITS_ONE
      self.com_serial.timeout  = 2
      self.com_serial.xonxoff  = 0
      self.com_serial.rtscts   = 0
      
      time.sleep(2.0)
      
      self.Identify=None
      initime=time.time()
      while((GetWord(self.Identify, 0)!=self.com_name) and ((time.time()-initime)<self.timeout_open)):
         self.Identify = self.GetLine(self.timeout_read)
         if(self.verbose==True):
            print("[Read|Ref::"+GetWord(self.Identify, 0)+"|"+self.com_name+"]")
   
      if(self.verbose==True):
         print("Open com_port to Arduino")
         print(self.com_serial)
         print("Identification: "+ self.com_name+" \ "+GetWord(self.Identify, 0))

      if(GetWord(self.Identify,0)==self.com_name):
         self.SendData(104);
         self.ComSpeed()
         return True
      
      return False
   
   #---------------------------------------------------------------------
   def SendData(self, data, acknowledge=False):
      
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
         data = 0
         initime=time.time()
         while((data!=104) and ((time.time()-initime)<self.timeout_acknowledge)):
            data=str(self.GetLine(self.timeout_read))
         
         return (data==104)
      
      return True
   
   #---------------------------------------------------------------------
   def ReceiveData(self):
      data=str(self.com_serial.readline())
      
      if(self.verbose==True):
         print("Receive: "+data)
   
   
   #---------------------------------------------------------------------
   def GetLine(self, timeout):
      
      line = None
      initime=time.time()
      
      while((line==None) and ((time.time()-initime)<timeout)):
         if(self.com_serial.isOpen()):
            try:
               line = self.com_serial.readline() # flush
            except:
               pass
      
      return line

   #---------------------------------------------------------------------
   def ComSpeed(self):

      initime=time.time()

      for x in range(0, 10):
         self.SendData(1)

      self.com_unit_speed = (time.time()-initime)/10.0

      if(self.verbose==True):
         print("ComSpeed: {0:.3f}msec".format(self.com_unit_speed*1000))


#---------------------------------------------------------------------
def GetWord(string, index):
   if(string!=None):
      if(len(string)>0):
         return string.split()[index]
   return ''
