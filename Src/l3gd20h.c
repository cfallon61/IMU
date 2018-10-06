/*
 * l3gd20h.c
 *
 *  Created on: Sep 15, 2018
 *      Author: Chris
 */
#include "l3gd20h.h"

HAL_StatusTypeDef gyro_init(I2C_HandleTypeDef *hi2c, GYRO_DATA_RATE data_rate, GYRO_FULL_SCALE full_scale, int high_pass_filter)
{
	HAL_StatusTypeDef status;

//	if the device is not ready return error
	if ((status = HAL_I2C_IsDeviceReady(hi2c, L3GD20H_ADDR, 2, 100)) != HAL_OK)
	{
		return status;
	}

	//set the sensitivity based on the full-scale selection
	switch (full_scale)
	{
		case (FS_245_DPS):
			gyro.sensitivity = 8.75;
			break;
		case (FS_500_DPS):
			gyro.sensitivity = 17.5;
			break;
		case(FS_2000_DPS):
			gyro.sensitivity = 70.0;
			break;
	}
	gyro.sensitivity = gyro.sensitivity / 1000;


	/*INIT CTRL1
	 * L3GD20H_CTRL1
	 * Output data rate / bandwidth selection / Power down mode /  enable XEN, YEN, ZEN
	 * [7:6] Data Rate. |
	 * [5:4] bandwidth Selection |
	 * [3] Power Mode. Default: 0 (Power Down); 1: (Normal Mode) |
	 * [2] Z axis enable. Default: 1 (Enabled); 0: (Disabled) |
	 * [1] Y axis enable. Default: 1 (Enabled); 0: (Disabled) |
	 * [0] X axis enable. Default: 1 (Enabled); 0: (Disabled)*/
	uint8_t bandwidth = 0x02 << 4;
	uint8_t init = 0x00;

	init = ( data_rate << 6 ) | bandwidth | ( 0x01 << 3) |
			( 0x01 << 2 ) | ( 0x01 << 1 ) | 0x01;
//	init = 0x6F;

	/*Write to CTRL1
	 * Set bandwidth to max speed, enable XYZ axes, and enable Normal Mode*/

	if ((status = HAL_I2C_Mem_Write(hi2c, L3GD20H_ADDR, L3GD20H_CTRL1, 1, &init, 1, 100)) != HAL_OK)
	{
		return status;
	}

	/*CTRL4 INIT
	 * [7] Block data update. Default: 0 (continuous update); 1: (output registers not updated until MSB and LSB reading) |
	 * [6] Big/little endian selection. Default: 0 (Data LSB @ lower address); 1: (Data MSB @ lower address) |
	 * [5:4] Full Scale Selection. Default: 0x00 (245 DPS); 0x01: (500 DPS); 0x02: (2000 DPS) |
	 * [3] Level Sensitive latch enable - NOT NEEDED |
	 * [2:1] Self-Test enable. Default: 0x00 (normal mode); 0x01: (self-test 0(+)); 0x02: (unused); 0x03: (self-test 1(-)) |
	 * [0] SPI Mode selection. Default: 0 (4-Wire) - NOT NEEDED*/

	init = full_scale << 4;

	/*write to CTRL4
	 * continuous update, little endian mode, full-scale selection, defaults for the rest
	 * if not HAL_OK, return the error*/

	if ((status = HAL_I2C_Mem_Write(hi2c, L3GD20H_ADDR, L3GD20H_CTRL4, 1, &init, 1, 100)) != HAL_OK)
	{
		return status;
	}

	/*If the high_pass_filter flag is set, then write to CTRL5 to enable it;
	 * */

	if (high_pass_filter)
	{
		/*L3GD20H_CTRL5
		[7] Reboot memory content. Default: 0 (normal mode); 1: (reboot memory content) |
		[6] FIFO enable. Default: 0 (disabled) |
		[5] - NOT NEEDED |
		[4] High pass filter enable. Default: 0 (Disabled); 1: (Enabled) |
		[3:0] - NOT NEEDED*/
		init = 0x01 << 4;
		if ((status = HAL_I2C_Mem_Write(hi2c, L3GD20H_ADDR, L3GD20H_CTRL5, 1, &init, 1, 100)) != HAL_OK)
			{
				return status;
			}
	}

	return HAL_OK;
}

/*Read the gyro data for each axis and put that value into the struct's storage values
 * I2C_HandleTypeDef *hi2c -- pointer to i2c HandleTypeDef
 * retval: HAL_StatusTypeDef*/

HAL_StatusTypeDef read_gyro(I2C_HandleTypeDef *hi2c)
{
	HAL_StatusTypeDef status;

	//read the X registers and write the raw values to the struct's x data point

	if ((status = read_reg(hi2c, L3GD20H_OUT_X_H, L3GD20H_OUT_X_L, &gyro.gyro_x_high, &gyro.gyro_x_low)) != HAL_OK)
	{
		return status;
	}

	//read the Y registers and write the raw values to the struct's y data point

	if ((status = read_reg(hi2c, L3GD20H_OUT_Y_H, L3GD20H_OUT_Y_L, &gyro.gyro_y_high, &gyro.gyro_y_low)) != HAL_OK)
	{
		return status;
	}

	//read the Z registers and write the raw values to the struct's z data point

	if ((status = read_reg(hi2c, L3GD20H_OUT_Z_H, L3GD20H_OUT_Z_L, &gyro.gyro_z_high, &gyro.gyro_z_low)) != HAL_OK)
	{
		return status;
	}

	return HAL_OK;
}


/*static HAL_StatusTypeDef read_reg	(I2C_HandleTypeDef *hi2c, uint8_t addr_high, uint8_t addr_low, int16_t *output)	-- reads the 2 data registers and puts the raw value into output
 *
 * NOTE: REGISTERS ARE LITTLE ENDIAN, THEREFORE THE HIGH REGISTER IS THE MSB AND LOW REGISTER IS LSB
 *
 *I2C_HandleTypeDef *hi2c		-- I2C pointer
 *uint8_t addr_high				-- address of the high output register
 *uint8_t addr_low				-- address of the low output register
 *int16_t *output				-- pointer to the raw data output value
 *retvalue: HAL_StatusTypeDef	--returns HAL_OK if no errors
 **/

static HAL_StatusTypeDef read_reg(I2C_HandleTypeDef *hi2c, uint8_t addr_high, uint8_t addr_low, uint8_t *out_high, uint8_t *out_low)
{
	HAL_StatusTypeDef status;
	//if there was an error reading the high storage address, set output to -10000 and return the error

	if ((status = HAL_I2C_Mem_Read(hi2c, L3GD20H_ADDR, addr_high, 1, out_high, 1, 100)) != HAL_OK)
	{
		return status;
	}
	//left shift the data and OR with the data from the high register

	//if there was an error reading the low storage address, set output to -10000 and return the error

	if ((status = HAL_I2C_Mem_Read(hi2c, L3GD20H_ADDR, addr_low, 1, out_low, 1, 100)) != HAL_OK)
	{
		return status;
	}
	//OR the MSB with the data read from the register
	return HAL_OK;
}

