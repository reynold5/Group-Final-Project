
// IR transmitter and receiver setup
  const byte IRLED = 11;  // Timer 2 "A" output: OC2A
  
  const byte IR1 = 8; // from receiver output
  const byte IR2 = 7; // from receiver output
  const byte IR3 = 6; // from receiver output
  const byte IR4 = 5; // from receiver output
  
  const byte LED1 = 13; // red indicator LED
  const byte LED2 = 12; // green indicator LED
  const byte LED3 = 10; // yellow indicator LED
  const byte LED4 = 9; // red indicator LED
  
  boolean Beam1;
  boolean Beam2;
  boolean Beam3;
  boolean Beam4;

void setup() {
    Serial.begin(9600);
  
    pinMode (LED1, OUTPUT);
    pinMode (LED2, OUTPUT);
    pinMode (LED3, OUTPUT);
    pinMode (LED4, OUTPUT);
    pinMode (IR1, INPUT);
    pinMode (IR2, INPUT);
    pinMode (IR3, INPUT);
    pinMode (IR4, INPUT);

  // 38kHz LED setup
    pinMode (IRLED, OUTPUT);
    // set 38kHz square wave on Timer 2, from http://forum.arduino.cc/index.php/topic,102430.0.html
    TCCR2A = _BV (COM2A0) | _BV(WGM21);  // CTC, toggle OC2A on Compare Match
    TCCR2B = _BV (CS20);   // No prescaler
    OCR2A =  209;          // compare A register value (210 * clock speed (1/16MHz))
    //  = 13.125 uS , so frequency is 1 / (2 * 13.125) = 38095
}

void loop() {
  Beam1 = digitalRead (IR1);
  if (Beam1 == HIGH) { // beam interrupted
    digitalWrite(LED1, LOW); // red LED off
  }
  else { // beam detected
    digitalWrite(LED1, HIGH); // red LED on
  }

  Beam2 = digitalRead (IR2);
  if (Beam2 == HIGH) { // beam interrupted
    digitalWrite(LED2, LOW); // yellow LED off
  }
  else { // beam detected
    digitalWrite(LED2, HIGH); // yellow LED on
  }

  Beam3 = digitalRead (IR3);
  if (Beam3 == HIGH) { // beam interrupted
    digitalWrite(LED3, LOW); // green LED off
  }
  else { // beam detected
    digitalWrite(LED3, HIGH); // green LED on
  }

  Beam4 = digitalRead (IR4);
  if (Beam4 == HIGH) { // beam interrupted
    digitalWrite(LED4, LOW); // red LED off
  }
  else { // beam detected
    digitalWrite(LED4, HIGH); // red LED on
  }
}
