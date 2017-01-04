/*
 * mcp3909.c
 *
 *  Created on: Jan 2, 2017
 *      Author: frank
 */

#include "mcp3909.h"
#include "freertos.h"

// Internal Utility functions (All in DMA)
uint8_t _mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address) {
	assert_param(address <= 0x0A);		// Address check
	assert_param(hmcp);					// Handle check

	// Assemble CONTROL BYTE
	// | 0 | 1 | A4 | A3 | A2 | A1 | R/!W |
	(hmcp->pTxBuf)[0] = 0;
	(hmcp->pTxBuf)[0] = address << 1;
	(hmcp->pTxBuf)[0] |= 1 << 6;
	(hmcp->pTxBuf)[0] |= 1;    // Write to address

	// Use DMA to transmit data to SPI
	if(HAL_SPI_Transmit_DMA(hmcp->hspi, hmcp->pTxBuf, REG_LEN + CTRL_LEN) == HAL_OK){
		return pdTRUE;
	} else {
		return pdFALSE;
	}
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
