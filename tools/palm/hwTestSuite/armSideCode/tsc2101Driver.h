#ifndef TSC2101_DRIVER_H
#define TSC2101_DRIVER_H

#include <stdint.h>

#define TSC2101_REG_LOCATION(page, address) ((page) << 6 | (address))

enum{
   TOUCH_DATA_X = TSC2101_REG_LOCATION(0, 0x00),
   TOUCH_DATA_Y,
   TOUCH_DATA_Z1,
   TOUCH_DATA_Z2,
   TOUCH_DATA_RESERVED_0,
   TOUCH_DATA_BAT,
   TOUCH_DATA_RESERVED_1,
   TOUCH_DATA_AUX1,
   TOUCH_DATA_AUX2,
   TOUCH_DATA_TEMP1,
   TOUCH_DATA_TEMP2,
   //TOUCH_DATA_RESERVED_X...
   TOUCH_CONTROL_TSC_ADC = TSC2101_REG_LOCATION(1, 0x00),
   TOUCH_CONTROL_STATUS,
   TOUCH_CONTROL_BUFFER_MODE,
   TOUCH_CONTROL_REFERENCE,
   TOUCH_CONTROL_RESET_CONTROL_REGISTER,
   TOUCH_CONTROL_CONFIGURATION,
   TOUCH_CONTROL_TEMPERATURE_MAX,
   TOUCH_CONTROL_TEMPERATURE_MIN,
   TOUCH_CONTROL_AUX1_MAX,
   TOUCH_CONTROL_AUX1_MIN,
   TOUCH_CONTROL_AUX2_MAX,
   TOUCH_CONTROL_AUX2_MIN,
   TOUCH_CONTROL_MEASUREMENT_CONFIGURATION,
   TOUCH_CONTROL_PROGRAMMABLE_DELAY,
   //TOUCH_CONTROL_RESERVED_X...
   //TODO: add audio control registers
   BUFFER_START = TSC2101_REG_LOCATION(3, 0),
   BUFFER_END = TSC2101_REG_LOCATION(3, 63)
};

uint16_t tsc2101Read(uint8_t address);
void tsc2101Write(uint8_t address, uint16_t value);

#endif
