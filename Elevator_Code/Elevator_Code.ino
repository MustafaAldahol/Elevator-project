#include <Servo.h>
#define floorSens0  A2
#define floorSens1  A1
#define floorSens2  A0
#define button0     A5
#define button1     A4
#define button2     A3
#define seven_segA  13
#define seven_segB  12
#define seven_segC  11
#define seven_segD  10
#define motorIN1    7
#define motorIN2    6
#define motorEN     5
#define hum_sensor0 0
#define hum_sensor1 8
#define hum_sensor2 9

Servo open_door_servo0;
Servo open_door_servo1;
Servo open_door_servo2;

byte current_floor = 4 ,pre_requsted_floor = 4 ,i = 0; // the value of "i" must be rather than the max floor number
byte requsted_floor[20]={4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}; // 20 is the max storage of requsted floor, and 4 is the init value "must be rather than the max floor number"
bool door_opened = 0;

//-----functions-----//
void read_button();   // reads buttons which entered by users ex: 0,1,2
void go_floor();      // controls elevator movment between floors
void open_door();     // controls floor's door whether be opened or closed
void seven_segment(); // display floor's number on seven segment

void setup() {
  
  pinMode(seven_segA ,OUTPUT);
  pinMode(seven_segB ,OUTPUT);
  pinMode(seven_segC ,OUTPUT);
  pinMode(seven_segD ,OUTPUT);
  pinMode(motorIN1 ,OUTPUT);
  pinMode(motorIN2 ,OUTPUT);
  pinMode(motorEN ,OUTPUT);
  pinMode(floorSens0 ,INPUT);
  pinMode(floorSens1 ,INPUT);
  pinMode(floorSens2 ,INPUT);
  pinMode(button0 ,INPUT);
  pinMode(button1 ,INPUT);
  pinMode(button2 ,INPUT);
  
  open_door_servo0.attach(3);
  open_door_servo1.attach(4);
  open_door_servo2.attach(5);

  
  //make sure all door are closed
  open_door_servo0.write(0);
  open_door_servo1.write(0);
  open_door_servo2.write(0);
  door_opened = 1; // door closed
  Serial.begin(57600);
}

void loop() {
  
  read_button();
  go_floor();
  seven_segment();
  
}


void read_button(){
  /* read requsted floor's from users, 
   * and make sure that value isn't stored twice 
  */ 
  
  if(digitalRead(button0)){
    if (pre_requsted_floor != 0){ //if previos button doesn't eqaul current button, then take this button's value
      requsted_floor[i] = 0;
      pre_requsted_floor = 0;
      i++; // initial the next time to store requsted floor
    }
  }
  else if(digitalRead(button1)){ 
    if (pre_requsted_floor != 1){ //if previos button doesn't eqaul current button, then take this button's value  
      requsted_floor[i] = 1;
      pre_requsted_floor = 1;
      i++; // initial the next time to store requsted floor
    }
  }
  else if(digitalRead(button2)){ 
     if (pre_requsted_floor != 2){ //if previos button doesn't eqaul current button, then take this button's value  
      requsted_floor[i] = 2;
      pre_requsted_floor = 2;
      i++; // initial the next time to store requsted floor
    }    
  }
  
}

