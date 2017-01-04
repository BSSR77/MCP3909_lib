/*
 * mcp3909.c
 *
 *  Created on: Jan 2, 2017
 *      Author: frank
 */

#include "mcp3909.h"

// Internal Utility functions (All in DMA)
uint8_t _mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address) {

}

uint8_t _mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t readType){


}
// User library functions
// SPI Utility functions
uint8_t mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * data){

}

uint8_t mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer) {

}

uint8_t mcp3909_SPI_ReadGroup(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){

}

uint8_t mcp3909_SPI_ReadAll(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){

}

// Initialization
uint8_t mcp3909_init(MCP3909HandleTypeDef * hmcp){

}

// Enter low-power mode
uint8_t mcp3909_sleep(MCP3909HandleTypeDef * hmcp){

}

// Exit low-power mode
uint8_t mcp3909_wakeup(MCP3909HandleTypeDef * hmcp){

}

// Obtain channel info
uint8_t mcp3909_readAllChannels(MCP3909HandleTypeDef * hmcp, uint32_t * buffer){

}

uint8_t mcp3909_readAllChannel(MCP3909HandleTypeDef * hmcp, uint8_t channelNum, uint32_t * buffer){

}
