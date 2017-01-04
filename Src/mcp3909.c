/*
 * mcp3909.c
 *
 *  Created on: Jan 2, 2017
 *      Author: frank
 */

#include "mcp3909.h"
#include "freertos.h"

// Internal Utility functions (All in DMA)
uint8_t inline _mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address) {
	return mcp3909_SPI_WriteReg(hmcp, address, hmcp->pTxBuf, REG_LEN + CTRL_LEN);
}

uint8_t inline _mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t readType, uint8_t address){
	return mcp3909_SPI_ReadReg(hmcp, address, hmcp->pRxBuf, readType);
}

// User library functions
// SPI Utility functions
uint8_t mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * data, uint8_t length){
#ifdef DEBUG
	assert_param(address <= CONFIG);		// Address check
	assert_param(hmcp);						// Handle check
#endif

	// Assemble CONTROL BYTE
	// | 0 | 1 | A4 | A3 | A2 | A1 | A0 | W |
	(hmcp->pTxBuf)[0] = 0x40;
	(hmcp->pTxBuf)[0] |= address << 1;

	// Use DMA to transmit data to SPI
	if(HAL_SPI_Transmit_DMA(hmcp->hspi, hmcp->pTxBuf, REG_LEN + CTRL_LEN) == HAL_OK){
		return pdTRUE;
	} else {
		return pdFALSE;
	}
}

uint8_t mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer, uint8_t readType) {
#ifdef DEBUG
	assert_param(hmcp);
	assert_param(address <= CONFIG);
	assert_param(readType <= READ_ALL);
#endif

	if(hmcp->readType != readType){
		hmcp->readType = readType;	// Update Handle status
		MODIFY_REG(hmcp->registers[STATUS - MAX_CHANNEL_NUM], (0b11 << READ_MODE_OFFSET), readType << READ_MODE_OFFSET);	// Update register data

		// Assemble CONTROL BYTE to write STATUS register
		// | 0 | 1 | STATUS | W |
		// 0 1 0 1 0 0 1 0
		(hmcp->pTxBuf)[0] = 0x52;

		// uint32_t to uint8_t array
		(hmcp->pTxBuf)[3] = (hmcp->registers[STATUS - MAX_CHANNEL_NUM]) & 0xFF;
		(hmcp->pTxBuf)[2] = ((hmcp->registers[STATUS - MAX_CHANNEL_NUM]) >> 8)  & 0xFF;
		(hmcp->pTxBuf)[1] = ((hmcp->registers[STATUS - MAX_CHANNEL_NUM]) >> 16) & 0xFF;

		if(_mcp3909_SPI_WriteReg(hmcp, STATUS) != pdTRUE){
			return pdFALSE;
		}
	}

	// Modify CONTROL BYTE
	// | 0 | 1 | A4 | A3 | A2 | A1 | R |
	(hmcp->pTxBuf)[0] = 0x1;    // Read control frame (0b01000001)
	(hmcp->pTxBuf)[0] |= address << 1;

	// Use DMA to transmit and receive data from SPI
	if(HAL_SPI_TransmitReceive_DMA(hmcp->hspi, hmcp->pTxBuf, buffer, REG_LEN + CTRL_LEN) == HAL_OK){
		return pdTRUE;
	} else {
		return pdFALSE;
	}
}

uint8_t inline mcp3909_SPI_ReadGroup(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){
	return mcp3909_SPI_ReadReg(hmcp, address, buffer, READ_GROUP);
}

uint8_t inline mcp3909_SPI_ReadType(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){
	return mcp3909_SPI_ReadReg(hmcp, address, buffer, READ_TYPE);
}

uint8_t inline mcp3909_SPI_ReadAll(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){
	return mcp3909_SPI_ReadReg(hmcp, address, buffer, READ_ALL);
}

// Initialization
uint8_t mcp3909_init(MCP3909HandleTypeDef * hmcp){

}

// Enter low-power mode
uint8_t mcp3909_sleep(MCP3909HandleTypeDef * hmcp){
	// Update Handle object
	for(uint8_t i = 0 ; i < MAX_CHANNEL_NUM; i++){
		hmcp->channel[i]->shutdown = SHUTDOWN_ON;
		hmcp->channel[i]->reset = RESET_ON;
	}

	// Assemble register data
	hmcp->registers[CONFIG - MAX_CHANNEL_NUM] |= (0x1F << SHUTDOWN_CHN_OFFSET);
	hmcp->registers[CONFIG - MAX_CHANNEL_NUM] |= (0x1F << RESET_CHN_OFFSET);

	// Load CONFIG register data to SPI Tx buffer
	(hmcp->pTxBuf)[3] = (hmcp->registers[CONFIG - MAX_CHANNEL_NUM]) & 0xFF;
	(hmcp->pTxBuf)[2] = ((hmcp->registers[CONFIG - MAX_CHANNEL_NUM]) >> 8)  & 0xFF;
	(hmcp->pTxBuf)[1] = ((hmcp->registers[CONFIG - MAX_CHANNEL_NUM]) >> 16) & 0xFF;

	// Disable GPIO DR Interrupt

	// Write a single register configuration
	return _mcp3909_SPI_WriteReg(hmcp, CONFIG);
}

// Exit low-power mode
uint8_t mcp3909_wakeup(MCP3909HandleTypeDef * hmcp){
	// Update Handle object
	for(uint8_t i = 0 ; i < MAX_CHANNEL_NUM; i++){
		hmcp->channel[i]->shutdown = SHUTDOWN_OFF;
		hmcp->channel[i]->reset = RESET_OFF;
	}

	// Assemble register data
	hmcp->registers[CONFIG - MAX_CHANNEL_NUM] |= 0x0FFF;

	// Load CONFIG register data to SPI Tx buffer
	(hmcp->pTxBuf)[3] = (hmcp->registers[CONFIG - MAX_CHANNEL_NUM]) & 0xFF;
	(hmcp->pTxBuf)[2] = ((hmcp->registers[CONFIG - MAX_CHANNEL_NUM]) >> 8)  & 0xFF;
	(hmcp->pTxBuf)[1] = ((hmcp->registers[CONFIG - MAX_CHANNEL_NUM]) >> 16) & 0xFF;

	if(_mcp3909_SPI_WriteReg(hmcp, CONFIG)){
		// TODO: Delay 50us power on reset time

		// TODO: Enable GPIO DR Interrupt

		return pdTRUE;
	}
	return pdFALSE;
}

// Obtain channel info
uint8_t mcp3909_readAllChannels(MCP3909HandleTypeDef * hmcp, uint32_t * buffer){

}

uint8_t mcp3909_readAllChannel(MCP3909HandleTypeDef * hmcp, uint8_t channelNum, uint32_t * buffer){

}
