/**
 * Arduino - Radio Receiver
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_RECEIVER_TEA5767_H__
#define __ARDUINO_RADIO_RECEIVER_TEA5767_H__ 1

#include <RadioReceiver.h>

#define RADIO_RECEIVER_TEA5767_I2C_ADDRESS      0x60
#define RADIO_RECEIVER_TEA5767_REF_FREQ         32768
#define RADIO_RECEIVER_TEA5767_INT_FREQ         225000
#define RADIO_RECEIVER_TEA5767_BYTES_COUNT      5
#define RADIO_RECEIVER_TEA5767_STATION_TO_FREQ  1000000.0

class RadioReceiverTEA5767: public RadioReceiver {

    union AWriteBits {

        struct {

            // Setting of synthesizer programmable counter for search or preset.
            unsigned char PLL :6;

            // Search mode: if SM = 1 then in search mode; if SM = 0 then not in search mode.
            unsigned char SM :1;

            // If MUTE = 1 then L and R audio are muted; if MUTE = 0 then L and R audio are not muted.
            unsigned char MUTE :1;
        };
        unsigned char value;
    };

    union BWriteBits {

        struct {

            // Setting of synthesizer programmable counter for search or preset.
            unsigned char PLL :8;
        };
        unsigned char value;
    };

    union CWriteBits {

        struct {

            // Software programmable port 1: if SWP1 = 1 then port 1 is HIGH; if SWP1 = 0 then port 1 is LOW.
            unsigned char SWP1 :1;

            // Mute Left: if ML = 1 then the left audio channel is muted and forced mono; if ML = 0 then the left audio channel is not muted.
            unsigned char ML :1;

            // Mute Right: if MR = 1 then the right audio channel is muted and forced mono; if MR = 0 then the right audio channel is not muted.
            unsigned char MR :1;

            // Mono to Stereo: if MS = 1 then forced mono; if MS = 0 then stereo ON.
            unsigned char MS :1;

            // High/Low Side Injection: if HLSI = 1 then high side LO injection; if HLSI = 0 then low side LO injection.
            unsigned char HLSI :1;

            // Search Stop Level.
            unsigned char SSL :2;

            // Search Up/Down: if SUD = 1 then search up; if SUD = 0 then search down.
            unsigned char SUD :1;
        };
        unsigned char value;
    };

    union DWriteBits {

        struct {

            // Search Indicator: if SI = 1 then pin SWPORT1 is output for the ready flag; if SI = 0 then pin SWPORT1 is software programmable port 1.
            unsigned char SI :1;

            // Stereo Noise Cancelling: if SNC = 1 then stereo noise cancelling is ON; if SNC = 0 then stereo noise cancelling is OFF.
            unsigned char SNC :1;

            // High Cut Control: if HCC = 1 then high cut control is ON; if HCC = 0 then high cut control is OFF.
            unsigned char HCC :1;

            // Soft Mute: if SMUTE = 1 then soft mute is ON; if SMUTE = 0 then soft mute is OFF.
            unsigned char SMUTE :1;

            // Clock frequency.
            unsigned char XTAL :1;

            // Band Limits: if BL = 1 then Japanese FM band; if BL = 0 then US/Europe FM band.
            unsigned char BL :1;

            // Standby: if STBY = 1 then in Standby mode; if STBY = 0 then not in Standby mode.
            unsigned char STBY :1;

            // Software programmable port 2: if SWP2 = 1 then port 2 is HIGH; if SWP2 = 0 then port 2 is LOW.
            unsigned char SWP2 :1;
        };
        unsigned char value;
    };

    union EWriteBits {

        struct {

            // Not used; position is don’t care.
            unsigned char :5;

            // If DTC = 1 then the de-emphasis time constant is 75 μs; if DTC = 0 then the de-emphasis time constant is 50 μs.
            unsigned char DTC :1;

            // If PLLREF = 1 then the 6.5 MHz reference frequency for the PLL is enabled;
            // If PLLREF = 0 then the 6.5 MHz reference frequency for the PLL is disabled.
            unsigned char PLLREF :1;
        };
        unsigned char value;
    };

    union AReadBits {

        struct {

            // PLL[13:8] setting of synthesizer programmable counter after search or preset.
            unsigned char PLL :5;

            // Band Limit Flag: if BLF = 1 then the band limit has been reached; if BLF = 0 then the band limit has not been reached.
            unsigned char BLF :1;

            // Ready Flag: if RF = 1 then a station has been found or the band limit has been reached; if RF = 0 then no station has been found.
            unsigned char RF :1;
        };
        unsigned char value;
    };

    union BReadBits {

        struct {

            // PLL[7:0] setting of synthesizer programmable counter after search or preset.
            unsigned char PLL :8;
        };
        unsigned char value;
    };

    union CReadBits {

        struct {

            // IF counter result.
            unsigned char IF :7;

            // Stereo indication: if STEREO = 1 then stereo reception; if STEREO = 0 then mono reception.
            unsigned char STEREO :1;
        };
        unsigned char value;
    };

    union DReadBits {

        struct {

            // This bit is internally set to logic 0.
            unsigned char :1;

            // Chip Identification: these bits have to be set to logic 0.
            unsigned char CI :3;

            // Level ADC output.
            unsigned char LEV :4;
        };
        unsigned char value;
    };

    union EReadBits {

        struct {

            // This bit is internally set to logic 0.
            unsigned char :8;
        };
        unsigned char value;
    };

    long frequency;

    AWriteBits aw;
    BWriteBits bw;
    CWriteBits cw;
    DWriteBits dw;
    EWriteBits ew;

    AReadBits ar;
    BReadBits br;
    CReadBits cr;
    DReadBits dr;
    EReadBits er;
public:

    enum SideInjection {
        SI_LOW,
        SI_HIGH
    };

    enum SearchStopLevel {

        // level ADC output = 5
        SSL_LOW = 0x01,

        // level ADC output = 7
        SSL_MID = 0x02,

        // level ADC output = 10
        SSL_HIGH = 0x03
    };

    RadioReceiverTEA5767();

    void initialize();

    void setFrequency(long frequency);

    long getFrequency();

    void setStation(float station);

    float getStation();

    void setStereo(bool stereo);

    bool getStereo();

    void mute();

    void mute(Side side);

    void unmute();

    void unmute(Side side);

    void setMute(Side side, bool mute);

    void setSearchDirection(SerachDirection direction);

    void setStandby(bool standby);

    void setSearchStopLevel(SearchStopLevel level);

    SearchStopLevel getSearchStopLevel();

    void startSearchMode();

    void stopSearchMode();

    void setSearchMode(bool mode);

    void setSideInjection(SideInjection level);

    void setRawConfiguration(unsigned char *buf);

private:

    void applyFrequency();

    void read();

    void flush();
};

#endif // __ARDUINO_RADIO_RECEIVER_TEA5767_H__
