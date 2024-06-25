const int startButtonPin = 2;
const int ledPin = 3;
const int enaPin = 11;
const int LAin1Pin = 10;
const int LAin2Pin = 9;
const int Relay1Pin = 7;
const int Relay2Pin = 6;
const int resetButtonPin = 4;
const int stopButtonPin = 5;

void setup() {
  // put your setup code here, to run once:
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

  //digitalWrite(11, HIGH);
  Serial.begin(9600);
}
unsigned long startTime = -32200;
unsigned long resetTime = -32200;
unsigned long stopTime = 0;

int startButton;
bool startFunction = false;
int resetButton;
bool resetFunction = false;
int stopButton;
bool trip1 = true;


void loop() {
  startButton = digitalRead(startButtonPin);
  resetButton = digitalRead(resetButtonPin);
  stopButton = digitalRead(stopButtonPin);

  if ((startButton == LOW) && (millis() - startTime) > 32200) {
    Serial.println("I AM HERE");
    startTime = millis();
    Serial.println(millis());
    Serial.println(startTime);

    startFunction = true;
  }

  if (resetButton == LOW && (millis() - resetTime) > 12500) {
    resetTime = millis();
    resetFunction = true;
  }

  if (stopButton == LOW) {
    startTime = -32200;
    resetTime = -32200;
    //stop all pumps and lower actuator
    digitalWrite(Relay2Pin, LOW);
    digitalWrite(Relay1Pin, LOW);
    digitalWrite(LAin1Pin, HIGH);
    digitalWrite(LAin2Pin, LOW);
  }

  //Lower cup and fill drink
  if (startFunction == true && (millis() - startTime) <= 32200) {
    //Stage one lower actuator
    if (trip1) {
      Serial.println("intrip");
      digitalWrite(ledPin, HIGH);
      digitalWrite(LAin1Pin, HIGH);
      digitalWrite(LAin2Pin, LOW);
      trip1 = false;
    }

    //Stage two turn on pump1 then off after 6 seconds
    if ((millis() - startTime) > 12500 && (millis() - startTime) <= 18500) {
      //Turn on pumps
      digitalWrite(Relay1Pin, HIGH);
    } else if ((millis() - startTime) >= 18500) {
      digitalWrite(Relay1Pin, LOW);
    }

    //Stage three turn on pump2 same time as 1 but turn off after 7 seconds
    if ((millis() - startTime) > 12500 && (millis() - startTime) <= 19500) {
      digitalWrite(Relay2Pin, HIGH);
    } else if ((millis() - startTime) > 19500) {
      digitalWrite(Relay2Pin, LOW);
    }

    //Stage four raise actuator
    if ((millis() - startTime) >= 19700) {
      digitalWrite(LAin1Pin, LOW);
      digitalWrite(LAin2Pin, HIGH);
    }
  } else {
    digitalWrite(ledPin, LOW);
    startFunction == false;
    trip1 = true;
  }

  //Reset actuator position
  if ((millis() - resetTime) <= 12500) {
    Serial.println("In reset");
    digitalWrite(LAin1Pin, LOW);
    digitalWrite(LAin2Pin, HIGH);
    //if stop is initiated or the actuator is done rising
  } else if (resetFunction == true) {
    resetFunction == false;
  }
}
