/*
 2.4G Receiver nRF24L01 module Arduino
 Control Steering servo+ESC
 Written by: Pason Tanpaiboon
 Febuary 2016
 Version.1.7
 
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <Servo.h> 
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };//2 pipes communication
uint8_t received_data[3];
uint8_t num_received_data =sizeof(received_data);

const int servo1 = 3;//  servo
const int esc1 = 6 ;// esc 
const int dumpsw  = 4;//dumper swich pin 4

Servo myservo;  // create servo object to control a servo
Servo myesc;  // create servo object to control a servo
int servoval;  // variable to read the value from the analog pin
int escval;  
int dumpswval=0;

void setup(void)
{
  Serial.begin(115200);
  delay(300); //wait until the esc starts in case of Arduino got power first
  myservo.attach(servo1);  // attaches the servo on pin 3 to the servo object 
  myesc.attach(esc1);  // attaches the ESC on pin 6 to the ese object  
  myservo.write(90);
  pinMode (dumpsw, OUTPUT);
  
  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();
 

}//void setup

void loop(void)
{
  
  ///////////////////////////////Radio reading///////////////////////////////////////////////////////     
  if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
    done = radio.read(&received_data, num_received_data  );
    delay(30);
    }// while (!done)
   
    Serial.print(received_data[0]);
    Serial.print ("---"); 
    Serial.print(received_data[1]);
    Serial.print ("---"); 
    Serial.print(received_data[2]);

    radio.stopListening();

    radio.startListening();
   
 /****************************************steering servo*********************************************************************/
    servoval = received_data[0];
    servoval = servoval*10;//scale back
    servoval = map(servoval, 150 , 910, 180, 0);//steering servo
   
    if (servoval >= 88 && servoval <= 97 )//stabilize steering servo
    {
      servoval = 94; //neutral straight position can be adjusted in each car 
      myservo.write(servoval);
      delay(30);
    }
      
    myservo.write(servoval); // sets the servo position according to the scaled value
    delay(30);
    
/******************************************esc servo***************************************************************/  
     escval = received_data[1];
     escval = escval*10 ; //scale back
     escval = map(escval, 170 , 900 ,0, 180); //esc triggger
  
   if (escval >= 89 && escval <= 98 ) //stabilize ESC neutral position
{
     escval = 92; //each esc have different values
     myesc.write(escval);   
     delay(30);
}
     myesc.write(escval);   
     delay(30);
     
    Serial.print (">>>"); 
    Serial.print(servoval);
    Serial.print ("---"); 
    Serial.print(escval);
    Serial.print ("---"); 
    Serial.println(dumpswval);
 /***********************************************Dump switch****************************************************************************/
    dumpswval = received_data[2];
    if ( dumpswval == 1 ){ // >HIGH dump on
      digitalWrite(dumpsw,HIGH);
      delay(15);   
    }
    else  {
      digitalWrite(dumpsw,LOW);
      delay(15);   
    }
  }// if ( radio.available() )
}//void loop(void)

