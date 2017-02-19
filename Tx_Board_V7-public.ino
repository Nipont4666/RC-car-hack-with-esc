/*
 2.4G Transmitter nRF24L01 module Arduino
 Control Steering servo+ESC
 Written by: Pason Tanpaiboon
 Febuary 2016
 Version.1.7
 
 This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

uint8_t data[3] ; 
const uint8_t buffer_size = sizeof(data);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };//two pipes communication 

const int analogInPinX = A3;// 
const int analogInPinY= A4;
const int dumpsw = 4;//dumper switch pin 4

int Str_value = 0; 
int Esc_value = 0;       
int dumpswval = 0;

void setup(void)
{
  Serial.begin(115200);
  pinMode(analogInPinX,INPUT );
  pinMode(analogInPinY,INPUT );

  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.stopListening();

}

void loop(void)
{

  Str_value = analogRead(analogInPinX); 

  Str_value = Str_value/10;//scaling Str_value
  data[0] = Str_value;
  //////////////////////////////////////////////////////////////////////// 
  Esc_value = analogRead(analogInPinY); 

  data[1] = Esc_value/10;//scaling Esc_value
  /////////////////////////////////////////////////////////////////////////
  dumpswval = digitalRead(dumpsw);
  delay(15); 

  if (dumpswval == HIGH){

    data[2] = 1;//dump switch on,HIGH
  }
  else {
    data[2] = 0;//dump switch off,LOW
  }
  /////////////////////////////////////////////////////////////////////////
  radio.stopListening();

  bool ok = radio.write(  data ,sizeof(data) );
  delay(30);
  radio.startListening();
  delay(20);
  if (ok){
    Serial.print("data[0]=");
    Serial.print(data[0]);
    Serial.print(" data[1]=");
    Serial.print(data[1]);
    Serial.print(" data[2]=");
    Serial.println(data[2]);
  }
  else
    Serial.println("Failed");

}//void loop()
























