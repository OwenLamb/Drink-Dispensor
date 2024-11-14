#include "SR04.h"
#define trigPin 12
#define echoPin 11
SR04 ultrasonicSensor = SR04(echoPin, trigPin);


const int startButtonPin = 2;
const int ledPin = 3;
const int LAin1Pin = 10;
const int LAin2Pin = 9;
const int Relay1Pin = 7;
const int Relay2Pin = 6;
const int resetButtonPin = 4;
const int stopButtonPin = 5;

unsigned long startTime = -44700;
unsigned long resetTime = -44700;
unsigned long blinkTime = 0;
unsigned long checkDistance;

int startButton;
bool startFunction = false;
int resetButton;
bool resetFunction = false;
int stopButton;
int ledState = HIGH;

void setup() {
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(LAin1Pin, OUTPUT);
  pinMode(LAin2Pin, OUTPUT);
  pinMode(Relay1Pin, OUTPUT);
  pinMode(Relay2Pin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);

  digitalWrite(LAin1Pin, LOW);
  digitalWrite(LAin2Pin, HIGH);

  Serial.begin(9600);
}

void loop() {
  startButton = digitalRead(startButtonPin);
  resetButton = digitalRead(resetButtonPin);
  stopButton = digitalRead(stopButtonPin);
  if ((startButton == LOW) && (millis() - startTime) > 44700) {
    startTime = millis();
    startFunction = true;
  }

  if (resetButton == LOW && (millis() - resetTime) > 20000) {
    resetTime = millis();
    resetFunction = true;
  }

  if (stopButton == LOW) {
    startFunction = false;
    resetFunction = false;
    startTime = -44700;
    resetTime = -44700;
    // Stop all pumps and lower actuator
    digitalWrite(Relay2Pin, LOW);
    digitalWrite(Relay1Pin, LOW);
    digitalWrite(LAin1Pin, HIGH);
    digitalWrite(LAin2Pin, LOW);
  }

  //Safety measure, if there isn't a cup detected get out of the start function
  if ((millis() - startTime) > 19900 && (millis() - startTime) < 24700) {
    checkDistance = ultrasonicSensor.Distance();
    //if nothing within 22 cm's is detected then set start to false and turn off pumps
    if (checkDistance > 22) {
      startFunction = false;
      digitalWrite(Relay1Pin, LOW);
      digitalWrite(Relay2Pin, LOW);

    }
  }

  // Lower cup and fill drink if the button is pressed, within time frame, and the reset button hasn't been pressed
  if (startFunction == true && (millis() - startTime) <= 44700 && resetFunction == false) {
    // Stage one: lower actuator
    if ((millis() - startTime) <= 20000) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(LAin1Pin, HIGH);
      digitalWrite(LAin2Pin, LOW);
    }

    // Stage two: turn on pump1 then off after 6 seconds
    if ((millis() - startTime) > 20000 && (millis() - startTime) <= 23000) {
      // Turn on pumps
      digitalWrite(Relay1Pin, HIGH);
    } else if ((millis() - startTime) >= 23000) {
      digitalWrite(Relay1Pin, LOW);
    }

    // Stage three: turn on pump2 same time as 1 but turn off after 7 seconds
    if ((millis() - startTime) > 20000 && (millis() - startTime) <= 24500) {
      digitalWrite(Relay2Pin, HIGH);
    } else if ((millis() - startTime) > 24500) {
      digitalWrite(Relay2Pin, LOW);
    }

    // Stage four: raise actuator
    if ((millis() - startTime) >= 24700) {
      digitalWrite(LAin1Pin, LOW);
      digitalWrite(LAin2Pin, HIGH);
    }
  } else if (startFunction) {
    digitalWrite(ledPin, LOW);
    startFunction = false;
  }

  // Reset actuator position
  if (resetFunction == true && (millis() - resetTime) <= 20000) {
    digitalWrite(LAin1Pin, LOW);
    digitalWrite(LAin2Pin, HIGH);

    // Blink LED every half second
    if ((millis() - blinkTime) >= 500) {
      digitalWrite(ledPin, ledState);
      ledState = !ledState;
      blinkTime = millis();
    }
  } else {
    resetFunction = false;
    digitalWrite(ledPin, LOW);
  }
}
