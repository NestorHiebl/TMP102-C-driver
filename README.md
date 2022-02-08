# TMP102 C driver

Important note: this driver was originally writen for the STM32 Hardware Abstraction Layer. However, given that the driver only interacts with the controller's peripherals through I²C, adapting it to a different architecture or library is very easy. All I²C interaction is encapsulated in the following two functions:

```c
int tmp102_generic_read(I2C_HandleTypeDef *hi2c, uint8_t *deviceRegister, uint8_t *data);
int tmp102_generic_write(I2C_HandleTypeDef *hi2c, uint8_t *deviceRegister, uint8_t *data);
```

To make them compatible with whatever you are using, simply replace the respective I²C interface methods in these functions' bodies. 

