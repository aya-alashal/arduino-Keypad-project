#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo gateServo;
const int servoPin = 9;
const int buzzerPin = 10; 

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, A0};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const String correctPasscode = "1234"; 
String inputPasscode = "";
bool isDoorOpen = false;
unsigned long openTime = 0;
const unsigned long doorOpenDuration = 5000;

void resetScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter PIN:");
  lcd.setCursor(0, 1);
  inputPasscode = "";
}

void setup() {
  lcd.init();
  lcd.backlight();
  
  gateServo.attach(servoPin);
  gateServo.write(0);
  
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  resetScreen();
}

void loop() {
  unsigned long currentTime = millis();

  if (isDoorOpen && (currentTime - openTime >= doorOpenDuration)) {
    gateServo.write(0);
    isDoorOpen = false;
    resetScreen();
  }

  char key = keypad.getKey();

  if (key && !isDoorOpen) {
    if (key == '#') { 
      if (inputPasscode == correctPasscode) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Welcome!");
        
        gateServo.write(90);
        isDoorOpen = true;
        openTime = millis();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied!");
        
        tone(buzzerPin, 1000, 800);
        delay(1000);
        noTone(buzzerPin);
        
        resetScreen();
      }
    } 
    else if (key == '*') { 
      resetScreen();
    } 
    else if (inputPasscode.length() < 4) { 
      inputPasscode += key;
      lcd.setCursor(inputPasscode.length() - 1, 1);
      lcd.print("*");
    }
  }
}