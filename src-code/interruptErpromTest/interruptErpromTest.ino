/**
 * @ Author Dat Nguyen 
 * Date: 5/28/18 
 * This program test the working of EEPROM and attach interrupt signal effect. 
 */

#include <EEPROM.h>
#include <LedCube.h>

#define SIZE 3
#define COLS (SIZE*SIZE)


const byte interruptPin = 2;

byte levelPins[SIZE] = {11,12,13};
byte colPins[COLS] = {A0,3,4,5,6,7,8,9,10};

LedCube cube(SIZE, levelPins, colPins);


/*Prototype*/
void clearEEPROM();
void(*resetFunc) (void) = 0;
void changeOption();
void lightOff();

//global variable 


void setup(){
  Serial.begin(9600);
  pinMode(interruptPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin),changeOption,CHANGE);
  cube.lightPulse(0, 0, 10000);
}

void loop(){
  changeOption();
  Serial.println("in LOOP");
}

void changeOption(){
  digitalWrite(13,HIGH);
  delay(100);
  digitalWrite(13,LOW);
  int option = (int)EEPROM.read(1) + 1;
  EEPROM.write(1,option);
  Serial.print("EEPROM");Serial.println(EEPROM.read(1));
  Serial.print("option: ");Serial.println(option);

  
  if(option == 1){

      while(1){
        Serial.print("in option: ");Serial.println(1);
        cube.lightPlane(1, 1000);// light plane #1
      }
  }else if(option == 2){

      while(1){
        Serial.print("in option: ");Serial.println(2);
        cube.lightPlane(2, 1000);// light plane #1
      }
  }else if(option == 3){

      while(1){
        Serial.print("in option: ");Serial.println(3);
        cube.lightPlane(3, 1000);// light plane #1
      }
  }else if(option == 4 || option == 0){

    option = 1;
    EEPROM.write(1,option);
    Serial.print("in option: ");Serial.println(4);
    changeOption();
  }
}

void lightOff(){
  for(int i = 0;i < sizeof(colPins); i++){
      digitalWrite(i,LOW);
  }
}

