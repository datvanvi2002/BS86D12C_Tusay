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

/// @brief Read temperature in Celsius from the temperature sensor connected to AN0.
/// @param adc_value ADC value from pressure sensor
/// @return
float temperature_sensor_read();

/// @brief Read pressure in channel0 PD0 pin
/// @return kPa: 0 -550 kPa
float pressure_sensor_read_kPa();
#endif // ADC_H