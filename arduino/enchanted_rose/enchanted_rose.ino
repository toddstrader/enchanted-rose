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
          spotlight.toggle();
          break;
        case 2:
          setServoAngle();
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
      Serial.println("1  -- toggle spotlight");
      Serial.println("2  -- set servo angle");
    }

    void setServoAngle() {
      Serial.println("Enter servo number:");
      int servo = Serial.parseInt();
      Serial.println("Enter servo angle (0-90)");
      int angle = Serial.parseInt();
      Serial.println("--->");
      Serial.println(servo);
      Serial.println(angle);
    }

    bool first = true;
};

SerialInterface serialInterface;

void setup() {
  serialInterface.init();
  onboardLed.init();
}


void loop() {
  delay(500);
  serialInterface.handleSerial();
  onboardLed.toggle();
}