////////////////////////////////////////////////////////
//                             TODO
//
// - trigger all servos for last petal drop? -> big power draw
//
////////////////////////////////////////////////////////

#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#include <Adafruit_NeoPixel.h>

#define PIN 2 // Pin where NeoPixels are connected
#define LEDS 60
#define SPOTLIGHT 5
#define FLICKER 3

Adafruit_NeoPixel strip(60, PIN, NEO_GRB + NEO_KHZ800);

#include <ArduinoBLE.h>

#include <list>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void all_servos_trigger(int dly = 0);
void fade_all(bool fade_up, int step = 3, int dly = 30);

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
    //all_servos_angle(0);
    //delay(5000);
    //all_servos_angle(90);
    //fade_all(true);
    //delay(2000);
    //fade_all(false);
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
                switch (switchCharacteristic.value())
                {
                case 1:
                    colorWipe(strip.Color(161, 27, 87), 20);
                    break;
                case 2:
                    colorWipe(strip.Color(0, 255, 0), 20); // Green
                    break;
                case 3:
                    colorWipe(strip.Color(0, 0, 255), 20); // Blue
                    break;
                case 4:
                    theaterChase(strip.Color(255, 0, 0), 20); // Red
                    break;
                case 5:
                    fade_all(true);
                    break;
                case 6:
                    fade_all(true);
                    all_servos_trigger(1500);
                    fade_all(false);
                    break;
                case 7:
                    all_servos_trigger(1500);
                    break;
                case 8:
                    fade(FLICKER, 0, 255, 5, 30);
                    break;
                case 9:
                    fade(SPOTLIGHT, 0, 255, 5, 30);
                    break;
                case 0:
                    fade_all(false);
                    all_servos_reset();
                    break;
                }
            }
        }
    }
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait)
{
    for (int i = 0; i < strip.numPixels(); i++)
    {                                  // For each pixel in strip...
        strip.setPixelColor(i, color); //  Set pixel's color (in RAM)
        strip.show();                  //  Update strip to match
        delay(wait);                   //  Pause for a moment
    }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait)
{
    for (int a = 0; a < 10; a++)
    { // Repeat 10 times...
        for (int b = 0; b < 3; b++)
        {                  //  'b' counts from 0 to 2...
            strip.clear(); //   Set all pixels in RAM to 0 (off)
            // 'c' counts up from 'b' to end of strip in steps of 3...
            for (int c = b; c < strip.numPixels(); c += 3)
            {
                strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
            }
            strip.show(); // Update strip with new contents
            delay(wait);  // Pause for a moment
        }
    }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait)
{
    // Hue of first pixel runs 5 complete loops through the color wheel.
    // Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
    // means we'll make 5*65536/256 = 1280 passes through this outer loop:
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
    {
        for (int i = 0; i < strip.numPixels(); i++)
        { // For each pixel in strip...
            // Offset pixel hue by an amount to make one full revolution of the
            // color wheel (range of 65536) along the length of the strip
            // (strip.numPixels() steps):
            int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
            // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
            // optionally add saturation and value (brightness) (each 0 to 255).
            // Here we're using just the single-argument hue variant. The result
            // is passed through strip.gamma32() to provide 'truer' colors
            // before assigning to each pixel:
            strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
        }
        strip.show(); // Update strip with new contents
        delay(wait);  // Pause for a moment
    }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait)
{
    int firstPixelHue = 0; // First pixel starts at red (hue 0)
    for (int a = 0; a < 30; a++)
    { // Repeat 30 times...
        for (int b = 0; b < 3; b++)
        {                  //  'b' counts from 0 to 2...
            strip.clear(); //   Set all pixels in RAM to 0 (off)
            // 'c' counts up from 'b' to end of strip in increments of 3...
            for (int c = b; c < strip.numPixels(); c += 3)
            {
                // hue of pixel 'c' is offset by an amount to make one full
                // revolution of the color wheel (range 65536) along the length
                // of the strip (strip.numPixels() steps):
                int hue = firstPixelHue + c * 65536L / strip.numPixels();
                uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
                strip.setPixelColor(c, color);                       // Set pixel 'c' to value 'color'
            }
            strip.show();                // Update strip with new contents
            delay(wait);                 // Pause for a moment
            firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
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
        map(161, 0, 255, 0, value),
        map(27, 0, 255, 0, value),
        map(87, 0, 255, 0, value));
    for (int i = 0; i < strip.numPixels(); i++) {                                  // For each pixel in strip...
        strip.setPixelColor(i, strip_color); //  Set pixel's color (in RAM)
    }
    strip.show();

    delay(dly);
}

void fade_all(bool fade_up, int step, int dly) {
    static bool lights_up = false;
    if (fade_up == lights_up) return;
    lights_up = fade_up;

    if (fade_up) {
        for (int i = 0; i <= 255; i += step) fade_all_guts(i, dly);
    } else {
        for (int i = 255; i >= 0; i -= step) fade_all_guts(i, dly);
    }
}

void servo_angle(int servo, int angle) {
    int pulselen = map(angle, 0, 90, 1000, 2000);
    pwm.writeMicroseconds(servo, pulselen);
}

std::list<std::list<int>> servos = 
    //{{6, 4}, {3}, {5}, {0}, {1}, {2}};
    //{{5, 6}, {2}, {4}, {1}, {3}, {0}};
    //{{2,5}, {1}, {6}, {4}, {3}, {0, 1, 2, 3, 4, 5, 6}};
    {{6}, {2, 5}, {1}, {4}, {3}, {0}}; // <=== best so far
    //{{1}, {2}, {5, 6}, {4}, {3}, {0}};

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
    for (auto some_servos : servos) {
        for (int servo : some_servos) {
            servo_reset(servo);
        }
    }
}

void all_servos_trigger(int dly) {
    for (auto some_servos : servos) {
        for (int servo : some_servos) {
            servo_trigger(servo);
        }
        int pause = 1000;
        delay(pause);
        for (int servo : some_servos) {
            servo_reset(servo);
        }
        int delay_left = dly - pause;
        if (delay_left > 0) delay(dly);
    }
}
