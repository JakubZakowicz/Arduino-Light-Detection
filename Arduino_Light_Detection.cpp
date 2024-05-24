#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int buzzer = 8;
const int sensorPin = A0;

const int button1 = 6;
const int button2 = 7;
const int button3 = 9;
const int button4 = 10;

const int debounceDelay = 200;
int lastDebounceTime = 0;

bool alarmSet = false;
int alarmHours;
int alarmMinutes;
int alarmSeconds;

int settingHours = 0;
int settingMinutes = 0;
int settingSeconds = 0;

enum Modes {
  MEASUREMENTS = 0,
  ALARMS = 1,
  MAIN_MENU = 2,
  ALARM_SETTING = 3,
  LIGHT_INTENSITY_SETTING = 4,
  RINGING = 5
};

enum SettingTimeUnits {
  HOURS = 0,
  MINUTES = 1,
  SECONDS = 2
};

enum SettingActions {
  INCREMENT = 0,
  DECREMENT = 1
};

Modes mode = MEASUREMENTS;
SettingTimeUnits settingTimeUnit = HOURS;

bool lightDetectionSet = false;
int sensor;
int alarmLightIntensity;
int settingLightIntensity = 50;

bool alarmRinging = false;

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  displayMeasurements();
}

void loop() {
  if (mode == MEASUREMENTS) handleMeasurements();
  if (mode == ALARMS) handleAlarms();
  if (mode == MAIN_MENU) handleMainMenu();
  if (mode == ALARM_SETTING) handleSetAlarm();
  if (mode == LIGHT_INTENSITY_SETTING) handleSetLightIntensity();
  if (mode == RINGING) {
  	handleRinging();
  }
  
  if (!alarmRinging) {
    if (alarmSet) checkAlarmTime();
    if (lightDetectionSet) checkLightIntensity();
  }
  
}

void handleMeasurements () {
  if (buttonRead(button1)) goToMainMenu();
  if (buttonRead(button2)) goToAlarms();
}

void goToMeasurements() {
  mode = MEASUREMENTS;
  displayMeasurements();
}

void displayMeasurements () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: 12:45:00");
  lcd.setCursor(0, 1);
  lcd.print("Light: 60%");
}

void handleAlarms () {
  if (buttonRead(button1)) goToMeasurements();
  if (buttonRead(button2)) goToMainMenu();
  
  if (buttonRead(button3)) {
    if (alarmSet) {
      removeTimeAlarm();
      displayAlarms();
    }
  }
  
  if (buttonRead(button4)) {
    if (lightDetectionSet) {
      removeLightAlarm();
      displayAlarms();
    }
  }
}

void goToAlarms () {
  mode = ALARMS;
  displayAlarms();
}

void displayAlarms () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm: ");
  if (alarmSet) {
  	displayTime(alarmHours, alarmMinutes, alarmSeconds);
  } else {
  	lcd.print("NOT SET");
  }
  lcd.setCursor(0, 1);
  lcd.print("Light Alarm: ");
  if (lightDetectionSet) {
  	lcd.print(alarmLightIntensity);
    lcd.print("%");
  } else {
  	lcd.print("NS");
  }
}

void removeTimeAlarm () {
  // continue here
  alarmSet = false;
  alarmHours = NULL;
  alarmMinutes = NULL;
  alarmSeconds = NULL;
}

void removeLightAlarm() {
  // continue here
  lightDetectionSet = false;
  alarmLightIntensity = NULL;
}

void handleMainMenu () {
  if (buttonRead(button1)) goToAlarms();
  if (buttonRead(button2)) goToMeasurements();
  
  if (buttonRead(button3)) {
    goToSetAlarm();
    resetSettingTime();
  }
  
  if (buttonRead(button4)) {
    goToSetLightIntensity();
    settingLightIntensity = 50;
  }
}

void goToMainMenu () {
  mode = MAIN_MENU;
  displayMainMenu();
}

void displayMainMenu () {
  lcd.clear();
  lcd.print("1.Set Alarm");
  lcd.setCursor(0, 1);
  lcd.print("2.Set light");
}

