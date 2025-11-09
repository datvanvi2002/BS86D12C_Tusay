#ifndef ADC_H
#define ADC_H
#include "bs86d12c.h"
#include <stdint.h>
/// @brief Initialize ADC with VDD as reference voltage. Configure AN0 and AN1 as analog inputs.
void adc_init_vdd_ref(void);
/// @brief Read the ADC value from the specified channel (0..15).
/// @param channel ADC channel number (0..15)

unsigned int adc_read_channel(unsigned char channel);
/// @brief Convert a 12-bit ADC value to millivolts based on the given VDD in millivolts.
/// @param adc12 12-bit ADC value (0..4095) 
/// @param vdd_mV VDD voltage in millivolts

/// @brief Read pressure sensor value in millibar from ADC value.
/// @param adc_value ADC value from pressure sensor
/// @return 
int temperature_sensor_read(uint16_t adc_value);

#endif // ADC_H