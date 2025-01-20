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
    }
    void handleSerial() {
      if (!Serial) return;
      if (Serial.peek() == -1) return;
      String command = Serial.readString();
      command.trim();
      printHelp();
      spotlight.toggle();
    }
  private:
    void printHelp() {
      Serial.println("0 -- toggle spotlight");
    }
};

SerialInterface serialInterface;

void setup() {
  serialInterface.init();
  onboardLed.init();
}


void loop() {
  delay(3000);
  serialInterface.handleSerial();
  onboardLed.toggle();
}