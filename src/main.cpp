// Derived from https://www.instructables.com/id/Interfacing-a-Digital-Micrometer-to-a-Microcontrol/

#include <Arduino.h>

int req = 5; //mic REQ line goes to pin 5 through q1 (arduino high pulls request line low)
int dat = 2; //mic Data line goes to pin 2
int clk = 3; //mic Clock line goes to pin 3
int i = 0;
int j = 0;
int k = 0;
int signCh = 8;
int sign = 0;
int decimal;
float dpp;
int units;

byte mydata[14];
String value_str;
long value_int; //was an int, could not measure over 32mm
float value;

void setup() {
    Serial.begin(9600);
    pinMode(req, OUTPUT);
    pinMode(clk, INPUT_PULLUP);
    pinMode(dat, INPUT_PULLUP);
    digitalWrite(req,LOW); // set request at high
}

void loop() {
    digitalWrite(req, HIGH); // generate set request

    for( i = 0; i < 13; i++ ) {
        k = 0;
        for (j = 0; j < 4; j++) {
            while( digitalRead(clk) == LOW) {
            } // hold until clock is high
            while( digitalRead(clk) == HIGH) {
            } // hold until clock is low
            bitWrite(k, j, (digitalRead(dat) & 0x1));
        }
        mydata[i] = k;
    }

    sign = mydata[4];
    value_str = String(mydata[5]) + String(mydata[6]) + String(mydata[7]) + String(mydata[8] + String(mydata[9] + String(mydata[10]))) ;
    decimal = mydata[11];
    units = mydata[12];

    value_int = value_str.toInt();
    if (decimal == 0) dpp = 1.0;
    if (decimal == 1) dpp = 10.0;
    if (decimal == 2) dpp = 100.0;
    if (decimal == 3) dpp = 1000.0;
    if (decimal == 4) dpp = 10000.0;
    if (decimal == 5) dpp = 100000.0;

    value = value_int / dpp;

    if (sign == 0) {
        Serial.println(value,decimal);
    }
    if (sign == 8) {
        Serial.print("-"); Serial.println(value,decimal);
    }

    digitalWrite(req,LOW);
    delay(100);
}
