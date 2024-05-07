#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int buzzer = 8;
const int sensorPin = A0;

const int button1 = 6;
const int button2 = 7;
const int button3 = 9;
const int button4 = 10;

const int debounceDelay = 150;
int lastDebounceTime = 0;

int hours = 0;
int minutes = 0;
int seconds = 0;

enum Modes {
	MAIN_MENU = 0,
  	ALARM_SETTING = 1,
  	LIGHT_INTENSITY_SETTING = 2
};

enum SettingTimeUnits {
  HOURS = 0,
  MINUTES = 1,
  SECONDS = 2
};

enum SettingTimeActions {
  INCREMENT = 0,
  DECREMENT = 1
};

Modes mode = MAIN_MENU;

SettingTimeUnits settingTimeUnit = HOURS;

int sensor;
const int threshold = 500;

bool isSetAlarmOpen = false;

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  displayMainMenu();
}

void loop() {
  sensor = analogRead(sensorPin);
  // Serial.println(sensor);
  if(sensor > threshold){
  	digitalWrite(buzzer, HIGH);
  } else {
  	digitalWrite(buzzer, LOW);
  }
  
  handleSetAlarm();
}

void displayMainMenu () {
  lcd.clear();
  lcd.print("1.Set Alarm Time");
  lcd.setCursor(0, 1);
  lcd.print("2.Set light intensity");
}

void handleSetAlarm () {
  if (mode == MAIN_MENU) {
    
    if (buttonRead(button1)) {
      mode = ALARM_SETTING;
      resetTime();
      displaySetAlarm();
    }
  }
  
  if (mode == ALARM_SETTING) {
    switch (settingTimeUnit) {
      case HOURS:
        if (buttonRead(button3)) {
          executeTimeAction(HOURS, INCREMENT);
          displaySetAlarm();
          return;
        }

        if (buttonRead(button4)) {
          executeTimeAction(HOURS, DECREMENT);
          displaySetAlarm();
          return;
        }

        if (buttonRead(button1)) {
          settingTimeUnit = MINUTES;
        }
      	break;
      case MINUTES:
      	if (buttonRead(button3)) {
          executeTimeAction(MINUTES, INCREMENT);
          displaySetAlarm();
          return;
        }

        if (buttonRead(button4)) {
          executeTimeAction(MINUTES, DECREMENT);
          displaySetAlarm();
          return;
        }

        if (buttonRead(button1)) {
          settingTimeUnit = SECONDS;
        }
      	break;
      case SECONDS:
      	if (buttonRead(button3)) {
          executeTimeAction(SECONDS, INCREMENT);
          displaySetAlarm();
          return;
        }

        if (buttonRead(button4)) {
          executeTimeAction(SECONDS, DECREMENT);
          displaySetAlarm();
          return;
        }

        if (buttonRead(button1)) {
          settingTimeUnit = HOURS;
          mode = MAIN_MENU;
          displayMainMenu();
        }
      	break;
    }   
  }
}

void displaySetAlarm () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set alarm time:");
  lcd.setCursor(0, 1);
  displayTime();
}

void displayTime () {
  if (hours <= 9){
  	lcd.print("0");
  }
  
  lcd.print(hours);
  lcd.print(":");
  
  if (minutes <= 9){
  	lcd.print("0");
  }
  
  lcd.print(minutes);
  lcd.print(":");
  
  if (seconds <= 9){
  	lcd.print("0");
  }
    lcd.print(seconds);
}

void executeTimeAction (int unit, int action) {
  switch (unit) {
    case HOURS:
      if (action == INCREMENT) {
        if (hours == 24) {
          hours = 0;
        } else {
          hours++;
        }
      } else {
        if (hours == 0)
        {
          hours = 24;
        } else {
          hours--;
        }
      }
      break;
    case MINUTES:
      if (action == INCREMENT) {
        if (minutes == 60) {
          minutes = 0;
        } else {
          minutes++;
        }
      } else {
        if (minutes == 0)
        {
          minutes = 60;
        } else {
          minutes--;
        }
      }
      break;
    case SECONDS:
      if (action == INCREMENT) {
        if (seconds == 60) {
          seconds = 0;
        } else {
          seconds++;
        }
      } else {
        if (seconds == 0)
        {
          seconds = 60;
        } else {
          seconds--;
        }
      }
      break;
  }
}

void resetTime () {
  hours = 0;
  minutes = 0;
  seconds = 0;
}

bool buttonRead (int buttonPin) {
  if (!canBeClicked()) return false;
    
  int buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
	lastDebounceTime = millis();
    return true;
  }
  
  return false;
}

bool canBeClicked () {
  return ((millis() - lastDebounceTime) > debounceDelay);
}