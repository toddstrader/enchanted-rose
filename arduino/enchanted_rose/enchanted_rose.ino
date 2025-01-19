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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  onboardLed.init();
}

void handleSerial() {
  if (!Serial) return;
  Serial.println("hi there");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(3000);
  handleSerial();
  onboardLed.toggle();
}
