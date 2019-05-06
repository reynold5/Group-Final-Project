// if using 'restored' with 0 and 1s doesn't work, possibly use timers instead

// set actuator level 1.5LED below beambroken LED.
// going up: 3 broke, move up from 1.5 to 2.5
// going down: 3 broke, move down from 2.5 to 1.5

// IR transmitter and receiver setup
#include <Servo.h> 

//Defines
#define LINEARACTUATORPIN 9        //Linear Actuator Digital Pin
#define LINEARACTUATORPIN1 10

const int button2Pin = 2;     // the number of the pushbutton pin
const int emergencyButtonPin = 0; //pin assignment
const int buzzer = A0;

int button1State = 0;         // variable for reading the pushbutton status
int button2State = 0;         // variable for reading the pushbutton status
boolean emergencyButton = 0;         // variable for reading the pushbutton status

Servo LINEARACTUATOR;  // create servo objects to control the linear actuator
Servo LINEARACTUATOR1;

static int linearValue = 1200; //Attuator starts from middle position

  const byte IRLED = 11;  // Timer 2 "A" output: OC2A
  
  const byte IR1 = 8; // from receiver output, lowest level
  const byte IR2 = 7; // from receiver output
  const byte IR3 = 6; // from receiver output
  const byte IR4 = 5; // from receiver output, highest level
  
  const byte LED1 = 13; // indicator LED
  const byte LED2 = 12; // indicator LED
  const byte LED3 = 4; // indicator LED
  const byte LED4 = 3; // indicator LED
  
  boolean Beam1;
  boolean Beam2;
  boolean Beam3;
  boolean Beam4;


  //global values
  static uint8_t level = 4;
  static uint8_t prevlevel = 4;
  
  boolean lockout = 0;

// Preset height levels: need to know how actuator height is set
  // Going up:     min    1      2     3     4    max
  // Going down:     1    2      3     4          max
  int preset[6] = {1100, 1150, 1350, 1550, 1750, 1800};

void setup() {
    Serial.begin(9600);

  
    pinMode (LED4, OUTPUT);
    pinMode (LED3, OUTPUT);
    pinMode (LED2, OUTPUT);
    pinMode (LED1, INPUT);
    pinMode (IR4, INPUT);
    pinMode (IR3, INPUT);
    pinMode (IR2, INPUT);
    pinMode (IR1, INPUT);

  // 38kHz LED setup
    pinMode (IRLED, OUTPUT);
    // set 38kHz square wave on Timer 2, from http://forum.arduino.cc/index.php/topic,102430.0.html
    TCCR2A = _BV (COM2A0) | _BV(WGM21);  // CTC, toggle OC2A on Compare Match
    TCCR2B = _BV (CS20);   // No prescaler
    OCR2A =  209;          // compare A register value (210 * clock speed (1/16MHz))
    //  = 13.125 uS , so frequency is 1 / (2 * 13.125) = 38095

    delay(2000);

  LINEARACTUATOR.attach(LINEARACTUATORPIN, 1050, 2000);      // attaches/activates the linear actuator as a servo object 
  LINEARACTUATOR1.attach(LINEARACTUATORPIN1, 1050, 2000);
   
  pinMode(button2Pin, INPUT);
  pinMode(emergencyButtonPin, INPUT);
  pinMode(buzzer, OUTPUT); 
  LINEARACTUATOR.writeMicroseconds(linearValue); 
  LINEARACTUATOR1.writeMicroseconds(linearValue);
}

void SetLevel(uint8_t &level, uint8_t &prevlevel, boolean &updown, uint8_t restored[5]) {
  if (lockout != 1) {
    if (level > prevlevel) {
      updown = 1; // move actuator up
    }
    else if (level < prevlevel) {
      updown = 0; // move actuator down
    }
  //  else { // level == prevlevel
  //    if (updown == 1) {
  //      updown = 0;
  //    }
  //    else {
  //      updown = 1;
  //    }
  //  }
  
    // in place of else above
    else if (level == 4 && prevlevel == 4) { // if at top level allow triggering of same LED
      if (updown == 1) {
        updown = 0;
      }
//      else {
//        updown = 1;
//      }
    } 
    else if (level == 1 && prevlevel == 1) { // if at bot level allowing triggering of same LED if
//      if (updown == 1) { // already triggered second time and set to move up, do not allow to move down again
//        Emergency();
//      }
//      else { // initial trigger while going down, second trigger means going up
        updown = 1;
//      }
    }
  //  else if (updown == 1) { // if not at top LED and trigger same LED, assume rep fail in progress
  //    level = 4; // raise the actuator to top
  //  }
  }

  if (lockout != 1) {
    MoveActuator(level, updown);
    prevlevel = level;
  
    // Reset all beam states except one currently blocked
    restored[1] = 0;
    restored[2] = 0;
    restored[3] = 0;
    restored[4] = 0;
    //restored[level] = 1;
  }
}

