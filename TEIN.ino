#include <LiquidCrystal.h>
#include <EEPROM.h>

//Set the pins
byte directionPin = 10;
byte buttonSelect = 6;
byte buttonEnter = 7;
byte buttonLight = 8;
byte lcdBacklight = 13;

int dialValue = 500;

boolean lock = true;
boolean displayOn = false;
boolean equal = true;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //----Placeholder pins, needs to change before testing----


class Selected {
  public:
    String state;
    String displayValue;
    int address;
    int value;
    byte pin;
    void runStep() {
      for (int i = 0; i < 16 * abs(dialValue-value); i++) { //----16 is a temporary number for testing purposes. The number is the amount of steps needed for 1 click on the adjusment screw----
        digitalWrite(pin, HIGH);
        delay(50);
        digitalWrite(pin, LOW);

      }
      EEPROM.update(address, value);
    }
    void lcdPrint() {
      lcd.setCursor(0, address);
      lcd.print(displayValue);
      lcd.print(value);
      lcd.print(" ");
    }
};

Selected current;
Selected front;
Selected rear;



void setup() {
  Serial.begin(9600);   //COMMENT OUT WHEN NOT USING PC, OTHERWISE PIN 0 AND 1 DOESNT WORK

  lcd.begin(16, 2);
  lcd.noDisplay();
  front.state = "Front";
  front.displayValue = "Fr:";
  front.address = 0;
  front.value = EEPROM.read(front.address);
  front.pin = 9;

  rear.state = " Rear";
  rear.displayValue = "Re:";
  rear.address = 1;
  rear.value = EEPROM.read(rear.address);
  rear.pin = 14;
  pinMode(front.pin, OUTPUT);
  pinMode(rear.pin, OUTPUT);
  pinMode(lcdBacklight, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(buttonEnter, INPUT_PULLUP);
  pinMode(buttonLight, INPUT_PULLUP);
  lcdDisplay();
  current = front;
}

void loop() {
  dialValue = analogRead(A0)/63,9375; //analogRead
  if (lock == true && dialValue == current.value) {
    printTemp(0, true);
    printTemp(1, true);
  }
  else if (lock == true) {
    printTemp(0, false);
    printTemp(1, false);
  }
  else if (current.pin == front.pin && dialValue != current.value) {
    printTemp(0, false);
    printTemp(1, true);
  }
  else if (current.pin == rear.pin && dialValue != current.value) {
    printTemp(1, false);
    printTemp(0, true);
  }
  else {
    printTemp(0, true);
    printTemp(1, true);
  }



  if (digitalRead(buttonSelect) == LOW) {
    selectButton();
  }
  else if (digitalRead(buttonEnter) == LOW) {
    enterButton();
  }
  else if (digitalRead(buttonLight) == LOW) {
    lightButton();
  }
  delay(50);
}

void printTemp(int i, boolean equal) {
  lcd.setCursor(6, i);
  if (equal) {
    lcd.print("    ");
  }
  else {
    lcd.print("(");
    lcd.print(dialValue);
    lcd.print(") ");
  }

}

void lcdDisplay() {
  if (lock == true) {
    front.lcdPrint();
    rear.lcdPrint();
  }
  else if (current.pin == front.pin) {
    front.lcdPrint();
  }
  else {
    rear.lcdPrint();
  }
  lcd.setCursor(11, 1);
  if (lock == true) {
    lcd.print(" Lock");
  }
  else {
    lcd.print(current.state);
  }
}

void selectButton() {
  if (lock == true) {
    current = front;
    lock = false;
    printTemp(1, false);
  }
  else if (current.pin == front.pin) {
    current = rear;
    printTemp(0, false);
  }
  else {
    lock = true;
    current = front;
  }
  lcdDisplay();
  delay(500);
}

void enterButton() {
  if (dialValue == current.value) {
    return;
  }
  else if (dialValue < current.value) {
    digitalWrite(directionPin, HIGH); //----Might need to be changed depending on the stepper control board----
  }
  else {
    digitalWrite(directionPin, LOW); //----Same for this one----
  }
  current.value = dialValue;
  setValue();
  lcdDisplay();
  delay(500);
}

void setValue() {
  if (lock == true) {
    front.runStep();
    front.value = current.value;
    rear.runStep();
    rear.value = current.value;
  }
  else if (current.pin == front.pin) {
    front.runStep();
    front.value = current.value;
  }
  else {
    rear.runStep();
    rear.value = current.value;
  }
}

void lightButton() {
  if (displayOn == true) {
    digitalWrite(lcdBacklight, LOW);
    lcd.noDisplay();
  }
  else {
    digitalWrite(lcdBacklight, HIGH);
    lcd.display();
  }
  displayOn = !displayOn;
  delay(500);
}
