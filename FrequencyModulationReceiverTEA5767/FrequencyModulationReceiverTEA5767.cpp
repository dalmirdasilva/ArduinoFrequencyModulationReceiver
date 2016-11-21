#include <FrequencyModulationReceiverTEA5767.h>
#include <Arduino.h>
#include <Wire.h>

FrequencyModulationReceiverTEA5767::FrequencyModulationReceiverTEA5767()
        : FrequencyModulationReceiver(), frequency(0) {
}

void FrequencyModulationReceiverTEA5767::initialize() {
    unsigned char raw[] = { 0x00, 0x00, 0xb0, 0x10, 0x00 };
    Wire.begin();
    read();
    setRawConfiguration(raw);
}

void FrequencyModulationReceiverTEA5767::setFrequency(long frequency) {
    if (frequency < TEA5767_LOW_BAND_LIMIT_FREQ) {
        frequency = TEA5767_LOW_BAND_LIMIT_FREQ;
    } else if (frequency > TEA5767_HIGH_BAND_LIMIT_FREQ) {
        frequency = TEA5767_HIGH_BAND_LIMIT_FREQ;
    }
    this->frequency = frequency;
    applyFrequency();
    autoAjustSideInjection();
}

long FrequencyModulationReceiverTEA5767::getFrequency() {
    return frequency;
}

void FrequencyModulationReceiverTEA5767::setStation(float station) {
    setFrequency(stationToFrequency(station));
}

float FrequencyModulationReceiverTEA5767::getStation() {
    return frequencyToStation(frequency);
}

void FrequencyModulationReceiverTEA5767::setStereo(bool stereo) {
    w3rd.MS = stereo;
    flush();
}

bool FrequencyModulationReceiverTEA5767::isStereo() {
    read();
    return r3rd.STEREO;
}

void FrequencyModulationReceiverTEA5767::mute() {
    mute(SIDE_BOTH);
}

void FrequencyModulationReceiverTEA5767::mute(Side side) {
    setMute(side, 1);
}

void FrequencyModulationReceiverTEA5767::unmute() {
    unmute(SIDE_BOTH);
}

void FrequencyModulationReceiverTEA5767::unmute(Side side) {
    setMute(side, 0);
}

void FrequencyModulationReceiverTEA5767::setSoftmute(bool softmute) {
    w4th.SMUTE = softmute;
    flush();
}

bool FrequencyModulationReceiverTEA5767::isSoftmute() {
    return w4th.SMUTE;
}

void FrequencyModulationReceiverTEA5767::setSearchDirection(SerachDirection direction) {
    w3rd.SUD = direction;
    flush();
}

void FrequencyModulationReceiverTEA5767::setStandby(bool standby) {
    w4th.STBY = standby;
    flush();
}

bool FrequencyModulationReceiverTEA5767::isStandby() {
    return w4th.STBY;
}

void FrequencyModulationReceiverTEA5767::setSearchStopLevel(SearchStopLevel level) {
    w3rd.SSL = level;
    flush();
}

FrequencyModulationReceiverTEA5767::SearchStopLevel FrequencyModulationReceiverTEA5767::getSearchStopLevel() {
    return (SearchStopLevel) w3rd.SSL;
}

unsigned char FrequencyModulationReceiverTEA5767::getSignalLevel() {
    read();
    return r4th.LEV;
}

void FrequencyModulationReceiverTEA5767::setSearchMode(bool mode) {
    w1st.SM = mode;
    flush();
}

long FrequencyModulationReceiverTEA5767::searchNextFrequency() {
    long nextFrequency = 0;
    long oneGridStep = (w3rd.SUD == SD_UP) ? TEA5767_ONE_GRID_STEP : -(TEA5767_ONE_GRID_STEP);
    mute();
    setFrequency(frequency + oneGridStep);
    setSearchMode(true);
    while (!searchFinished())
        ;
    if (!isBandLimitReached()) {
        nextFrequency = getFoundStationFrequency();
    }
    setSearchMode(false);
    setFrequency(frequency - oneGridStep);
    unmute();
    return nextFrequency;
}

