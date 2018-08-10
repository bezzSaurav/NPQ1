#include <SPI.h>
#include <RH_RF24.h>

RH_RF24 Trans_Obj; //Object for RFM 

#define GPIO0 7 //GPIO0 of RFM Transceiver controlled by D7 pin.
#define SDN_Pin 3
#define Bit_Duration 10000 
//50 Baud = 1/50 seconds= 22222 microS 
//833 for 1200Baud
//10000 for 100
boolean Transmit=HIGH;


RH_RF24::ModemConfig FSKFd430 = {
0x8A, 0x00, 0x13, 0x88, 0x01, 0x00, 0x00, 0x0f, 0x01, 0x34,
0x11, 0x02, 0x71, 0x00, 0xd1, 0xb7, 0x00, 0x69, 0x02, 0x36,
0x80, 0x01, 0x5a, 0xfc, 0xe2, 0x11, 0x89, 0x89, 0x00, 0x02,
0xff, 0xff, 0x00, 0x2b, 0x02, 0x81, 0x00, 0xad, 0x3a, 0xff,
0xba, 0x0f, 0x51, 0xcf, 0xa9, 0xc9, 0xfc, 0x1b, 0x1e, 0x0f,
0x01, 0xfc, 0xfd, 0x15, 0xff, 0x00, 0x0f, 0xff, 0xba, 0x0f,
0x51, 0xcf, 0xa9, 0xc9, 0xfc, 0x1b, 0x1e, 0x0f, 0x01, 0xfc,
0xfd, 0x15, 0xff, 0x00, 0x0f, 0x3f, 0x2c, 0x0e, 0x04, 0x0c,
0x73
};




void setup() 
{
  pinMode(6, INPUT); //There was mistake with 2nd Version of PCB
  pinMode(GPIO0, OUTPUT);
  pinMode(SDN_Pin,OUTPUT);
  digitalWrite(SDN_Pin, LOW);
  Serial.begin(9600);
  if(!Trans_Obj.init())
  {
     //If Failed to Intialize, Reboot using SDN Pin
    Serial.println("Failed to Intialize, Trying to reboot SDN Pin");
    digitalWrite(SDN_Pin, HIGH);
    delay(200);
    digitalWrite(SDN_Pin, LOW);
    Trans_Obj.init();
  }
 Trans_Obj.setModemRegisters(&FSKFd430);

 Trans_Obj.setTxPower(0x7f);
 Trans_Obj.setModeTx();
 Serial.println(Trans_Obj.printRegisters()); 
 //Printing the values of Registers, Useful for Debugging Process
}

void loop() {
  //Transmit only in Transmit mode
  if(Transmit)
  {
    //Send_RTTY_ASCII8("$$$$NPQ1,23,25,30,00,90,0,22,30,23,12,56,233,23,FP,");
   // Send_RTTY_ASCII8("$$$$NPQ1,23,25,30,00,90,0,22,30,23,12,56,233,23,FP,6AP");
    Send_RTTY_ASCII8("NE");
  }
}


void Send_RTTY_ASCII8(char *Input_Char)
{
  char Temp_Char;
  Temp_Char = *Input_Char++;
  //Repeat until the Input Character array is empty
  while(Temp_Char != '\0')
  {
    int Bit_Counter;
    //Define Start Bit with 0
    Send_Bits_As_Frequency(0);
    for(Bit_Counter=0; Bit_Counter<8; Bit_Counter++)
    {
      //Take First bit from the character
      if(Temp_Char & 1)
        Send_Bits_As_Frequency(1);
      else
        Send_Bits_As_Frequency(0);
        Temp_Char = Temp_Char>>1; //Shift 1 bit
    }
    //Send two Stop Bits
    Send_Bits_As_Frequency(1);
    Send_Bits_As_Frequency(1);
    Temp_Char = *Input_Char++;
  }
}

void Send_Bits_As_Frequency(int Input_Bit)
{
  if(Input_Bit)
  {
    digitalWrite(GPIO0,HIGH); //Send CW of f1 Frequency
    Trans_Obj.setModeTx();
  }
  else
  {
    digitalWrite(GPIO0,LOW); //Send CW of f2 Frequency
    Trans_Obj.setModeTx();
  }
  delayMicroseconds(Bit_Duration);
  //Transmit till the bit duration
}



