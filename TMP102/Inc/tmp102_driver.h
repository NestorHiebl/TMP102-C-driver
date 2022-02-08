#include <limits.h>
#include <stdint.h>

#ifndef TMP102
#define TMP102


#define TMP102_I2C_ADDRESS (0x48 << 1)
#define TMP102_I2C_TIMEOUT 1000
#define TMP102_RESOLUTION 0.0625f
#define TMP102_ERROR 0xff

#define TMP102_MAX_TEMP_C 128.0f
#define TMP102_MIN_TEMP_C -40.0f

/* Tmp102 register addresses */
#define TMP102_TEMP_REGISTER_ADDRESS 	 0x0
#define TMP102_CONF_REGISTER_ADDRESS 	 0x1
#define TMP102_TLOW_REGISTER_ADDRESS	 0x2
#define TMP102_THIGH_REGISTER_ADDRESS	 0x3

/*
 *  Tmp102 configuration register structure
 *  Configuration byte 1
┌───────────────┬──────────┬───────────────┬───────────────┬───────────────┬───────────────┬──────────┬─────────────────┬───────────────┐
│      Bit      │    D7    │      D6       │      D5       │      D4       │      D3       │    D2    │       D1        │      D0       │
├───────────────┼──────────┼───────────────┼───────────────┼───────────────┼───────────────┼──────────┼─────────────────┼───────────────┤
│ Function      │ One-Shot │ Resolution 1  │ Resolution 0  │ Fault queue 1 │ Fault queue 0 │ Polarity │ Thermostat mode │ Shutdown mode │
│ Default value │ 0        │ 1 (Read-only) │ 1 (Read-only) │ 0             │ 0             │ 0        │ 0               │ 0             │
└───────────────┴──────────┴───────────────┴───────────────┴───────────────┴───────────────┴──────────┴─────────────────┴───────────────┘
 * Configuration byte 2
┌───────────────┬───────────────────┬───────────────────┬───────────────┬───────────────┬────┬────┬────┬────┐
│      Bit      │        D7         │        D6         │      D5       │      D4       │ D3 │ D2 │ D1 │ D0 │
├───────────────┼───────────────────┼───────────────────┼───────────────┼───────────────┼────┼────┼────┼────┤
│ Function      │ Conversion Rate 1 │ Conversion Rate 0 │ Alert         │ Extended Mode │  0 │  0 │  0 │  0 │
│ Default value │ 1                 │ 0                 │ 1 (Read-only) │ 0             │  0 │  0 │  0 │  0 │
└───────────────┴───────────────────┴───────────────────┴───────────────┴───────────────┴────┴────┴────┴────┘
 */

/* Configuration settings */
#define TMP102_EMPTY_CONFIG				(0x0)
#define TMP102_DEFAULT_CONFIG			(0x60A0)

#define TMP102_TLOW_REGISTER_DEFAULT	(0x4B00)
#define TMP102_THIGH_REGISTER_DEFAULT	(0x5000)

#define TMP102_ONESHOT					(0x1 << 15)

#define TMP102_1_FAULT_ALERT			(0x0 << 11)
#define TMP102_2_FAULT_ALERT			(0x1 << 11)
#define TMP102_4_FAULT_ALERT			(0x2 << 11)
#define TMP102_6_FAULT_ALERT			(0x3 << 11)

#define TMP102_POLARITY_0				(0x0)
#define TMP102_POLARITY_1				(0x1 << 10)

#define TMP102_THERMO_COMPARATOR_MODE	(0x0)
#define TMP102_THERMO_INTERRUPT_MODE	(0x1 << 9)

#define TMP102_SHUTDOWN_MODE_OFF		(0x0)
#define TMP102_SHUTDOWN_MODE_ON			(0x1 << 8)

#define TMP102_CONV_RATE_QUARTER_HZ		(0x0)
#define TMP102_CONV_RATE_1_HZ			(0x1 << 6)
#define TMP102_CONV_RATE_4_HZ			(0x2 << 6)
#define TMP102_CONV_RATE_8_HZ			(0x3 << 6)

#define TMP102_EXTENDED_MODE_OFF		(0x0)
#define TMP102_EXTENDED_MODE_ON			(0x1 << 4)

typedef uint16_t tmp102_config_t;

/**
 * This function expects a char array of length 2 as its input. It converts the raw temperature
 * format found in the tmp102 temperature register to a float expressing the temperature in Celsius. It
 * achieves this by shifting the most significant byte, data[0] all the way to the left, adding it to the
 * least significant byte, data[1], and then shifting the result to the right four times before multiplying
 * it with the tmp102's temperature resolution (0.0625).
 */
float tmp102_convert_temperature_from_register(uint8_t *data);

/**
 * The inverse of the above conversion function. Takes a given float value between -40.0 and 128.0 and
 * converts it to the format used by the tmp102's registers. The result is handed over as a unsigned 16
 * bit integer, which can then be converted to a unsigned 8 bit integer array of length 2. The last four
 * bits of the return value will always be 0 if the conversion worked. On error, TMP102_ERROR
 * is returned.
 */
uint16_t tmp102_convert_temperature_to_register(float data);

/**
 * Returns the contents of the tmp102's temperature register, after conversion to Celsius. The result is
 * received through communication over I²C, with the hi2c parameter referring to the microcontroller's
 * I²C connection the tmp102 can be found on. On error, returns TMP102_ERROR.
 */
float tmp102_get_temperature(I2C_HandleTypeDef *hi2c);

/**
 * Returns the contents of the tmp102's configuration register. The result is received through communication
 * over I²C, with the hi2c parameter referring to the microcontroller's I²C connection the tmp102 can be
 * found on. On error, returns TMP102_ERROR. For a brief description of the configuration register's format,
 * refer to the section at the beginning of this file.
 */
uint16_t tmp102_get_config(I2C_HandleTypeDef *hi2c);

int tmp102_set_tlow(I2C_HandleTypeDef *hi2c, float temperature);

int tmp102_set_thigh(I2C_HandleTypeDef *hi2c, float temperature);

/**
 * Reads the contents of a register on the tmp102 determined by the deviceRegister parameter. The result is
 * placed into the array specified by the data parameter. It must be a uint8_t array of length 2. The result
 * is received through communication over I²C, with the hi2c parameter referring to the microcontroller's I²C
 * connection the tmp102 can be found on. On error, returns TMP102_ERROR.
 */
int tmp102_generic_read(I2C_HandleTypeDef *hi2c, uint8_t *deviceRegister, uint8_t *data);

/**
 * Writes the contents of the length two uint8_t data array to the tmp102 given by deviceRegister. The hi2c
 * parameter determines the microcontroller's I²C connection the tmp102 can be found on. On error, returns
 * TMP102_ERROR.
 */
int tmp102_generic_write(I2C_HandleTypeDef *hi2c, uint8_t *deviceRegister, uint8_t *data);

/**
 * Loads a given configuration into the tmp102's configuration register. The proper way to assemble a
 * configuration is to start with the TMP102_EMPTY_CONFIG macro and bitwise or the needed properties onto it.
 * See the table at the top of this document for more information on the configuration options. The end result
 * inside the tmp102's configuration register will probably not be identical to the given config argument, as
 * certain bits inside the register are read-only.
 */
int tmp102_set_config(I2C_HandleTypeDef *hi2c, tmp102_config_t config);

#endif