void FrequencyModulationReceiverTEA5767::setSideInjection(SideInjection level) {
    w3rd.HLSI = level;
    flush();
}

void FrequencyModulationReceiverTEA5767::autoAjustSideInjection() {
    unsigned char signal;
    setSideInjection(SI_LOW);
    signal = getSignalLevel();
    setSideInjection(SI_HIGH);
    if (signal > getSignalLevel()) {
        setSideInjection(SI_LOW);
    }
}

bool FrequencyModulationReceiverTEA5767::isBandLimitReached() {
    read();
    return r1st.BLF;
}

bool FrequencyModulationReceiverTEA5767::searchFinished() {
    read();
    return r1st.RF;
}

float FrequencyModulationReceiverTEA5767::getFoundStation() {
    return frequencyToStation(getFoundStationFrequency());
}

long FrequencyModulationReceiverTEA5767::getFoundStationFrequency() {
    unsigned int phaseLockedLoop = 0;
    phaseLockedLoop = r1st.PLL;
    phaseLockedLoop <<= 8;
    phaseLockedLoop |= r2nd.PLL;
    return phaseLockedLoopToFrequency(phaseLockedLoop);
}

unsigned char FrequencyModulationReceiverTEA5767::getIntermediateFrequency() {
    flush();
    delay(TEA5767_IF_LOAD_DELAY_MS);
    read();
    return r3rd.IF;
}

void FrequencyModulationReceiverTEA5767::setRawConfiguration(unsigned char *buf) {
    unsigned char* values[] = { &w1st.value, &w2nd.value, &w3rd.value, &w4th.value, &w5th.value };
    for (unsigned char i = 0; i < TEA5767_BYTES_COUNT; i++) {
        *values[i] = buf[i];
    }
    flush();
}

long FrequencyModulationReceiverTEA5767::stationToFrequency(float station) {
    return station * TEA5767_STATION_TO_FREQ;
}

float FrequencyModulationReceiverTEA5767::frequencyToStation(long frequency) {
    return frequency / TEA5767_STATION_TO_FREQ;
}

void FrequencyModulationReceiverTEA5767::setMute(Side side, bool mute) {
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

long FrequencyModulationReceiverTEA5767::phaseLockedLoopToFrequency(unsigned int phaseLockedLoop) {
    long intFreq = (w3rd.HLSI) ? TEA5767_INT_FREQ : -(TEA5767_INT_FREQ);
    return (phaseLockedLoop * TEA5767_REF_FREQ) / 4 - intFreq;
}

unsigned int FrequencyModulationReceiverTEA5767::frequencyToPhaseLockedLoop(long frequency) {
    long intFreq = (w3rd.HLSI) ? TEA5767_INT_FREQ : -(TEA5767_INT_FREQ);
    return 4 * (frequency + intFreq) / TEA5767_REF_FREQ;
}

void FrequencyModulationReceiverTEA5767::applyFrequency() {
    unsigned int phaseLockedLoop = frequencyToPhaseLockedLoop(frequency);
    w1st.PLL = (phaseLockedLoop >> 8) & 0x3f;
    w2nd.PLL = phaseLockedLoop & 0xff;
    flush();
}

void FrequencyModulationReceiverTEA5767::read() {
    unsigned char* input[] = { &r1st.value, &r2nd.value, &r3rd.value, &r4th.value, &r5th.value };
    Wire.requestFrom(TEA5767_I2C_ADDRESS, TEA5767_BYTES_COUNT);
    if (Wire.available()) {
        for (unsigned char i = 0; i < TEA5767_BYTES_COUNT; i++) {
            *input[i] = Wire.read();
        }
    }
    delay(50);
}

void FrequencyModulationReceiverTEA5767::flush() {
    unsigned char output[] = { w1st.value, w2nd.value, w3rd.value, w4th.value, w5th.value };
    Wire.beginTransmission(TEA5767_I2C_ADDRESS);
    for (unsigned char i = 0; i < TEA5767_BYTES_COUNT; i++) {
        Wire.write(output[i]);
    }
    Wire.endTransmission();
    delay(50);
}
