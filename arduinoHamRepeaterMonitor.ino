/*
 * Ham_RepeaterMonitor
 * 
 * 
 * https://www.arduino.cc/en/Reference/LiquidCrystalConstructor
 * 
 * 
 * The Circuit:
 *  TBD
 *
 * LCD RS pin to digital pin 
 */


#include <LiquidCrystal.h>
#include <Wire.h>


#define CHARS_PER_LINE 20
#define LCD_LINES 4

#define FWDPOWER_IN A3
#define REVPOWER_IN A5
#define REDLITE 3
#define GREENLITE 5
#define BLUELITE 6
#define RELAYPIN_OUT 13


// initialize the library with the numbers of the interface pins
// rs, enable, d,d,d,d)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

   int brightness = 255;
   int relayStatus = 0;
   int analogPinFWD = FWDPOWER_IN;
   int analogPinREV = REVPOWER_IN;
   int digitalPinPowerCtrl = RELAYPIN_OUT; 
   int refreshTime = 500; // MilliSeconds
   unsigned long maxSeconds = 16;
   unsigned long countDownSeconds = maxSeconds;
   unsigned long visualWarningSeconds = maxSeconds/4;
   unsigned long startTime = 0;
   int resetTime = 10;  // I can not remember what this was for
   int fwdReading = 0;
   int revReading = 0;
   int voltageThreshold = 400;
   int inTransmit = 0 ; 
   int powerCycleDelay = 5000; // wait for 5 seconds



   
void setup() {
   Serial.begin(9600);
   lcd.begin(CHARS_PER_LINE, LCD_LINES);
   lcd.print("Repeater Watchdog v1");
   pinMode(digitalPinPowerCtrl, OUTPUT);
   pinMode(REDLITE, OUTPUT);
   pinMode(GREENLITE, OUTPUT);
   pinMode(BLUELITE, OUTPUT);
   digitalWrite(digitalPinPowerCtrl, LOW); // default to low for on.
   lcd.setCursor(0,1);
   lcd.print((String)"Checking Status...");
}

void powerCycle(){
  setBacklight(255,0,0);
  clearLine(1);
  lcd.print((String)"Power: Set CTRL HIGH");
  digitalWrite(digitalPinPowerCtrl,HIGH); // 
  Serial.println((String) "Power off at " + millis());
  Serial.flush();
  delay(powerCycleDelay);
  clearLine(1);
  lcd.print("Power: Set CTRL LOW ");
  digitalWrite(digitalPinPowerCtrl,LOW);
  Serial.println((String) "Power on at " + millis());
  delay(1000);
  inTransmit=0;  
  startTime=0;
}


void loop() {
  fwdReading = analogRead(analogPinFWD);
  revReading = analogRead(analogPinREV);
  Serial.print((String) "FWD: " + fwdReading + " / REV: " + revReading);
  Serial.print((String) " / inTX: " + inTransmit + " / StartTime: " + startTime);
  Serial.println((String) " Timer: " + countDownSeconds);
  Serial.flush();
  clearLine(2);
  lcd.print((String)"Last FWD Power: " + fwdReading );
  clearLine(3);
  lcd.print((String)"Last REV Power: " + revReading );
  if(fwdReading > voltageThreshold) {
    if(startTime <= 0){
            startTime=millis();
    }
    if(!inTransmit){  //start timer
      inTransmit++;
      setBacklight(0,50,255);  // BLUE in TX 
      startTime=millis();
      clearLine(1);
      lcd.print((String)"Status: Tx: " + maxSeconds);
    }else{        // check Timer
      countDownSeconds = maxSeconds - ((millis()-startTime)/1000);
      if(countDownSeconds < visualWarningSeconds ){
        setBacklight(255,255,0);
      }
      if(countDownSeconds < 3 ){
        setBacklight(255,0,0);
      }
      clearLine(1);
      lcd.print((String)"Status: Tx: " + countDownSeconds);
      Serial.println((String) " Timer: " + countDownSeconds);

      if(countDownSeconds <= 0 ){
        powerCycle();
        resetTimer();
      }
    }
    
  }else{ // Repeater is NOT transmitting now
    resetTimer();
  }
  delay(refreshTime);
}

void resetTimer(){
   countDownSeconds = maxSeconds;
   inTransmit=0;
   startTime = 0;
   setBacklight(0,255,0); // Not in TX. GREEN
   clearLine(1);
   lcd.print((String)"Status: Idle");  
}

void clearLine(int lineIDX){
  lcd.setCursor(0,lineIDX);
  for(int i=0;i<CHARS_PER_LINE;i++){
    lcd.print(" ");
  }
  lcd.setCursor(0,lineIDX);
}

void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest!
  r = map(r, 0, 255, 0, 100);
  g = map(g, 0, 255, 0, 150);
  // Adjust the brightness
  r = map(r, 0, 255, 0, brightness);
  g = map(g, 0, 255, 0, brightness);
  b = map(b, 0, 255, 0, brightness);
 
  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);
  Serial.print("R = "); Serial.print(r, DEC);
  Serial.print(" G = "); Serial.print(g, DEC);
  Serial.print(" B = "); Serial.println(b, DEC);
  analogWrite(REDLITE, r);
  analogWrite(GREENLITE, g);
  analogWrite(BLUELITE, b);
}
