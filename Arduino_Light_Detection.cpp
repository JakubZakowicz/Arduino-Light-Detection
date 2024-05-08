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

bool alarmSet = false;
int alarmHours;
int alarmMinutes;
int alarmSeconds;

int settingHours = 0;
int settingMinutes = 0;
int settingSeconds = 0;

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
  
  if (alarmSet) checkAlarmTime();
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
      resetSettingTime();
      displaySetAlarm();
    }
  }
  
  if (mode == ALARM_SETTING) {
    switch (settingTimeUnit) {
      case HOURS:
      	if (buttonRead(button1)) {
          settingTimeUnit = MINUTES;
          displaySetAlarm();
        }
      
      	if (buttonRead(button2)) {
          settingTimeUnit = HOURS;
          mode = MAIN_MENU;
          displayMainMenu();
        }
      
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
      
      	break;
      case MINUTES:
       	if (buttonRead(button1)) {
          settingTimeUnit = SECONDS;
          displaySetAlarm();
        }
      
        if (buttonRead(button2)) {
          settingTimeUnit = HOURS;
          displaySetAlarm();
        }
      
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
      
      	break;
      case SECONDS:
      	if (buttonRead(button1)) {
          setAlarm();
          settingTimeUnit = HOURS;
          mode = MAIN_MENU;
          displayMainMenu();
        }
      
        if (buttonRead(button2)) {
          settingTimeUnit = MINUTES;
          displaySetAlarm();
        }
      
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

      	break;
    }   
  }
}

void displaySetAlarm () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set alarm time:");
  
  switch (settingTimeUnit) {
  	case HOURS:
      lcd.print("h");
      break;
    case MINUTES:
      lcd.print("m");
      break;
    case SECONDS:
      lcd.print("s");
      break;
  }
  
  lcd.setCursor(0, 1);
  displayTime();
}

void displayTime () {
  if (settingHours <= 9){
  	lcd.print("0");
  }
  
  lcd.print(settingHours);
  lcd.print(":");
  
  if (settingMinutes <= 9){
  	lcd.print("0");
  }
  
  lcd.print(settingMinutes);
  lcd.print(":");
  
  if (settingSeconds <= 9){
  	lcd.print("0");
  }
    lcd.print(settingSeconds);
}

void executeTimeAction (int unit, int action) {
  switch (unit) {
    case HOURS:
      if (action == INCREMENT) {
        if (settingHours == 24) {
          settingHours = 0;
        } else {
          settingHours++;
        }
      } else {
        if (settingHours == 0)
        {
          settingHours = 24;
        } else {
          settingHours--;
        }
      }
      break;
    case MINUTES:
      if (action == INCREMENT) {
        if (settingMinutes == 60) {
          settingMinutes = 0;
        } else {
          settingMinutes++;
        }
      } else {
        if (settingMinutes == 0)
        {
          settingMinutes = 60;
        } else {
          settingMinutes--;
        }
      }
      break;
    case SECONDS:
      if (action == INCREMENT) {
        if (settingSeconds == 60) {
          settingSeconds = 0;
        } else {
          settingSeconds++;
        }
      } else {
        if (settingSeconds == 0)
        {
          settingSeconds = 60;
        } else {
          settingSeconds--;
        }
      }
      break;
  }
}

void resetSettingTime () {
  settingHours = 0;
  settingMinutes = 0;
  settingSeconds = 0;
}

void setAlarm () {
  alarmSet = true;
  alarmHours = settingHours;
  alarmMinutes = settingMinutes;
  alarmSeconds = settingSeconds;
}

void checkAlarmTime () {
  if (alarmHours == 7 && alarmMinutes == 0 
      && alarmSeconds == 0) {
  	digitalWrite(buzzer, HIGH);
  }
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