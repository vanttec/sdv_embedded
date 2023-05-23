/*
 * mcp45hvx1.c
 *
 *  Created on: Apr 17, 2023
 *      Author: Edison
 */
#include "mcp45hvx1.h"
#include "stdbool.h"
#include <string.h>
#include "stm32l4xx_hal.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "main.h"
uint8_t pot_address = 120;
TCON_Register tcon;
bool enable_potentiometer = false;
extern I2C_HandleTypeDef hi2c1;

void initialize_devices()
{
  //
  HAL_StatusTypeDef status =
      HAL_I2C_IsDeviceReady(&hi2c1, pot_address, 3u, 10u);
  if (HAL_OK == status)
  {
    enable_potentiometer = true;
  }
}
void begin_pot()
{
  if (enable_potentiometer)
  {
    uint8_t initialValue = 127;
    write_register(0b00, &initialValue, 1);
    HAL_GPIO_WritePin(WLAT_GPIO_Port, WLAT_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SHDN_GPIO_Port, SHDN_Pin, GPIO_PIN_SET);
    defaultTCON();
  }
}

/* Wiper Register..........................................................*/
void writeWiper(uint8_t wiperValue)
{
  if (enable_potentiometer)
  {
    write_register(0x00, &wiperValue, 1);
  }
}

void read_register(uint8_t register_address, uint8_t *read_buffer,
                   uint16_t size)
{
  send_command(register_address, 0b11, NULL, 0);

  if (HAL_I2C_Master_Receive(&hi2c1, pot_address, read_buffer, size, 100) !=
      HAL_OK)
  {
    Error_Handler();
  };
}

#define MAX_REGISTER_SIZE 32
uint8_t i2cBuffer[MAX_REGISTER_SIZE];

void send_command(uint8_t address, uint8_t command, const uint8_t *data, uint32_t size)
{
  if (size > MAX_REGISTER_SIZE)
  {
    Error_Handler();
  }

  i2cBuffer[0] = address << 4 | (command & 0b11) << 2;
  memcpy(i2cBuffer + 1, data, size);
  HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(
      &hi2c1, pot_address, i2cBuffer, size + 1, HAL_MAX_DELAY);

  if (ret != HAL_OK)
  {
    Error_Handler();
  }
}

void write_register(uint8_t address, const uint8_t *data, uint32_t size)
{
  send_command(address, 0b00, data, size);
}

uint8_t readWiper()
{
  uint8_t buf[2];
  read_register(0b00, buf, 2);
  return buf[1];
}

/* TCON Register...........................................................*/
uint8_t readTCON()
{
  uint8_t buf[2];
  read_register(0x4, buf, 2);
  return buf[1];
  // uint8_t write_info[1];
  // write_info[0] = MEM_TCON | COM_READ;
  // write_register(write_info, 1);
  // uint8_t reg_ptr = MEM_TCON | COM_READ;  // First set the register pointer to
  //                                         // the register wanted to be read
  // uint8_t buffer[2];  // First byte is 0x00 and // Second byte contains the
  //                     // wiper value (for compatability)
  // read_register(reg_ptr, buffer, 2);
  // return buffer;
}

void defaultTCON()
{
  tcon.R0HW = true;
  tcon.R0A = true;
  tcon.R0B = true;
  tcon.R0W = true;

  write_TCON_Register();
}

void writeTCON(TCON_Register *inReg)
{
  memcpy(&tcon, inReg, sizeof(tcon));
  write_TCON_Register();
}

void write_TCON_R0HW(bool isOn)
{
  tcon.R0HW = isOn;
  write_TCON_Register();
}

void write_TCON_R0A(bool isOn)
{
  tcon.R0A = isOn;
  write_TCON_Register();
}

void write_TCON_R0W(bool isOn)
{
  tcon.R0W = isOn;
  write_TCON_Register();
}

void write_TCON_R0B(bool isOn)
{
  tcon.R0B = isOn;
  write_TCON_Register();
}

void write_TCON_Register()
{
  uint8_t buff = 0xFF;

  tcon.R0HW ? (buff |= TCON_R0HW) : (buff ^= TCON_R0HW);
  tcon.R0A ? (buff |= TCON_R0A) : (buff ^= TCON_R0A);
  tcon.R0B ? (buff |= TCON_R0B) : (buff ^= TCON_R0B);
  tcon.R0W ? (buff |= TCON_R0W) : (buff ^= TCON_R0W);
  buff = 0b1111111;

  write_register(0x4, &buff, 1);
  // write_register(write_info, 2);
}
