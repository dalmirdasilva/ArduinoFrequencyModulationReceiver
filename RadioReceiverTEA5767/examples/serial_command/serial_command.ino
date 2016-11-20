#include <RadioReceiverTEA5767.h>
#include <Arduino.h>

RadioReceiverTEA5767 receiver;
float stations[] = {107.1, 95.9, 95.9};

void printHelp() {
    Serial.println("u: station up 0.1");
    Serial.println("d: station down 0.1");
    Serial.println("s: standby");
    Serial.println("h: side injection high");
    Serial.println("l: side injection low");
    Serial.println("a: auto ajustSide injection");
    Serial.println("U: auto search up");
    Serial.println("D: auto search down");
    Serial.println("c: current station");
    Serial.println("S: signal level");
    Serial.println("1: station 1");
    Serial.println("2: station 2");
    Serial.println("3: station 3");
}

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing radio...");
    receiver.initialize();
    receiver.setSearchStopLevel(RadioReceiverTEA5767::SSL_MID);
    receiver.setStation(stations[0]);
    printHelp();
    Serial.println("Done.");
}

bool standby = false;

void loop() {
    long nextFrequency;
    if (Serial.available()) {
        char c = Serial.read();
        switch (c) {
        case 'u':
            receiver.setStation(receiver.getStation() + 0.1);
            Serial.println(receiver.getStation());
            break;
        case 'd':
            receiver.setStation(receiver.getStation() - 0.1);
            Serial.println(receiver.getStation());
            break;
        case 's':
            receiver.setStandby(standby = !standby);
            break;
        case 'h':
            receiver.setSideInjection(RadioReceiverTEA5767::SI_HIGH);
            break;
        case 'l':
            receiver.setSideInjection(RadioReceiverTEA5767::SI_LOW);
            break;
        case 'a':
            receiver.autoAjustSideInjection();
            break;
        case 'D':
            receiver.setSearchDirection(RadioReceiverTEA5767::SD_DOWN);
            receiver.searchNextFrequency();
            nextFrequency = receiver.searchNextFrequency();
            if (nextFrequency > 0) {
                receiver.setFrequency(nextFrequency);
            }
            Serial.println(receiver.frequencyToStation(nextFrequency));
            break;
        case 'U':
            receiver.setSearchDirection(RadioReceiverTEA5767::SD_UP);
            nextFrequency = receiver.searchNextFrequency();
            if (nextFrequency > 0) {
                receiver.setFrequency(nextFrequency);
            }
            Serial.println(receiver.frequencyToStation(nextFrequency));
            break;
        case 'c':
            Serial.println(receiver.getStation());
            break;
        case 'S':
            Serial.println(receiver.getSignalLevel());
            break;
        case '1':
        case '2':
        case '3':
            receiver.setStation(stations[c - '1' ]);
            break;
        case 'H':
            printHelp();
            break;
        }
    }
}