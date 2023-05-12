/*
* mcp45hvx1.h
*
*  Created on: Apr 17, 2023
*      Author: edison
*/

#ifndef MCP45HVX1_H_
#define MCP45HVX1_H_
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
               bool R0HW;
               bool R0A;
               bool R0B;
               bool R0W;
}TCON_Register;
//uint8_t pot_address;

//TCON_Register TCON_lib_reg;

void begin_pot();
void write_TCON_Register();
void write_TCON_R0HW(bool state);
void write_TCON_R0A(bool state);
void write_TCON_R0W(bool state);
void write_TCON_R0B(bool state);
/* Wiper Register ...................................................... */
uint8_t readWiper();
void read_register(uint8_t register_address, uint8_t* read_buffer,uint16_t size);

void send_command(uint8_t address, uint8_t command, const uint8_t *data, uint32_t size);
void write_register(uint8_t address, const uint8_t* data, uint32_t size);

void writeWiper(uint8_t wiperValue);

/* 7 Bit I2C Operation Components ......................................... */

/* TCON configuration.................. */
#define TCON_R0HW (0x08)  // Shutdown Resistor Force
#define TCON_R0A  (0x04)  // Terminal A Connection
#define TCON_R0W  (0x02)  // Wiper Connection
#define TCON_R0B  (0x01)  // Terminal B Connection

#define GCALL_TCON          (0x60)
#define GCALL_WIPER         (0x40)
#define GCALL_WIPERUP       (0x42)
#define GCALL_WIPERDWN      (0x44)
#define GCALL_COM_WRITE     (0x02)
#define GCALL_COM_RWRITE    (0x03)
#define GCALL_COM_WIPERINC  (0x42)
#define GCALL_COM_WIPERDEC  (0x44)

#define MEM_WIPER           (0x00)
#define MEM_TCON            (0x40)

#define COM_WRITE           (0x00)
#define COM_READ            (0x0C)
#define COM_WIPERINC        (0x04)
#define COM_WIPERDEC        (0x08)



/* TCON Register ....................................................... */
uint8_t readTCON();
void writeTCON(TCON_Register *inReg);
void defaultTCON();

void inline shutdown()
{
               write_TCON_R0HW(false);
};
void inline startup()
{
               write_TCON_R0HW(true);
};
void inline connectTerminalA()
{
               write_TCON_R0A(true);
};
void inline disconnectTerminalA()
{
               write_TCON_R0A(false);
};
void inline connectTerminalB()
{
               write_TCON_R0B(true);
};
void inline disconnectTerminalB()
{
               write_TCON_R0B(false);
};
void inline connectWiper()
{
               write_TCON_R0W(true);
};
void inline disconnectWiper()
{
               write_TCON_R0W(false);
};

#endif /* MCP45HVX1_H_ */





