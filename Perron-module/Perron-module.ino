#include <Ultrasonic.h>
#include <EasyUltrasonic.h>
#include <Adafruit_NeoPixel.h>
#include "KT403A_Player.h"
#include <SoftwareSerial.h>
#include <elapsedMillis.h>
#include <NewPing.h>
#include "Arduino.h"


#define LEDPIN3 2
#define LEDPIN1 6
#define LEDPIN2 8
#define VIBRAPIN 5


#define LED_MODE 1

#define MAX_DISTANCE 200
#define NUMPIXELS 15
#define DELAYVAL 20
const int USRPIN  = 7;

//Time
elapsedMillis lapsedTime;

//Ultrasonic ranger
NewPing sonar(USRPIN, USRPIN, MAX_DISTANCE);

//Ledstrip1
Adafruit_NeoPixel pixelsStrip1(NUMPIXELS, LEDPIN1, NEO_GRB + NEO_KHZ800); //links

//Ledstrip2
Adafruit_NeoPixel pixelsStrip2(NUMPIXELS, LEDPIN2, NEO_GRB + NEO_KHZ800); //rechts

//Ledstrip2
Adafruit_NeoPixel pixelsStrip3(NUMPIXELS, LEDPIN3, NEO_GRB + NEO_KHZ800); // midden

const int ledPin = 3;      // the number of the LED pin, D3
const int buttonPin = 4;    // the number of the pushbutton pin, D4

const int trainDistance = 60;
const int cheatDistance = 30;
elapsedMillis verstrekenTijd1;

const boolean breathMode = true;  //

long distance = 0;
long curDistance = 0;

int ledState = LOW;         // the current state of the output pin
int ledFadeValue = 0;
int ledFadeStep = 5;
int ledFadeInterval = 20;   //milliseconds
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long lastLedFadeTime = 0;




bool isPressed;

void setup() {
  Serial.begin(9600);      //normal serial monitor

  pixelsStrip1.begin();  //ledstrip 1
  pixelsStrip2.begin();  //ledstrip 2
  pixelsStrip3.begin();

  pixelsStrip1.show();
  pixelsStrip2.show();
  pixelsStrip3.show();

  pixelsStrip1.setBrightness(255);
  pixelsStrip2.setBrightness(255);
  pixelsStrip3.setBrightness(255);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  pinMode(VIBRAPIN, OUTPUT);
}

void loop() {
  ButtonPress();
  Serial.print(isPressed);
  if (verstrekenTijd1 > 5) 
  {
    distance = sonar.ping_cm();
    Serial.print(distance);
    Serial.println();

    DistanceChecker();
    verstrekenTijd1 = 0;
  }
  delay(1);
  ButtonPress();
  DistanceChecker();
}


void DistanceChecker() {

  if(distance > trainDistance)
  {
    //trein is niet aanwezig zet de module uit
    VibrateFloor(false);
    DisableLights();
    
  }

  if(distance <= trainDistance)
  {
    if(distance <= cheatDistance)
    {
      // GreenLightsOn();
      GreenLightsOn();
      VibrateFloor(false);
    }
    else
    {
      RedLightsOn();
      if (lastButtonState == 0) 
      {
        RedLightFlash();
        VibrateFloor(true);
      }
      else if(lastButtonState == 1)
      {
      VibrateFloor(false);
      }
    }
  }

delay(50);
}



void DisableLights()
{
  pixelsStrip1.clear();
  pixelsStrip2.clear();
  pixelsStrip3.clear();
  pixelsStrip1.show();
  pixelsStrip2.show();
  pixelsStrip3.show();

}

void ButtonPress()
{
   int reading = digitalRead(buttonPin);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

#if LED_MODE == 1
      if (buttonState == LOW) {  //button is pressed
          ledState = !ledState;
          ledFadeValue = 0;
          lastLedFadeTime = millis();
      }
#else
      if (buttonState == LOW) {  //button is pressed
        ledState = HIGH;
        ledFadeValue = 0;
        lastLedFadeTime = millis();
      } else {                   //button is released
        ledState = LOW;
      }
#endif
    }
  }

  // set the LED:
  if (breathMode && ledState != LOW) {
    if (millis() - lastLedFadeTime > ledFadeInterval) {
      lastLedFadeTime = millis();
      analogWrite(ledPin, ledFadeValue);
      ledFadeValue += ledFadeStep;
      if (ledFadeValue > 255){
        ledFadeValue = 255 - ledFadeStep;
        ledFadeStep = -ledFadeStep;
      } else if (ledFadeValue < 0) {
        ledFadeValue = 0;
        ledFadeStep = -ledFadeStep;
      }
    }
  } else {
    digitalWrite(ledPin, ledState);
  }

  lastButtonState = reading;
}

void VibrateFloor(bool goVibrate) {
  if (goVibrate) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(VIBRAPIN, HIGH);
      delay(500);
    }
  }
  if (!goVibrate) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(VIBRAPIN, LOW);
      delay(500);
    }
  }
}



  void RedLightsOn() {
    pixelsStrip1.clear();
    pixelsStrip2.clear();
    pixelsStrip3.clear();

    for (uint16_t i = 0; i < pixelsStrip1.numPixels(); i = i + 2) {
      pixelsStrip1.setPixelColor(i + 3, pixelsStrip1.Color(255, 0, 0));
      pixelsStrip2.setPixelColor(i + 3, pixelsStrip2.Color(255, 0, 0));
      pixelsStrip3.setPixelColor(i + 3, pixelsStrip3.Color(255, 0, 0));
    }
    pixelsStrip1.show();
    pixelsStrip2.show();
    pixelsStrip3.show();
  }

  void GreenLightsOn() {
    pixelsStrip3.clear();
    pixelsStrip3.show();

    pixelsStrip1.clear();
    pixelsStrip2.clear();
    for (uint16_t i = 0; i < pixelsStrip1.numPixels(); i = i + 2) {
      pixelsStrip1.setPixelColor(i + 3, pixelsStrip1.Color(0, 255, 0));
      pixelsStrip2.setPixelColor(i + 3, pixelsStrip2.Color(0, 255, 0));
    }
    pixelsStrip1.show();
    pixelsStrip2.show();
  }

  void RedLightFlash() {
    for (int i = 0; i < 10; i++) {
      for (int q = 0; q < 3; q++) {
      for (short i = 0; i < NUMPIXELS; i++) {
        pixelsStrip1.setPixelColor(i + q, pixelsStrip1.Color(255, 0, 0));
        pixelsStrip2.setPixelColor(i + q, pixelsStrip2.Color(255, 0, 0));
        pixelsStrip3.setPixelColor(i + q, pixelsStrip3.Color(255, 0, 0));
      }
      pixelsStrip1.show();
      pixelsStrip2.show();
      pixelsStrip3.show();

      delay(30);

      for (short i = 0; i < NUMPIXELS; i++) {
        pixelsStrip1.setPixelColor(i + q, 0);
        pixelsStrip2.setPixelColor(i + q, 0);
        pixelsStrip3.setPixelColor(i + q, 0);
      }
      pixelsStrip1.show();
      pixelsStrip2.show();
      pixelsStrip3.show();
      }
    }
  }


