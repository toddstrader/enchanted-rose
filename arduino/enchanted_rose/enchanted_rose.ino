#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    60
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

#include <ArduinoBLE.h>

BLEService ledService("180A"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);

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
bool stripOn = false;

void toggleStrip() {
  stripOn = !stripOn;
  if (!stripOn) FastLED.clear();
}

class SerialInterface {
  public:
    void init() {
      Serial.begin(9600);
      Serial.setTimeout(1);
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
        case 4:
          toggleStrip();
          break;
        case -1:
          printHelp();
          break;
      }
      Serial.setTimeout(1);
    }

  private:
    void printHelp() {
      Serial.println("Commands:");
      Serial.println("-1 -- this help");
      Serial.println("1  -- set servo angle");
      Serial.println("2  -- toggle spotlight");
      Serial.println("3  -- toggle flicker LEDs");
      Serial.println("4  -- toggle LED strip");
      Serial.println("micros:");
      Serial.println(micros());
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
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  FastLED.clear(true);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Enchanted Rose");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  delay(100);
}

CRGBPalette16 palette(
    CRGB::Amethyst,
    CRGB::Amethyst,
    CRGB::Amethyst,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::Amethyst,
    CRGB::White,
    CRGB::Amethyst,
    CRGB::White,
    CRGB::Amethyst,
    CRGB::White,
    CRGB::Amethyst,
    CRGB::White,
    CRGB::Amethyst,
    CRGB::White
);
/*
CRGBPalette16 palette(
  CRGB::Purple,
  CRGB::Green,
  CRGB::Orange,
  CRGB::Pink,
  CRGB::Yellow,
  CRGB::Blue,
  CRGB::Bisque,
  CRGB::Teal,
  CRGB::Lavender,
  CRGB::Aqua,
  CRGB::White,
  CRGB::DarkGray,
  CRGB::LightCoral,
  CRGB::MediumAquamarine,
  CRGB::Maroon,
  CRGB::Lime
);*/

void FillLEDsFromPaletteColors( uint8_t colorIndex) {
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette(palette, colorIndex);
        colorIndex += 1;
    }
}

void guts() {
  unsigned long last = 0;

  serialInterface.handleSerial();
  // TODO -- not this
  onboardLed.toggle();

  static uint8_t startIndex = 0;
  if (stripOn) {
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors(startIndex);
  }
  // // TODO -- this plus BLE
  // // TODO -- does Serial slow this down? -- yes
  // FastLED.delay(1000 / UPDATES_PER_SECOND);
  unsigned now = micros();
  if (now - last > 1000 / UPDATES_PER_SECOND) {
    FastLED.show();
  }
  last = now;
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // doesn't appear to block
  // Serial.println("after constructor");  
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        switch (switchCharacteristic.value()) {   // any value other than 0
          case 01:
            toggleStrip();
            break;
          case 02:
            spotlight.toggle();
            break;
          case 03:
            flickerLeds.toggle();
            break;
        }
      }
      //delay(100);
      guts();
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }

  guts();
}