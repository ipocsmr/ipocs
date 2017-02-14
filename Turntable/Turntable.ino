/*
  Turntable control

  This example code is in the public domain.

*/
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

//Definition of outputs used for the Motor
const int CW = 3;
const int CCW = 4;
//Definition of output used for Rotation Warning
const int Warning = 5;
//Definition of inpust used for positioning
const int Homepos = 6;
const int Trackpos = 7;
//Variables to save current pos
int currentpos = 0;
//Variables to save new pos
int newpos = 0;
boolean registered=false;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 10 bytes for the inputString:
  inputString.reserve(10);
  //initialize IO
  pinMode(CW, OUTPUT);
  pinMode(CCW, OUTPUT);
  pinMode(Warning, OUTPUT);
  pinMode(Homepos, INPUT_PULLUP);
  pinMode(Trackpos, INPUT_PULLUP);
 //Initialize turntable position
void initpos();
}

void loop() {

  // Handle incoming order:
  if(stringComplete) {
    //Convert input string to integer number and store in variable "newpos"
    newpos = inputString.toInt();
    stringComplete = false;}
     //Handle request to reinitilize
    if(0==newpos)void initpos();
    // if new position is higher than current position rotate CW to new pos
    // minus 1 plus release of track position, then wait to reach newpos
    if(newpos > currentpos) {
      // activate warning
      digitalWrite(Warning, HIGH);
      //Start rotate and keep rotating until reaching and passing new track -1
      if (newpos - currentpos > 1 && digitalRead(Trackpos) == LOW )digitalWrite(CW, HIGH);
      //Check if trackpos goes LOW and increment currpos
      if (digitalRead(Trackpos) == LOW && registered == false) {
        currentpos = currentpos + 1;
        registered = true;
      }
      //continue to rotate until trackpos goes high then set registered to false
      if (digitalRead(Trackpos) == HIGH && registered == true) registered = false;
      //approaching Newpos, lower rotating output
      if (1 == (newpos - currentpos) && digitalRead(Trackpos) == HIGH) digitalWrite(CW, LOW);
      //wait to reach Newpos
      if (1 == (newpos - currentpos) && digitalRead(Trackpos) == LOW)
        //set currpos=newpos
        currentpos = newpos;
      //deactivate warning
      digitalWrite(Warning, LOW);
   //End CW rotation to new pos
    }

    
    // if new position is lower than current position rotate CCW to new pos
    // plus 1 plus release of track position, then wait to reach newpos
    if(newpos < currentpos) {
      // activate warning
      digitalWrite(Warning, HIGH);
      //Start rotate and keep rotating until reaching and passing new track -1
      if (currentpos - newpos > 1 && digitalRead(Trackpos) == LOW )digitalWrite(CCW, HIGH);
      //Check if trackpos goes LOW and increment currpos
      if (digitalRead(Trackpos) == LOW && registered == false) {
        currentpos = currentpos - 1;
        registered = true;
      }
      //continue to rotate until trackpos goes high then set registered to false
      if (digitalRead(Trackpos) == HIGH && registered == true) registered = false;
      //approaching Newpos, lower rotating output
      if (1 == (currentpos - newpos) && digitalRead(Trackpos) == HIGH) digitalWrite(CCW, LOW);
      //wait to reach Newpos
      if (1 == (currentpos - newpos) && digitalRead(Trackpos) == LOW)
        //set currpos=newpos
        currentpos = newpos;
      //deactivate warning
      digitalWrite(Warning, LOW);
   //End CCW rotation to new pos
    }

    //End loop
  }

  //Function to handle incoming orders via USB
  void serialEvent() {
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n') {
        stringComplete = true;
      }
    }
  }
  void initpos() {
    // activate warning
    digitalWrite(Warning, HIGH);
    // rotate CCW to Homepos
    digitalWrite(CCW, HIGH);
    while(digitalRead(Homepos) == HIGH)delay(1);
    digitalWrite(CCW, LOW);
    //Rotate CW to pos 1
    digitalWrite(CW, HIGH);
    while(digitalRead(Homepos) == LOW)delay(1);
    digitalWrite(CW, LOW);
    //wait until turtable reaches pos 1
    while(digitalRead(Trackpos) == HIGH)delay(1);
    //wait until in position
    delay(1000);
    //Set current position 1
    currentpos = 1;
    //deactivate warning
    digitalWrite(Warning, LOW);
  }


