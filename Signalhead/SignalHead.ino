/*
  Signal Head Control with the ability to Lit, blink or turn off an output.
  In IPOCS Signal Heads for one signal mast are grouped to allow them to blink sychronized.
  Outputs are turned on and off softly through PWM control of the output (output does not need to be PWM capable)
  And blink intensity follows i sine wave curve ON/OFF

  This code is in the public domain.

*/


String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
//80micro second step with a resolution of 254 equals 49hz
const int resolution = 80;
long lastfade;
int steg = 1;
int output = 11;
int dutycykle = 0;
unsigned long laststart = 0;
//This is the intensity values the on/off and blink functions use
int intensity[256] = {0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 6, 6, 8, 9, 10, 11, 12, 14, 15, 17, 18, 20, 22, 23, 25, 27, 29, 31, 33, 35, 38, 40, 42, 45, 47, 49, 52, 54, 57, 60, 62, 65, 68, 71, 73, 76, 79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 113, 116, 119, 122, 125, 128, 131, 135, 138, 141, 144, 147, 150, 153, 156, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 189, 191, 194, 197, 199, 202, 204, 207, 209, 212, 214, 216, 218, 221, 223, 225, 227, 229, 231, 232, 234, 236, 238, 239, 241, 242, 243, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 253, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 241, 239, 238, 236, 234, 232, 231, 229, 227, 225, 223, 221, 218, 216, 214, 212, 209, 207, 204, 202, 199, 197, 194, 191, 189, 186, 183, 180, 177, 174, 171, 168, 165, 162, 159, 156, 153, 150, 147, 144, 141, 138, 135, 131, 128, 125, 122, 119, 116, 113, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 76, 73, 71, 68, 65, 62, 60, 57, 54, 52, 49, 47, 45, 42, 40, 38, 35, 33, 31, 29, 27, 25, 23, 22, 20, 18, 17, 15, 14, 12, 11, 10, 9, 8, 6, 6, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0};
int PWMvalue = 0;
long laststep = 0;
int fadeValue = 0;
int frequency = 1;
int fadestep=int((frequency*1000)/254);
int lamppin = 11;
//Variables to save the state each lamp should have 0==off, 1==on, 2==blink
int state = 0;
int currentstatus=0;
int pin=11;
int stat=0;
 /*
    Status definitions
    From IPOCS */
# define off 0
# define on 1
# define flash 2
  //Internal statuses
# define rising 3
# define fading 4

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 10 bytes for the inputString:
  inputString.reserve(10);
  pinMode(output, OUTPUT);
}

void loop() {
  // Handle incoming order:
  if (stringComplete) {
    //Convert input string to integer number and store in variable "state"
    state = inputString.toInt();
    Serial.println(state);
        //When a new command comes in, Set the new lamphead state
    inputString="";
    stringComplete=false;
  }
  //Run aspect control with fadestep delay
   
  if (millis() - lastfade > fadestep) {
  lastfade = millis();
    //step the pointer for current lamp intensity value
    fadeValue = fadeValue + 1;
    if (fadeValue > 255)fadeValue = 0;
    PWMvalue = intensity[fadeValue];
    }
   
  //Update LampHead
    LampHead(lamppin, state);
  //End loop

}
void LampHead(int pin, int stat) {
 
int newstatus=stat;
    if (newstatus != currentstatus) {

    // If current status == off and new status == on set status rising and start initilize on at fadeValue==0 and set status on at fadeValue=128;    if (currentstatus == off && newstatus == on) {if(0==fadeValue) currentstatus=rising;}
    if (currentstatus == off && newstatus == on) {if(0==fadeValue) currentstatus=rising;}
    if (currentstatus == rising && newstatus == on) {if(128==fadeValue) currentstatus=on;}
    
    // If current status == off and new status == flash at fadeValue=0 set status blinking
    if (currentstatus == off && newstatus == flash) {if(0==fadeValue) currentstatus=flash;}
       
    // If current status == on and new status == off set status fading at fadeValue=128 and off at fadeValue=0
    if (currentstatus == on && newstatus == off) {if(128==fadeValue) currentstatus=fading;}
    if (currentstatus == fading && newstatus == off) {if(0==fadeValue) currentstatus=off;}
    
    // If current status == on and new status == flash set status blinking at  fadeValue=128
    if (currentstatus == on && newstatus == flash) {if(0==fadeValue) currentstatus=flash;}

    // If current status == flash and new status == off set status off at  fadeValue=0
    if (currentstatus == flash && newstatus == off) {if(128==fadeValue) currentstatus=fading;}
    if (currentstatus == fading && newstatus == off) {if(0==fadeValue) currentstatus=off;}
    
    // If current status == flashing and new status == on set status on at fadeValue=128
    if (currentstatus == flash && newstatus == on) {if(128==fadeValue) currentstatus=on;}
  }
if(currentstatus==off)PWM(lamppin, 0);
if(currentstatus==rising)PWM(lamppin, PWMvalue);
if(currentstatus==fading)PWM(lamppin, PWMvalue);
if(currentstatus==on)PWM(lamppin, 254);
if(currentstatus==flash)PWM(lamppin, PWMvalue);
//End LampHead
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

void PWM(int pinne, int intensity) {

  if (micros() > (laststart + (resolution * intensity))) digitalWrite(pinne, LOW); else digitalWrite(pinne, HIGH);
  if (micros() > (laststart + (254L * resolution))) laststart = micros();

}

