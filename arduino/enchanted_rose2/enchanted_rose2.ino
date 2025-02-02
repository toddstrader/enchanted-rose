////////////////////////////////////////////////////////
//                             TODO
//
////////////////////////////////////////////////////////

#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#include <Adafruit_NeoPixel.h>

#define PIN 2 // Pin where NeoPixels are connected
#define LEDS 50
#define SPOTLIGHT 5
#define FLICKER 3

Adafruit_NeoPixel strip(LEDS, PIN, NEO_GRB + NEO_KHZ800);

#include <ArduinoBLE.h>

#include <vector>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void all_servos_trigger(int dly = 0);
void fade_all(bool fade_up, int step = 3, int dly = 30);
void fade_all_toggle(int step = 3, int dly = 30);

std::vector<std::vector<int>> servos = 
    //{{6, 4}, {3}, {5}, {0}, {1}, {2}};
    //{{5, 6}, {2}, {4}, {1}, {3}, {0}};
    //{{2,5}, {1}, {6}, {4}, {3}, {0, 1, 2, 3, 4, 5, 6}};
    //{{6}, {2, 5}, {1}, {4}, {3}, {0}}; // cumulative servos is too much power
    //{{1}, {2}, {5, 6}, {4}, {3}, {0}};
    //{{6}, {2, 5, 6}, {1, 2, 5, 6}, {4, 1, 2, 5, 6}, {3, 4, 1, 2, 5, 6}, {0, 1, 2, 3, 4, 5, 6}}; // <=== better? -- yes
    {{6}, {2, 5, 6}, {1, 2, 5, 6}, {4}, {3, 4}, {0}}; // something in between
    //{{2}, {2, 1}, {5, 6, 2, 1}, {4, 5, 6, 2, 1}, {3, 4, 5, 6, 2, 1}, {0, 1, 2, 3, 4, 5, 6}};

void setup()
{
    Serial.begin(115200);
    Serial.println("Hello World!");

    // custom services and characteristics can be added as well
    // begin initialization
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed!");

        while (1)
            ;
    }

    Serial.print("Peripheral address: ");
    Serial.println(BLE.address());

    // set advertised local name and service UUID:
    BLE.setLocalName("LED");
    BLE.setAdvertisedService(ledService);

    // add the characteristic to the service
    ledService.addCharacteristic(switchCharacteristic);

    // add service
    BLE.addService(ledService);

    // set the initial value for the characeristic:
    switchCharacteristic.writeValue(0);

    // start advertising
    BLE.advertise();

    strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();  // Turn OFF all pixels ASAP

    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, LOW);

    pinMode(SPOTLIGHT, OUTPUT);
    digitalWrite(SPOTLIGHT, LOW);
    pinMode(FLICKER, OUTPUT);
    digitalWrite(FLICKER, LOW);

    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(50);
    all_servos_reset();
}

void loop()
{
    BLEDevice central = BLE.central();

    // if a central is connected to peripheral:
    if (central)
    {
        Serial.print("Connected to central: ");
        // print the central's MAC address:
        Serial.println(central.address());

        // while the central is still connected to peripheral:
        while (central.connected())
        {
            // if the remote device wrote to the characteristic,
            // use the value to control the LED:
            if (switchCharacteristic.written())
            {
                int value = switchCharacteristic.value();
                switch (value)
                {
                case 0xa:
                    fade_all(true);
                    all_servos_trigger(1500);
                    fade_all(false);
                    break;
                case 0xb:
                    all_servos_trigger(1500);
                    break;
                case 0xc:
                    fade(FLICKER, 0, 255, 5, 30);
                    break;
                case 0xd:
                    fade(SPOTLIGHT, 0, 255, 5, 30);
                    break;
                case 0:
                    fade_all_toggle();
                    break;
                default:
                    if (value > 0 && value <= 6) {
                        servos_trigger(servos[value-1]);
                        if (value == 6) fade_all(false);
                    }
                }
            }
        }
    }
}

void fadeGuts(int pin, int value, int dly) {
    analogWrite(pin, value);
    delay(dly);
}

void fade(int pin, int start, int stop, int step, int dly) {
    if (start < stop) {
      for (int i = start; i <= stop; i += step) fadeGuts(pin, i, dly);
    } else {
      for (int i = start; i >= stop; i -= step) fadeGuts(pin, i, dly);
    }
}

void fade_all_guts(int value, int dly) {
    analogWrite(SPOTLIGHT, value);
    analogWrite(FLICKER, value);
    uint32_t strip_color = strip.Color(
      // 161, 27, 87
        map(128, 0, 255, 0, value),
        map(14, 0, 255, 0, value),
        map(65, 0, 255, 0, value));
    for (int i = 0; i < strip.numPixels(); i++) {                                  // For each pixel in strip...
        strip.setPixelColor(i, strip_color); //  Set pixel's color (in RAM)
    }
    strip.show();

    delay(dly);
}

bool lights_up = false;

void fade_all(bool fade_up, int step, int dly) {
    if (fade_up == lights_up) return;
    lights_up = fade_up;

    if (fade_up) {
        for (int i = 0; i <= 255; i += step) fade_all_guts(i, dly);
    } else {
        for (int i = 255; i >= 0; i -= step) fade_all_guts(i, dly);
    }
}

void fade_all_toggle(int step, int dly) {
    fade_all(!lights_up, step, dly);
}

void servo_angle(int servo, int angle) {
    int pulselen = map(angle, 0, 90, 1000, 2000);
    pwm.writeMicroseconds(servo, pulselen);
}

void all_servos_angle(int angle) {
    for (auto some_servos : servos) {
        for (int servo : some_servos) {
            servo_angle(servo, angle);
        }
    }
}

void servo_reset(int servo) {
    int angle = servo % 2 ? 0 : 90;
    servo_angle(servo, angle);
}

void servo_trigger(int servo) {
    int angle = servo % 2 ? 90 : 0;
    servo_angle(servo, angle);
}

void all_servos_reset() {
    for (int servo = 0; servo < 7; servo++) {
        servo_reset(servo);
    }
}

int servo_pause = 1000;

void servos_trigger(std::vector<int>& some_servos) {
    for (int servo : some_servos) {
        servo_trigger(servo);
    }
    delay(servo_pause);
    for (int servo : some_servos) {
        servo_reset(servo);
    }
}

void all_servos_trigger(int dly) {
    for (auto some_servos : servos) {
        servos_trigger(some_servos);
        int delay_left = dly - servo_pause;
        if (delay_left > 0) delay(dly);
    }
}
