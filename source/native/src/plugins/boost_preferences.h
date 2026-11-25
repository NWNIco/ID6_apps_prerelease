#ifndef BOOST_PREFERENCES_H
#define BOOST_PREFERENCES_H

#include "gauge_preferences.h"
#include "gauge_core.h"

enum BoostSetting {
    BOOST_PRESSURE, BOOST_SPEED, BOOST_TEMPERATURE, BOOST_STYLE,
    BOOST_CHIME, BOOST_COOLANT_ALERT, BOOST_OIL_TEMP_ALERT, BOOST_OIL_PRESSURE_ALERT,
    BOOST_BOOST_UNIT, BOOST_SETTING_COUNT
};

// BOOST_BOOST_UNIT: 0 follows the pressure setting, 1..3 select psi, bar or kPa
// for the boost gauge only.
static const unsigned char kBoostChoices[] = {3, 2, 2, 2, 2, 10, 10, 6, 4};

typedef char boost_choice_count[(sizeof kBoostChoices==BOOST_SETTING_COUNT) ? 1 : -1];

inline bool boostAlertSetting(unsigned row) {
    return row>=BOOST_COOLANT_ALERT && row<=BOOST_OIL_PRESSURE_ALERT;
}

struct BoostPreferences {
    unsigned color;
    unsigned char choice[BOOST_SETTING_COUNT];

    BoostPreferences() : color(0) {
        for (unsigned i=0; i<BOOST_SETTING_COUNT; ++i) choice[i]=0;
    }

    void load(const char* path, unsigned colors) {
        FILE* file=fopen(path,"rb");
        if (!file) return;
        unsigned char data[3+BOOST_SETTING_COUNT];
        size_t length=fread(data,1,sizeof data,file);
        bool complete=fgetc(file)==EOF && !ferror(file);
        fclose(file);
        if (!complete || length<4 || data[0]!='G' || data[2]>=colors) return;
        if (data[1]==1 && length==4 && data[3]<=1) {
            color=data[2];
            choice[BOOST_PRESSURE]=choice[BOOST_SPEED]=choice[BOOST_TEMPERATURE]=data[3];
        } else if ((data[1]==2 && length==sizeof data-1) ||
                   (data[1]==3 && length==sizeof data)) {
            // Version 2 files predate the boost unit and keep following the pressure setting.
            if (data[1]==2) data[3+BOOST_BOOST_UNIT]=0;
            // Earlier beta builds offered pressure thresholds of up to 60 psi.
            unsigned char& floor=data[3+BOOST_OIL_PRESSURE_ALERT];
            const unsigned char maximum=kBoostChoices[BOOST_OIL_PRESSURE_ALERT]-1;
            if (floor>maximum && floor<=12) floor=maximum;
            for (unsigned i=0; i<BOOST_SETTING_COUNT; ++i)
                if (data[3+i]>=kBoostChoices[i]) return;
            color=data[2];
            for (unsigned i=0; i<BOOST_SETTING_COUNT; ++i) choice[i]=data[3+i];
        }
    }

    bool save(const char* path) const {
        unsigned char data[3+BOOST_SETTING_COUNT]={'G',3,static_cast<unsigned char>(color)};
        for (unsigned i=0; i<BOOST_SETTING_COUNT; ++i) data[3+i]=choice[i];
        return savePersistentBytes(path,data,sizeof data);
    }

    int units(unsigned signal) const {
        if (signal==BD_SPEED) return choice[BOOST_SPEED];
        if (signal==BD_BOOST && choice[BOOST_BOOST_UNIT]) return choice[BOOST_BOOST_UNIT]-1;
        if (bd_is_pressure(signal)) return choice[BOOST_PRESSURE];
        return choice[BOOST_TEMPERATURE];
    }
};

inline double boostAlertThreshold(unsigned alarm, unsigned option) {
    if (alarm==2) return option*5.0/14.5037738; // psi floor converted to bar
    return (220.0+5.0*(option-1)-32.0)/1.8;   // Fahrenheit limit converted to Celsius
}

class BoostAlerts {
    uint64_t since_[3];
    bool latched_[3];
public:
    BoostAlerts() { for (unsigned i=0;i<3;++i) reset(i); }
    void reset(unsigned i) { since_[i]=0; latched_[i]=false; }

    bool evaluate(unsigned i, bool enabled, bool fresh, double value,
                  double threshold, uint64_t sample, bool& entered) {
        entered=false;
        if (!enabled) { reset(i); return false; }
        if (!fresh) { since_[i]=0; return false; }
        const double margin=i==2 ? 0.2 : 3.0;
        if (latched_[i]) {
            const bool recovered=i==2 ? value>=threshold+margin : value<=threshold-margin;
            if (recovered) reset(i);
        } else if (i==2 ? value<threshold : value>=threshold) {
            if (!since_[i]) since_[i]=sample;
            if (sample>=since_[i] && sample-since_[i]>=1200) entered=latched_[i]=true;
        } else since_[i]=0;
        return latched_[i];
    }
};

#endif