void MoveActuator(uint8_t level, boolean updown) {
  
  //int linearValue = 0
  if (updown == 1) { // move actuator up
    // Set height to preset value corresponding to the level;
    linearValue = preset[level];
  }
  else {
    // Set height to preset value corresponding to the level;
    linearValue = preset[level-1];
  }
  Serial.print("linearValue func: ");
  Serial.println(linearValue);

}

void Emergency() {
  linearValue = 1800;
  Serial.println("Emergency Lockout");
  lockout = 1;
  delay(2000);
}

void loop() {
  static int buzzerState = 0;
  emergencyButton = digitalRead(emergencyButtonPin);
  if (emergencyButton == LOW) {
    linearValue = 1800;
    lockout = 1;
    Serial.println("Emergency Putton Pressed: Alarm Triggered");
    buzzerState = 1;
  }
  


  button2State = digitalRead(button2Pin);
  if (button2State == HIGH) {     
    // set the position value   
    linearValue = 1800;  //fully extended
    lockout = 1;
    Serial.println("Button 2");
  } 

  static int prevCount = 0;
  if (lockout == 1 && buzzerState >= 1) {
    if (buzzerState >= 6){
      buzzerState = 0;
      noTone(buzzer);
    }
    if (millis() - prevCount >= 2000) {
      if (buzzerState % 2 == 0) {
        tone(buzzer, 1000);
      }
      else {
        tone(buzzer, 500);
      }
      buzzerState = buzzerState + 1;
      prevCount = millis();
    }
  }
 
  //use the writeMicroseconds to set the actuator to the new position
  //LINEARACTUATOR.writeMicroseconds(linearValue); 
  //LINEARACTUATOR1.writeMicroseconds(linearValue); 

 
  
     
  static boolean updown = 1; // 1=up, 0=down
  static uint8_t restored[5] = {1,1,1,1,0}; // Only move actuator when beam initially broken
  static unsigned long prevtime;
  
  Beam4 = digitalRead (IR4);
  if (Beam4 == HIGH) { // beam interrupted
    digitalWrite(LED4, LOW); // red LED off
    if (restored[4] == 0) {
      level = 4;
      restored[4] = 1;
      SetLevel(level, prevlevel, updown, restored);
      prevtime = millis();
    }
  }
  else { // beam detected
    digitalWrite(LED4, HIGH); // red LED on
    if (millis() - prevtime >= 250) { // "debounce" for repeat readings of 4th beam
      restored[4] = 0;
    }
  }

  Beam3 = digitalRead (IR3);
  if (digitalRead(IR3) == HIGH) { // beam interrupted
    digitalWrite(LED3, LOW); // green LED off
    if (restored[3] == 0) {
      level = 3;
      restored[3] = 1;
      SetLevel(level, prevlevel, updown, restored);
      prevtime = millis();
    }
  }
  else { // beam detected
    digitalWrite(LED3, HIGH); // green LED on
  }

  Beam2 = digitalRead (IR2);
  if (Beam2 == HIGH) { // beam interrupted
    digitalWrite(LED2, LOW); // yellow LED off
    if (restored[2] == 0) {
      level = 2;
      restored[2] = 1;
      SetLevel(level, prevlevel, updown, restored);
      prevtime = millis();
    }
  }
  else { // beam detected
    digitalWrite(LED2, HIGH); // yellow LED on
  }

  Beam1 = digitalRead (IR1);
  if (Beam1 == HIGH) { // beam interrupted
    digitalWrite(LED1, LOW); // red LED off
    if (restored[1] == 0) {
      level = 1;
      restored[1] = 1;
      SetLevel(level, prevlevel, updown, restored);
      prevtime = millis();
    }
  }
  else { // beam detected
    digitalWrite(LED1, HIGH); // red LED on
    if (millis() - prevtime >= 250) { // "debounce" for repeat readings of 1st beam
      restored[1] = 0;
    }
  }

  LINEARACTUATOR.writeMicroseconds(linearValue); 
  LINEARACTUATOR1.writeMicroseconds(linearValue);
//  Serial.print("linearValue: ");
//  Serial.println(linearValue);
}
