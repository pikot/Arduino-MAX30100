#ifndef MAX30100_ULP_H
#define MAX30100_ULP_H

#include "MAX30100_BeatDetector.h"
#include "MAX30100_Filters.h"


#define MAX30100_ULP_FLAG_SENSOR_IS_INITED 0x1
#define MAX30100_ULP_FLAG_BUFFER_IS_FULL 0x2

class  MAX30100_ULP {
public:
    MAX30100_ULP();
    bool begin( uint16_t *_flags, uint32_t **data_array, uint32_t curr_pointer);

    float    calcHeartRate( int debuggingMode);
    uint16_t rawDataLen();
    float    flushUlpBuffer();
private:
    uint16_t *flags;
    uint32_t **data_start_pointer;
    uint32_t data_curr_pointer;
  //  bool debuggingMode;

    FilterBuLp1ULP lpf; 
    BeatDetector beatDetector;
    DCRemover   irDCRemover; 
};


#endif
