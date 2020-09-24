// Derived from https://www.instructables.com/id/Interfacing-a-Digital-Micrometer-to-a-Microcontrol/

#include <Arduino.h>

// When defined, will send SPC data as hexadecimal over serial.
// #define HEX_DATA_OUTPUT

// When defined, will convert raw data to human readable string
// and send over serial.
#define HUMAN_READABLE_OUTPUT

#ifdef HUMAN_READABLE_OUTPUT
#ifndef HEX_DATA_OUTPUT
// When defined, will pretend to be a Fowler/Sylvak indicator
// connected with the USB proximity data cable. Follows observed
// behavior which is a little different from what was in the manual.
//  * Use serial port parameters 4800 7E2
//  * Will wait for an incoming byte before sending value
//    Response is always 10 bytes.
//  * Value always starts with a sign - positive or negative
//  * Next 8 characters will be the number
//    One of those characters is a decimal point
//  * Final (10th) character is the \r termination
//
//  * HEX_DATA_OUTPUT must NOT be defined
//  * HUMAN_READABLE_OUTPUT must be defined
#define FOWLER_PLACEHOLDER
#endif
#endif

int req = 5; //Arduino pin for REQ line, drives transistor to ground SPC port pin 5 (~REQ)
int dat = 2; //Arduino pin for Data line, connects directly to SPC port pin 2 (DATA)
int clk = 3; //Arduino pin for Clock line, connects directly to SPC port pin 3 (~CLK)

int i = 0;
int j = 0;
int k = 0;

// Mitutoyo SPC data port transmits 13 4-bit values
// Byte Meaning
// 0    Header, always 0xF
// 1    Header, always 0xF
// 2    Header, always 0xF
// 3    Header, always 0xF
// 4    Sign. Zero is positive. 0x8 is negative.
// 5    Most significant digit (MSD) of measurement
// 6    Next digit of measurement
// 7    Next digit of measurement
// 8    Next digit of measurement
// 9    Next digit of measurement
// 10   Least significant digit (LSD) of measurement
// 11   Digit to place decimal point
// 12   Unit. Zero is mm. Nonzero (b1000 is 1?) is inch

byte spcdata[13]; // The raw data sent by instrument

#ifdef HUMAN_READABLE_OUTPUT
float value;      // The value calculated from raw data
int decimal;      // Number of digits that are after decimal point
#endif

#ifdef FOWLER_PLACEHOLDER
byte readBuffer[1];
#endif

void setup() {
#ifdef FOWLER_PLACEHOLDER
    Serial.begin(4800, SERIAL_7E2);
    Serial.setTimeout(20); // Short timeout on read.
#else
    Serial.begin(9600);
#endif
    pinMode(req, OUTPUT);
    pinMode(clk, INPUT_PULLUP);
    pinMode(dat, INPUT_PULLUP);
    digitalWrite(req,LOW); // set request at high
}

void loop() {
#ifdef FOWLER_PLACEHOLDER
    while (Serial.available() < 1) {
        // Wait for a byte to be sent from computer
    }
    // What was sent appears to be irrelevant, so we never look at it.
    Serial.readBytes(readBuffer, 1);
#endif

    digitalWrite(req, HIGH); // generate set request

    for( i = 0; i < 13; i++ ) {
        k = 0;

        // Timing diagram indicates data bit has been valid for about 120 
        // microseconds before the clock signal is raised, and remains
        // valid for about 120 microseconds afterwards. This code reads data
        // bit at the falling clock edge.
        for (j = 0; j < 4; j++) {
            while( digitalRead(clk) == LOW) {
            } // hold until clock is high
            while( digitalRead(clk) == HIGH) {
            } // hold until clock is low
            bitWrite(k, j, (digitalRead(dat) & 0x1));
        }

        // After reading the first 4-bit value, we can drop REQ output.
        if (i == 0) {
            digitalWrite(req,LOW);
        }
        spcdata[i] = k;
    }

#ifdef HEX_DATA_OUTPUT
    for( i = 0; i < 13; i++ ) {
        Serial.print(spcdata[i],HEX);
    }
#ifdef HUMAN_READABLE_OUTPUT
    // Need a separator if we're printing human readable as well
    Serial.print(" ");
#endif // HUMAN_READABLE_OUTPUT
#endif //HEX_DATA_OUTPUT

#ifdef HUMAN_READABLE_OUTPUT
    // Combine measurement digits into a number
    value = 0;
    for( i = 5; i <= 10; i++) {
        value *= 10;
        value += spcdata[i];
    }

    // Adjust number for decimal point position
    decimal = spcdata[11];
    value /= pow(10,decimal);

    // Adjust if number is negative
    if (spcdata[4] == 0x8) {
        value *= -1;
    }

    if (value >= 0) {
        Serial.print("+");
    }
    // Print resulting value to serial port, to specified
    // number of digits after decimal point.
    Serial.print(value,decimal);
#ifdef FOWLER_PLACEHOLDER
    // Now we have to calculate how many zeros to pad out the end.
    // NOTE: This breaks down if value is >=100 or <=-100
    //      (Only works with one or two digits before decimal point.)
    int zeroPadding = 5-decimal;

    if (value < 10 && value > -10) {
        // Extra zero padding for single digit before decimal.
        zeroPadding++;
    }
    for (int i = 0; i < zeroPadding; i++) {
        Serial.print("0");
    }
    Serial.print("\r");
#else
    // Append units for value
    if (spcdata[12] == 0) {
        Serial.print(" mm");
    } else {
        Serial.print(" in");
    }
#endif // FOWLER_PLACEHOLDER
#endif // HUMAN_READABLE_OUTPUT

#ifndef FOWLER_PLACEHOLDER
    Serial.println();
#endif
}
