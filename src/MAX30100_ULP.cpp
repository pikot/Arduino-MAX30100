#include <Arduino.h>

#include "MAX30100_ULP.h"
#include "MAX30100_PulseOximeter.h"


// it works only for esp32, and HR!
MAX30100_ULP::MAX30100_ULP()
{
}

bool MAX30100_ULP::begin( uint16_t *_flags, uint32_t **data_array, uint32_t curr_pointer )
{
    flags      = _flags;
    data_start_pointer = data_array;
    data_curr_pointer  = curr_pointer;

    return true;
}

float  MAX30100_ULP::calcHeartRate( int _debuggingMode)
{
    uint32_t rawIRValue;
    if ( !flags || !*flags ||
         !data_start_pointer || !*data_start_pointer  ||
         !data_curr_pointer  ||
        0 == *flags & MAX30100_ULP_FLAG_BUFFER_IS_FULL ) { // if data not ready - skip
        return -1;
    }

    int raw_data_len = rawDataLen();
   // Serial.printf( "raw_data_len %d\n", raw_data_len );
 //   return -1
    uint32_t *mm = ( uint32_t *)  data_start_pointer;
    // Dequeue all available samples, they're properly timed by the HRM
    for ( int32_t i = 2; i <  raw_data_len ; i++ ) {
        rawIRValue =  mm[i] & UINT16_MAX;
        float irACValue = irDCRemover.step(rawIRValue);

        // The signal fed to the beat detector is mirrored since the cleanest monotonic spike is below zero
        float filteredPulseValue = lpf.step(-irACValue);
        bool beatDetected = beatDetector.addSample(filteredPulseValue);

        switch (_debuggingMode) {
            case PULSEOXIMETER_DEBUGGINGMODE_RAW_VALUES:
                Serial.printf("R:%d\n", rawIRValue);
                break;

            case PULSEOXIMETER_DEBUGGINGMODE_AC_VALUES:
                Serial.print("R:");
                Serial.println(irACValue);
                break;

            case PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT:
                Serial.print("R:");
                Serial.println(filteredPulseValue);
           //     Serial.print(",");
         //       Serial.println(beatDetector.getCurrentThreshold());
                break;

            default:
                break;
        }
    }
    return beatDetector.getRate();
}

uint16_t MAX30100_ULP::rawDataLen(){
      return  ( (data_curr_pointer & UINT16_MAX) *4 -  ( (uint32_t)( data_start_pointer )& UINT16_MAX) )/ 4;
}


float  MAX30100_ULP::flushUlpBuffer()
{
    *flags  = *flags & MAX30100_ULP_FLAG_BUFFER_IS_FULL;
}