void handleSetAlarm () {  
  switch (settingTimeUnit) {
    case HOURS:
      if (buttonRead(button1)) {
        settingTimeUnit = MINUTES;
        displaySetAlarm();
      }

      if (buttonRead(button2)) {
        settingTimeUnit = HOURS;
        goToMainMenu();
      }

      if (buttonRead(button3)) {
        executeTimeAction(HOURS, INCREMENT);
        displaySetAlarm();
      }

      if (buttonRead(button4)) {
        executeTimeAction(HOURS, DECREMENT);
        displaySetAlarm();
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
      }

      if (buttonRead(button4)) {
        executeTimeAction(MINUTES, DECREMENT);
        displaySetAlarm();
      }
    
      break;
    case SECONDS:
      if (buttonRead(button1)) {
        setAlarm();
        settingTimeUnit = HOURS;
        goToMainMenu();
      }

      if (buttonRead(button2)) {
        settingTimeUnit = MINUTES;
        displaySetAlarm();
      }

      if (buttonRead(button3)) {
        executeTimeAction(SECONDS, INCREMENT);
        displaySetAlarm();
      }

      if (buttonRead(button4)) {
        executeTimeAction(SECONDS, DECREMENT);
        displaySetAlarm();
      }

      break;
    }   
}

void goToSetAlarm() {
  mode = ALARM_SETTING;
  displaySetAlarm();
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
  displayTime(settingHours, settingMinutes, settingSeconds);
}

void displayTime (int hours, int minutes, int seconds) {
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

void handleSetLightIntensity () {
  if (buttonRead(button1)) {
    setLightIntensityAlarm();
  	goToMainMenu();
  }
  
  if (buttonRead(button2)) {
    goToMainMenu();
  }
  
  if (buttonRead(button3)) {
    executeLightIntensityAction(INCREMENT);
    displaySetLightIntensity();
  }
  
  if (buttonRead(button4)) {
  	executeLightIntensityAction(DECREMENT);
    displaySetLightIntensity();
  }
}

void goToSetLightIntensity () {
  mode = LIGHT_INTENSITY_SETTING;
  displaySetLightIntensity();
}

void displaySetLightIntensity () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Light intensity:");
  lcd.setCursor(6, 1);
  displayLightIntensity();
}

void displayLightIntensity () {
   if (settingLightIntensity < 10) {
    lcd.print("0");
    lcd.print(settingLightIntensity);
  } else {
  	lcd.print(settingLightIntensity);
  }
  lcd.print("%");
}

void executeLightIntensityAction (int action) {
  if (action == INCREMENT) {
  	if (settingLightIntensity == 100) {
      settingLightIntensity = 0;
    } else {
      settingLightIntensity++;
    }
  } else {
    if (settingLightIntensity == 0) {
      settingLightIntensity = 100;
    } else {
      settingLightIntensity--;
    } 
  }
}

void setLightIntensityAlarm () {
  lightDetectionSet = true;
  alarmLightIntensity = settingLightIntensity;
}

void handleRinging () {
  if (buttonRead(button1) || buttonRead(button2) 
      || buttonRead(button3) || buttonRead(button4)) {
    digitalWrite(buzzer, LOW);
    if (alarmSet) removeTimeAlarm();
    if (lightDetectionSet) removeLightAlarm();
    
    alarmRinging = false;
    
    goToMeasurements();
  }
}

void goToRinging () {
  mode = RINGING;
  displayRinging();
  alarmRinging = true;
}

void displayRinging () {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALARM!!!!");
}

void checkAlarmTime () {
  if (alarmHours == 7 && alarmMinutes == 0 && alarmSeconds == 0) {
  	digitalWrite(buzzer, HIGH);
    goToRinging();
  }
}

void checkLightIntensity () {
  sensor = analogRead(sensorPin);
  if(sensor > alarmLightIntensity * 6.8){
  	digitalWrite(buzzer, HIGH);
    goToRinging();
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