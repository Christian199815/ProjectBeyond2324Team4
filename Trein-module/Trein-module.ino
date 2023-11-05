#include <Adafruit_NeoPixel.h>  //ledstrip library
#include "KT403A_Player.h"      //audio library
#include <SoftwareSerial.h>     //Serial library
#include <NewPing.h>            //ultrasonic library
#include <elapsedMillis.h>      //installeer de lapsedMillis.h library (van Peter Freerick)
#include "Arduino.h"


#define USRPIN 7   //set ultrasonic ranger sensor to correct pin
#define MP3PIN 2   //set MP3 actuator to correct pin
#define LEDPIN1 5  //set ledstrip actuator to correct pin
#define LEDPIN2 2  //set ledstrip actuator to correct pin
#define MOTPIN 8   //set motion sensor to correct pin

#define MAX_DISTANCE 200  //set maximal distance of checkking for the ranger sensor
#define NUMPIXELS 15      //set the number of led pixels on the ledstrip


//mp3
SoftwareSerial SoftSerial(MP3PIN, 3);  //define the mp3 component
KT403A<SoftwareSerial> MP3Player;      //activate the mp3 component
int playingSong = 99;                  //set the current playing song to a high number far away


//Ultrasonic ranger
NewPing sonar(USRPIN, USRPIN, MAX_DISTANCE);  //activate the ranger component

//Ledstrip1
Adafruit_NeoPixel pixelsStrip1(NUMPIXELS, LEDPIN1, NEO_GRB + NEO_KHZ800);  //activate the first ledstrip

//Ledstrip2
Adafruit_NeoPixel pixelsStrip2(NUMPIXELS, LEDPIN2, NEO_GRB + NEO_KHZ800);  //activate the second ledstrip

long distance = 0;     //create a number that holds the detecting distance of the ranger component
long curDistance = 0;  //create a number that holds the current number, for double checking
elapsedMillis verstrekenTijd1;

//1: toggle mode, 2: follow mode
#define LED_MODE   1

const int ledPin = 3;      // the number of the LED pin, D3
const int buttonPin = 4;    // the number of the pushbutton pin, D4
const boolean breathMode = true;  // if or not the led lights as breath mode when it's on

// Variables will change:
int ledState = LOW;         // the current state of the output pin
int ledFadeValue = 0;
int ledFadeStep = 5;
int ledFadeInterval = 20;   //milliseconds
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;
bool isPressed = false;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long lastLedFadeTime = 0;


void setup() {
  Serial.begin(9600);      //start the normal serial monitor
  SoftSerial.begin(9600);  //start the mp3 serial monitor

  pixelsStrip1.begin();  //turn on the first ledstrip
  pixelsStrip2.begin();  //turn on the second ledstrip

  // pixelsStrip1.show();  //let the lights on the first ledstrip be visible
  // pixelsStrip2.show();  //let the lights on the second ledstrip be visible

  pixelsStrip1.setBrightness(255);  // set the brightness on the first ledstrip
  pixelsStrip2.setBrightness(255);  // set the brightness on the second ledstrip

  MP3Player.init(SoftSerial);  //initiate the mp3 component
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  pinMode(MOTPIN, INPUT);  //initatie the motion sensor
}

void loop() 
{
  ButtonPress();
  Serial.print(isPressed);
  if (verstrekenTijd1 > 500) 
  {
    distance = sonar.ping_cm();
    Serial.print(distance);
    CollapseDataLong(curDistance, distance);

    DistanceChecker();
    verstrekenTijd1 = 0;
  }
  delay(1);
}

  //function that according to a number will play the number in the list on the SD card
void PlayAudio(int song) {
      MP3Player.playSongMP3(song);
      playingSong = song;
    }
  }


  //the function below makes sure that the the distance number wont be displayed for multiple times
  //but only shows the changes
  void CollapseDataLong(long value1, long value2) {
    if (value1 != value2) {
      Serial.print(value1);
      Serial.print("\n");
      value1 = value2;
    }
  }

//the function below checks the distance of the ranger sensor, if the ranger sensor detects an obstruction
//than the motion sensor will detect if the obstruction moves and so will the system continue its functions
void DistanceChecker() 
{
  if(isPressed)
  {
    if(distance > 140)
    {
      Serial.println("NO ONE");
    PlayAudio(7);
    GreenLightsOn();
    delay(3000);
    }
    else if (distance < 140 || digitalRead(MOTPIN)) 
    {
      Serial.println("LOOK THERE!");
      PlayAudio(8);
      RedLightsOn();

    }

  }else if(!isPressed)
  {
    pixelsStrip1.clear();
    pixelsStrip2.clear();
    pixelsStrip1.show();
    pixelsStrip2.show();
  }

}



void RedLightsOn() 
{
  pixelsStrip1.clear();
  pixelsStrip2.clear();

  for (uint16_t i = 0; i < pixelsStrip1.numPixels(); i = i + 2) 
  {
    pixelsStrip1.setPixelColor(i + 3, pixelsStrip1.Color(255, 0, 0));
    pixelsStrip2.setPixelColor(i + 3, pixelsStrip2.Color(255, 0, 0));
  }
    pixelsStrip1.show();
    pixelsStrip2.show();
}

void GreenLightsOn() 
{
  pixelsStrip1.clear();
  pixelsStrip2.clear();
  for (uint16_t i = 0; i < pixelsStrip1.numPixels(); i = i + 2) 
  {
    pixelsStrip1.setPixelColor(i + 3, pixelsStrip1.Color(0, 255, 0));
    pixelsStrip2.setPixelColor(i + 3, pixelsStrip2.Color(0, 255, 0));
  }
    
  pixelsStrip1.show();
  pixelsStrip2.show();
}


void ButtonPress()
{
int reading = digitalRead(buttonPin);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    isPressed = !isPressed;
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
                isPressed = !isPressed;
      }
#else
      if (buttonState == LOW) {  //button is pressed
        ledState = HIGH;
        ledFadeValue = 0;
        lastLedFadeTime = millis();
      } else {                   //button is released
        ledState = !ledState;
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




