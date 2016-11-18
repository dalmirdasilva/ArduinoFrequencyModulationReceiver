#include <RadioReceiverTEA5767.h>
#include <Arduino.h>
#include <Wire.h>

RadioReceiverTEA5767::RadioReceiverTEA5767()
        : RadioReceiver(), frequency(0) {
}

void RadioReceiverTEA5767::initialize() {
    Wire.begin();
    aw.value = 0x00;
    bw.value = 0x00;
    cw.value = 0xb0;
    dw.value = 0x10;
    ew.value = 0x00;
    flush();
    read();
}

void RadioReceiverTEA5767::setFrequency(long frequency) {
    this->frequency = frequency;
    applyFrequency();
}

long RadioReceiverTEA5767::getFrequency() {
    return frequency;
}

void RadioReceiverTEA5767::setStation(float station) {
    setFrequency(station * RADIO_RECEIVER_TEA5767_STATION_TO_FREQ);
}

float RadioReceiverTEA5767::getStation() {
    return frequency / RADIO_RECEIVER_TEA5767_STATION_TO_FREQ;
}

void RadioReceiverTEA5767::setStereo(bool stereo) {
    cw.MS = stereo;
    flush();
}

bool RadioReceiverTEA5767::getStereo() {
    return cw.MS;
}

void RadioReceiverTEA5767::mute() {
    mute(SIDE_BOTH);
}

void RadioReceiverTEA5767::mute(Side side) {
    setMute(side, 1);
}

void RadioReceiverTEA5767::unmute() {
    unmute(SIDE_BOTH);
}

void RadioReceiverTEA5767::unmute(Side side) {
    setMute(side, 0);
}

void RadioReceiverTEA5767::setMute(Side side, bool mute) {
    if (side & SIDE_LEFT) {
        cw.ML = mute;
    }
    if (side & SIDE_RIGHT) {
        cw.ML = mute;
    }
    flush();
}

void RadioReceiverTEA5767::setSearchDirection(SerachDirection direction) {
    cw.SUD = direction;
    flush();
}

void RadioReceiverTEA5767::setStandby(bool standby) {
    dw.STBY = standby;
    flush();
}

void RadioReceiverTEA5767::setSearchStopLevel(SearchStopLevel level) {
    cw.SSL = level;
    flush();
}

RadioReceiverTEA5767::SearchStopLevel RadioReceiverTEA5767::getSearchStopLevel() {
    return (SearchStopLevel) cw.SSL;
}

void RadioReceiverTEA5767::startSearchMode() {
    setSearchMode(true);
}

void RadioReceiverTEA5767::stopSearchMode() {
    setSearchMode(false);
}

void RadioReceiverTEA5767::setSearchMode(bool mode) {
    aw.SM = mode;
    flush();
}

void RadioReceiverTEA5767::setSideInjection(SideInjection level) {
    cw.HLSI = level;
    flush();
}

void RadioReceiverTEA5767::setRawConfiguration(unsigned char *buf) {
    unsigned char* values[] = { &aw.value, &bw.value, &cw.value, &dw.value, &ew.value };
    for (unsigned char i = 0; i < RADIO_RECEIVER_TEA5767_BYTES_COUNT; i++) {
        *values[i] = buf[i];
    }
    flush();
}

void RadioReceiverTEA5767::applyFrequency() {
    long pll = 4 * (frequency + RADIO_RECEIVER_TEA5767_INT_FREQ) / RADIO_RECEIVER_TEA5767_REF_FREQ;
    aw.PLL = (pll >> 8) & 0x3f;
    Serial.println(aw.PLL, HEX);
    bw.PLL = pll & 0xff;
    Serial.println(bw.PLL, HEX);
    flush();
}

void RadioReceiverTEA5767::read() {
    unsigned char* input[] = { &ar.value, &br.value, &cr.value, &dr.value, &er.value };
    Wire.requestFrom(RADIO_RECEIVER_TEA5767_I2C_ADDRESS, RADIO_RECEIVER_TEA5767_BYTES_COUNT);
    if (Wire.available()) {
        for (unsigned char i = 0; i < RADIO_RECEIVER_TEA5767_BYTES_COUNT; i++) {
            *input[i] = Wire.read();
        }
    }
    delay(10);
}

void RadioReceiverTEA5767::flush() {
    Serial.println("send");
    unsigned char output[] = { aw.value, bw.value, cw.value, dw.value, ew.value };
    Wire.beginTransmission(RADIO_RECEIVER_TEA5767_I2C_ADDRESS);
    for (unsigned char i = 0; i < RADIO_RECEIVER_TEA5767_BYTES_COUNT; i++) {
        Serial.println(output[i], HEX);
        Wire.write(output[i]);
    }
    Wire.endTransmission();
    delay(10);
}
