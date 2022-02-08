#include "main.h"
#include "tmp102_driver.h"
#include <stdint.h>

float tmp102_convert_temperature_from_register(uint8_t *data) {
	if (data == NULL) {
		return TMP102_ERROR;
	}

	uint8_t negative = 0;

	/* If the MSB of the temperature register is 1, the temperature is negative in two's complement */
	if (data[0] >= (0x1 << 7)) {
		negative = 1;
	}

	int16_t temp = ((data[0] << 8) | data [1]) >> 4;

	if (negative) {
		temp |= 0xf000;
	}

	return TMP102_RESOLUTION * (float) temp;
}

uint16_t tmp102_convert_temperature_to_register(float data) {
	if((data < TMP102_MIN_TEMP_C) || (data > TMP102_MAX_TEMP_C)) {
		return TMP102_ERROR;
	}

	int16_t temp = (int16_t) (data / TMP102_RESOLUTION);


	if (temp < 0) {
		/* Convert to 12-bit, left justified number */
		temp <<= 4;
		/* Denote negative number by setting MSB to 1 */
		temp |= (0x1 << 15);
	} else {
		/* Convert to 12-bit, left justified number */
		temp <<= 4;
		/* Denote positive number by setting MSB to 0 */
		temp &= ~(0x1 << 15);
	}

	/* Casting from signed to unsigned does not change the binary form of the result */
	return (uint16_t) temp;
}

float tmp102_get_temperature(I2C_HandleTypeDef *hi2c) {
	uint8_t data[2] = { 0 };

	uint8_t tmp102_datapointer = TMP102_TEMP_REGISTER_ADDRESS;

	if (tmp102_generic_read(hi2c, &tmp102_datapointer, data) == TMP102_ERROR) {
		return (float) TMP102_ERROR;
	}

	return tmp102_convert_temperature_from_register(data);
}

uint16_t tmp102_get_config(I2C_HandleTypeDef *hi2c) {
	uint8_t data[2] = { 0 };

	uint8_t tmp102_datapointer = TMP102_CONF_REGISTER_ADDRESS;

	if (tmp102_generic_read(hi2c, &tmp102_datapointer, data) == TMP102_ERROR) {
		return TMP102_ERROR;
	}

	uint16_t retval = (data[0] << 8) | (data[1]);

	return retval;
}

int tmp102_set_tlow(I2C_HandleTypeDef *hi2c, float temperature) {
	if((temperature < TMP102_MIN_TEMP_C) || (temperature > TMP102_MAX_TEMP_C)) {
			return TMP102_ERROR;
		}

	uint16_t temp_register = tmp102_convert_temperature_to_register(temperature);

	if (temp_register == TMP102_ERROR) {
		return TMP102_ERROR;
	}

	uint8_t data[2] = {temp_register >> 8, temp_register & 0xff};

	uint8_t device_register = TMP102_TLOW_REGISTER_ADDRESS;

	return tmp102_generic_write(hi2c, &device_register, data);
}

int tmp102_set_thigh(I2C_HandleTypeDef *hi2c, float temperature) {
	if((temperature < TMP102_MIN_TEMP_C) || (temperature > TMP102_MAX_TEMP_C)) {
		return TMP102_ERROR;
	}

	uint16_t temp_register = tmp102_convert_temperature_to_register(temperature);

	if (temp_register == TMP102_ERROR) {
		return TMP102_ERROR;
	}

	uint8_t data[2] = {temp_register >> 8, temp_register & 0xff};

	uint8_t device_register = TMP102_THIGH_REGISTER_ADDRESS;

	return tmp102_generic_write(hi2c, &device_register, data);
}

int tmp102_generic_read(I2C_HandleTypeDef *hi2c, uint8_t *deviceRegister, uint8_t *data) {
	uint8_t err = 0;

	if ((hi2c == NULL) || (deviceRegister == NULL) || (data == NULL)) {
		return TMP102_ERROR;
	}

	if (HAL_I2C_Master_Transmit(hi2c, TMP102_I2C_ADDRESS | 1, deviceRegister, 1, TMP102_I2C_TIMEOUT) == HAL_ERROR) {
		err = TMP102_ERROR;
	}

	if (HAL_I2C_Master_Receive(hi2c, TMP102_I2C_ADDRESS, data, 2, TMP102_I2C_TIMEOUT) == HAL_ERROR) {
		err = TMP102_ERROR;
	}

	return err;
}

int tmp102_generic_write(I2C_HandleTypeDef *hi2c, uint8_t *deviceRegister, uint8_t *data) {
	uint8_t err = 0;

	if ((hi2c == NULL) || (deviceRegister == NULL) || (data == NULL)) {
		return TMP102_ERROR;
	}

	uint8_t packet[3] = { *deviceRegister, data[0], data[1] };

	if (HAL_I2C_Master_Transmit(hi2c, TMP102_I2C_ADDRESS, packet, 3, TMP102_I2C_TIMEOUT) == HAL_ERROR) {
		err = TMP102_ERROR;
	}

	return err;
}

int tmp102_set_config(I2C_HandleTypeDef *hi2c, tmp102_config_t config) {
	uint8_t data[2] = {config >> 8, config & 0xff};

	uint8_t device_register = TMP102_CONF_REGISTER_ADDRESS;

	return tmp102_generic_write(hi2c, &device_register, data);
}
