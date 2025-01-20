#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

typedef enum ActiveLevel {ACTIVE_LOW = 0, ACTIVE_HIGH} ActiveLevel;
typedef enum OnOff {OFF = 0, ON} OnOff;

class Led {
  public:
    Led(int _pin, OnOff initialState = OFF,
        ActiveLevel _activeLevel = ACTIVE_HIGH)
      : pin{_pin}, isOn{initialState}, activeLevel{_activeLevel} {}
    void init() {
      pinMode(pin, OUTPUT);
      write();
    }
    void on() {
      isOn = true;
      write();
    }
    void off() {
      isOn = false;
      write();
    }
    void toggle() {
      isOn = !isOn;
      write();
    }

  private:
    void write() {
      int value = isOn;
      if (activeLevel == ACTIVE_LOW) isOn = !isOn;
      digitalWrite(pin, isOn);
    }

    int pin;
    bool isOn;
    ActiveLevel activeLevel;
};

Led onboardLed(13);
Led spotlight(5);
Led flickerLeds(3);

class SerialInterface {
  public:
    void init() {
      Serial.begin(9600);
      Serial.setTimeout(100);
    }

    void handleSerial() {
      if (!Serial) return;
      if (first) {
        printHelp();
        first = false;
      }
      int command = Serial.parseInt();
      Serial.setTimeout(10000);
      switch(command) {
        case 1:
          setServoAngle();
          break;
        case 2:
          spotlight.toggle();
          break;
        case 3:
          flickerLeds.toggle();
          break;
        case -1:
          printHelp();
          break;
      }
      Serial.setTimeout(100);
    }

  private:
    void printHelp() {
      Serial.println("Commands:");
      Serial.println("-1 -- this help");
      Serial.println("1  -- set servo angle");
      Serial.println("2  -- toggle spotlight");
      Serial.println("3  -- toggle flicker LEDs");
    }

    void setServoAngle() {
      Serial.println("Enter servo number (0-7):");
      int servo = Serial.parseInt();
      if (servo < 0 || servo > 7) {
        Serial.println("bad servo number");
        return;
      }

      Serial.println("Enter servo angle (0-90)");
      int angle = Serial.parseInt();
      if (angle < 0 || angle > 90) {
        Serial.println("bad angle");
        return;
      }
      Serial.println("--->");
      Serial.println(servo);
      Serial.println(angle);
      int pulselen = map(angle, 0, 90, 1000, 2000);
      Serial.println(pulselen);
      pwm.writeMicroseconds(servo, pulselen);
    }

    bool first = true;
};

SerialInterface serialInterface;

void setup() {
  serialInterface.init();
  onboardLed.init();
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
}


void loop() {
  delay(500);
  serialInterface.handleSerial();
  onboardLed.toggle();
}