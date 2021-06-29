

// Code tested on Arduino Nano 5V (no USB)

#include <ButtonDebounce.h>

const byte footSwitchPin = 2;    // the number of the footswitch input pin
const byte buttonFootSwitchPin = 3;    // the number of the pushbutton (Footswitch mode) input pin
const byte buttonExpPedalPin = 4;    // the number of the pushbutton (exp pedal channel) input pin

const byte expPedalPin = 0;    // the ANALOG pin of expression pedal

const byte MIDIChannel = 0; // MIDI Channel the messages will be sent to
const byte PedalCC = 20; // CC for the Footswitch
const byte PedalONValue = 0; // Value for the CC (ON)
const byte PedalOFFValue = 127; //Value for the CC (OFF)

const byte ModWheelCC = 1; // MIDI Standard: CC typically assigned to the Mod Wheel
const byte VolumeCC = 7; // MIDI Standard: CC Typically assigned to volume control

const int MIN_EXP_CHANGE = 8; // 1024/128 = 8 Minimum analog change to be detected
int deltaEXPChange = 0; // Initialize delta for the expression pedal
int lastEXPValue = 0; // Initialize last Expression pedal value read

bool Toggle_CC = true;
// By default, the arduino would send PedalONValue when pressed , and PedalOFFValue immediately when depressed. 
// This makes it work more like a latch: Press once (press-depress) for one value, then on the next cycle it will send the other value
bool Toggle_EXP = true; // Same latching mechanism but for the expression pedal (Not used)
int lastSWValue = PedalOFFValue; // Initialize first footswitch value
bool FOOTSW_LOGIC = true; //True for "PRESSED = DIGITAL READ is TRUE"

ButtonDebounce buttonFootSwitch(buttonFootSwitchPin, 250);
ButtonDebounce buttonExpPedal(buttonExpPedalPin, 250);
ButtonDebounce footSW(footSwitchPin, 50);

void setup() {
  pinMode(buttonFootSwitchPin, INPUT_PULLUP); // Set internal pullups
  pinMode(buttonExpPedalPin, INPUT_PULLUP); 
  pinMode(footSwitchPin, INPUT_PULLUP);

  buttonFootSwitch.setCallback(buttonFootSwitchToggle); //Set callbacks for each input
  buttonExpPedal.setCallback(buttonExpPedalToggle);
  footSW.setCallback(footSwitchFunction);

  lastEXPValue = analogRead(expPedalPin); // Initialize first expression pedal value

// Set MIDI baud rate:
  Serial.begin(31250);
}

void loop() {
  // Updates values.
  buttonFootSwitch.update();
  buttonExpPedal.update();
  footSW.update();
  expPedalFunction();
}

void footSwitchFunction(const int state) {
  // Callback function for the footswitch - Send MIDI
  if (Toggle_CC) {
    //ONLY SEND ON PEDAL DOWN. SEND OPPOSITE OF WHAT WAS LAST SENT
    if (digitalRead(footSwitchPin) == FOOTSW_LOGIC) {
      if (lastSWValue == PedalOFFValue) {
        lastSWValue = PedalONValue;
    sendMIDICC(PedalCC, PedalONValue);
    // DEBUG Serial.println("Toggle_FootSW: ON");
      }
      else {
        lastSWValue = PedalOFFValue;
    sendMIDICC(PedalCC, PedalOFFValue);
        // DEBUG Serial.println("Toggle_FootSW: OFF");    
      }
    }
  }
  else {
    if (digitalRead(footSwitchPin) == FOOTSW_LOGIC) {
    sendMIDICC(PedalCC, PedalONValue);
      // DEBUG Serial.println("NORMAL_FootSW: ON");   
    }
    else {
    sendMIDICC(PedalCC, PedalOFFValue);
      // DEBUG Serial.println("NORMAL_FootSW: OFF");   
    }
  }
}

void expPedalFunction() {
  int currentEXPValue = analogRead(expPedalPin);
  deltaEXPChange = abs(currentEXPValue - lastEXPValue);
  if (deltaEXPChange >= MIN_EXP_CHANGE) {
    lastEXPValue = currentEXPValue;
    //Serial.println(lastEXPValue);
    //MAP AND SEND MIDI: 0 and 127 have a slightly expanded range
    int sendValue;
    if (lastEXPValue > 1010){ 
      sendValue = 127;
    }
    else if(lastEXPValue < 10){
      sendValue = 0;
    }
    else{
      sendValue = map(lastEXPValue, 0, 1023, 0, 127);
    }
    //Serial.println("Send:" + sendValue);
    if(Toggle_EXP){
      sendMIDICC(ModWheelCC, sendValue); //SEND MODWHEEL
    }
    else{
      sendMIDICC(VolumeCC, sendValue); //SEND VOLUME CC
    }
      // DEBUG Serial.println("ExpPedal: " +  String(sendValue)); 
  }
}

void buttonFootSwitchToggle(const int state) {
  //Callback for the pushbutton: Change mode for the footswitch between latch and momentary
  if (state == 1) {
    Toggle_CC = !Toggle_CC;
    // DEBUG Serial.println("Changed: " + String(state));
  }
}

void buttonExpPedalToggle(const int state) {
  //(NOT USED) Callback for the pushbutton: Change mode for the expression pedal between latch and momentary
  if (state == 1) {
    Toggle_EXP = !Toggle_EXP;
    // DEBUG Serial.println("Changed: " + String(state));
  }
}

void sendMIDICC(byte ccnumber, byte ccvalue) {
  // Generic simplified "Send MIDI" function for CC values
  byte ccstatus = 0xB0 | MIDIChannel; //Mask StatusByte: CC byte is 1011 CCCC where CCCC is the MIDI channel
  Serial.write(ccstatus);
  Serial.write(ccnumber);
  Serial.write(ccvalue);
}
