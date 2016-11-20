#include <RadioReceiverTEA5767.h>
#include <Arduino.h>
#include <Wire.h>

RadioReceiverTEA5767::RadioReceiverTEA5767()
        : RadioReceiver(), frequency(0) {
}

void RadioReceiverTEA5767::initialize() {
    unsigned char raw[] = { 0x00, 0x00, 0xb0, 0x10, 0x00 };
    Wire.begin();
    read();
    setRawConfiguration(raw);
}

void RadioReceiverTEA5767::setFrequency(long frequency) {
    this->frequency = frequency;
    applyFrequency();
    autoAjustSideInjection();
}

long RadioReceiverTEA5767::getFrequency() {
    return frequency;
}

void RadioReceiverTEA5767::setStation(float station) {
    setFrequency(stationToFrequency(station));
}

float RadioReceiverTEA5767::getStation() {
    return frequencyToStation(frequency);
}

void RadioReceiverTEA5767::setStereo(bool stereo) {
    w3rd.MS = stereo;
    flush();
}

bool RadioReceiverTEA5767::getStereo() {
    return w3rd.MS;
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
    switch (side) {
    case SIDE_LEFT:
        w3rd.ML = mute;
        break;
    case SIDE_RIGHT:
        w3rd.MR = mute;
        break;
    case SIDE_BOTH:
        w1st.MUTE = mute;
        break;
    }
    flush();
}

void RadioReceiverTEA5767::setSearchDirection(SerachDirection direction) {
    w3rd.SUD = direction;
    flush();
}

void RadioReceiverTEA5767::setStandby(bool standby) {
    w4th.STBY = standby;
    flush();
}

void RadioReceiverTEA5767::setSearchStopLevel(SearchStopLevel level) {
    w3rd.SSL = level;
    flush();
}

RadioReceiverTEA5767::SearchStopLevel RadioReceiverTEA5767::getSearchStopLevel() {
    return (SearchStopLevel) w3rd.SSL;
}

unsigned char RadioReceiverTEA5767::getSignalLevel() {
    read();
    return r4th.LEV;
}

void RadioReceiverTEA5767::setSearchMode(bool mode) {
    w1st.SM = mode;
    flush();
}

long RadioReceiverTEA5767::searchNextFrequency() {
    long nextFrequency = 0;
    long oneGridStep = w3rd.SUD == SD_UP ? RADIO_RECEIVER_TEA5767_ONE_GRID_STEP : -(RADIO_RECEIVER_TEA5767_ONE_GRID_STEP);
    mute();
    setFrequency(frequency + oneGridStep);
    setSearchMode(true);
    while (!isReady())
        ;
    if (!isBandLimitReached()) {
        nextFrequency = getFoundStationFrequency();
    }
    setFrequency(frequency - oneGridStep);
    unmute();
    return nextFrequency;
}

void RadioReceiverTEA5767::setSideInjection(SideInjection level) {
    w3rd.HLSI = level;
    flush();
}

void RadioReceiverTEA5767::autoAjustSideInjection() {
    unsigned char signal;
    setSideInjection(SI_LOW);
    signal = getSignalLevel();
    setSideInjection(SI_HIGH);
    if (signal > getSignalLevel()) {
        setSideInjection(SI_LOW);
    }
}

bool RadioReceiverTEA5767::isBandLimitReached() {
    read();
    return r1st.BLF;
}

bool RadioReceiverTEA5767::isReady() {
    read();
    return r1st.RF;
}

float RadioReceiverTEA5767::getFoundStation() {
    return frequencyToStation(getFoundStationFrequency());
}

long RadioReceiverTEA5767::getFoundStationFrequency() {
    unsigned int phaseLockedLoop = 0;
    phaseLockedLoop = r1st.PLL;
    phaseLockedLoop <<= 8;
    phaseLockedLoop |= r2nd.PLL;
    return phaseLockedLoopToFrequency(phaseLockedLoop);
}

void RadioReceiverTEA5767::setRawConfiguration(unsigned char *buf) {
    unsigned char* values[] = { &w1st.value, &w2nd.value, &w3rd.value, &w4th.value, &w5th.value };
    for (unsigned char i = 0; i < RADIO_RECEIVER_TEA5767_BYTES_COUNT; i++) {
        *values[i] = buf[i];
    }
    flush();
}

long RadioReceiverTEA5767::stationToFrequency(float station) {
    return station * RADIO_RECEIVER_TEA5767_STATION_TO_FREQ;
}

float RadioReceiverTEA5767::frequencyToStation(long frequency) {
    return frequency / RADIO_RECEIVER_TEA5767_STATION_TO_FREQ;
}

long RadioReceiverTEA5767::phaseLockedLoopToFrequency(unsigned int phaseLockedLoop) {
    return (phaseLockedLoop * RADIO_RECEIVER_TEA5767_REF_FREQ) / 4 - RADIO_RECEIVER_TEA5767_INT_FREQ;
}

unsigned int RadioReceiverTEA5767::frequencyToPhaseLockedLoop(long frequency) {
    return 4 * (frequency + RADIO_RECEIVER_TEA5767_INT_FREQ) / RADIO_RECEIVER_TEA5767_REF_FREQ;
}

void RadioReceiverTEA5767::applyFrequency() {
    unsigned int phaseLockedLoop = frequencyToPhaseLockedLoop(frequency);
    w1st.PLL = (phaseLockedLoop >> 8) & 0x3f;
    w2nd.PLL = phaseLockedLoop & 0xff;
    flush();
}

void RadioReceiverTEA5767::read() {
    unsigned char* input[] = { &r1st.value, &r2nd.value, &r3rd.value, &r4th.value, &r5th.value };
    Wire.requestFrom(RADIO_RECEIVER_TEA5767_I2C_ADDRESS, RADIO_RECEIVER_TEA5767_BYTES_COUNT);
    if (Wire.available()) {
        for (unsigned char i = 0; i < RADIO_RECEIVER_TEA5767_BYTES_COUNT; i++) {
            *input[i] = Wire.read();
        }
    }
    delay(10);
}

void RadioReceiverTEA5767::flush() {
    unsigned char output[] = { w1st.value, w2nd.value, w3rd.value, w4th.value, w5th.value };
    Wire.beginTransmission(RADIO_RECEIVER_TEA5767_I2C_ADDRESS);
    for (unsigned char i = 0; i < RADIO_RECEIVER_TEA5767_BYTES_COUNT; i++) {
        Wire.write(output[i]);
    }
    Wire.endTransmission();
    delay(10);
}
