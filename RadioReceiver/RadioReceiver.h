/**
 * Arduino - Radio Receiver
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_RECEIVER_H__
#define __ARDUINO_RADIO_RECEIVER_H__ 1

class RadioReceiver {

public:

    enum SerachDirection {
        SD_DOWN,
        SD_UP
    };

    enum Side {
        SIDE_LEFT = 0x01,
        SIDE_RIGHT = 0x02,
        SIDE_BOTH = SIDE_LEFT | SIDE_RIGHT
    };

    enum SideInjection {
        SI_LOW = 0x00,
        SI_HIGH = 0x01
    };

    enum SearchStopLevel {
        SSL_LOW = 0x01,
        SSL_MID = 0x02,
        SSL_HIGH = 0x03
    };

    RadioReceiver();

    virtual ~RadioReceiver();

    virtual void setFrequency(long frequency) = 0;

    virtual long getFrequency() = 0;

    virtual void setStation(float station) = 0;

    virtual float getStation() = 0;

    virtual void setStereo(bool stereo) = 0;

    virtual bool getStereo() = 0;

    virtual void mute(Side size) = 0;

    virtual void unmute(Side size) = 0;

    virtual void setSearchDirection(SerachDirection direction) = 0;

    virtual void setStandby(bool standby) = 0;

    virtual void setSearchStopLevel(SearchStopLevel level) = 0;

    virtual SearchStopLevel getSearchStopLevel() = 0;

    virtual void setSearchMode(bool mode) = 0;

    virtual long searchNextFrequency() = 0;

    virtual unsigned char getSignalLevel() = 0;

    virtual bool isBandLimitReached() = 0;

    virtual bool isReady() = 0;

    virtual long getFoundStationFrequency() = 0;
};

#endif // __ARDUINO_RADIO_RECEIVER_H__