//-----go to floor function-----//
void go_floor(){
/*  detects the current floor and compare between current and requsted floor 
    and decides to go up or down
*/

  if ( ( !digitalRead(floorSens0) && !digitalRead(floorSens1) && !digitalRead(floorSens2) ) && current_floor == 4 ){//  problem detector
      digitalWrite(motorIN1 ,HIGH);
      digitalWrite(motorIN2 ,LOW);
      Serial.print("There is a problem go DOWN"); // the elevator is between to floors
  }
  else{ // normal operation
    //  detect current floor
    if(digitalRead(floorSens0)){ current_floor = 0;}
    else if(digitalRead(floorSens1)){ current_floor = 1;}
    else if(digitalRead(floorSens2)){ current_floor = 2;}
  
    Serial.print(current_floor);  Serial.print("        ");
    for(int j = 0; j < 19; j++){
      Serial.print(requsted_floor[j]); 
    } 

    if(requsted_floor[0] != 4){ // if there is one value at least in requsted floor array
      if(requsted_floor[0] == current_floor){
        digitalWrite(motorIN1 ,LOW);
        digitalWrite(motorIN2 ,LOW);
        open_door();
        // opration is completed, so delete the first value in requsted array "in anthor word, shift array to left one element"
        i--;  
        for(int j = 0; j < 19; j++){ //shift requsted_floor vector one element to left
          requsted_floor[j] = requsted_floor[j+1];
        }
         
   
      }
      else if( (requsted_floor[0] > current_floor && door_opened == 0)  ){
        digitalWrite(motorIN1 ,HIGH);
        digitalWrite(motorIN2 ,LOW);
        Serial.print("  UP");
      }
      else if( (requsted_floor[0] < current_floor && door_opened == 0) ){
        digitalWrite(motorIN1 ,LOW);
        digitalWrite(motorIN2 ,HIGH);
        Serial.print("  DOWN");
      }
    }
  }
  Serial.println("        ");  
  
}


//-----add featuare that opens floor door when elevator get it's floor gool-----//
void open_door(){
/*  open floor's door when elevator arrived to the requsted floor
 */
 
  if(current_floor == requsted_floor[0]){
    door_opened = 1; // raise the door openning flag
    
    if(requsted_floor[0] == 0){
      open_door_servo0.write(90);
      long Time = millis();
      while((millis() - Time) < 500){
        if(digitalRead(hum_sensor0)){ //make sure if you use IRsensor module you have to put ! befor digitalRead 
          Time = millis();            //due to sensor specification..
          }
        read_button(); // make sure to read new request in the openning door time  
      }
      Time = millis();
      while((millis() - Time) < 400){ //wiat for few second befor continue
        
        read_button();  // make sure to read new request in the  closing door time
        if(digitalRead(hum_sensor0)){ // reopennig door if someone try to enter elevator
          open_door_servo0.write(90);
          Time = millis();
        }
        else {
          open_door_servo0.write(0);
          door_opened = 0;
        }
      } 

      
    }
  
    else if(requsted_floor[0] == 1){
        open_door_servo1.write(90);
        long Time = millis();
        while((millis() - Time) < 500){
          if(digitalRead(hum_sensor1)){ //make sure if you use IRsensor module you have to put ! befor digitalRead 
            Time = millis();            //due to sensor specification..
          }
            read_button();
          
        }
        Time = millis();
        while((millis() - Time) < 400){//wiat for 1 second befor continue
          read_button();
          if(digitalRead(hum_sensor1)){
            open_door_servo1.write(90);
            Time = millis();
            
          }
          else {
            open_door_servo1.write(0);
            door_opened = 0;
          }
        }

    }
    
    else if(requsted_floor[0] == 2){
        open_door_servo2.write(90);
        long Time = millis();
        while((millis() - Time) < 500){
          if(digitalRead(hum_sensor2)){ //make sure if you use IRsensor module you have to put ! befor digitalRead 
            Time = millis();           //due to sensor specification..
          }
          read_button();
        }
        Time = millis();
        while((millis() - Time) < 400){//wiat for 1 second befor continue
          read_button();
          if(digitalRead(hum_sensor2)){
            open_door_servo2.write(90);
            Time = millis();

          }
          else {
            open_door_servo2.write(0);
            door_opened = 0;
          }          
      }

       
    }
  }
}

void seven_segment(){
// detect current floor
  if(digitalRead(floorSens0)){ current_floor = 0;}
  else if(digitalRead(floorSens1)){ current_floor = 1;}
  else if(digitalRead(floorSens2)){ current_floor = 2;}

  if(current_floor == 0){
    PORTB &=0b11000011;
  }
  else if(current_floor == 1){
    PORTB &=0b11000011;
    PORTB |=0b11100011;
  }
  else if(current_floor == 2){
    PORTB &=0b11000011;
    PORTB |=0b11010011;
  }
  else{ // problem state
    PORTB &=0b11000011;
    PORTB |=0b11011111;
  }
}

