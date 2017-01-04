/*
 * mcp3909.c
 *
 *  Created on: Jan 2, 2017
 *      Author: frank
 */

#include "mcp3909.h"
#include "freertos.h"

// Internal Utility functions (All in DMA)
inline uint8_t _mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address) {
	return mcp3909_SPI_WriteReg(hmcp, address, hmcp->pTxBuf, REG_LEN + CTRL_LEN);
}

inline uint8_t _mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t readType, uint8_t address){
	return mcp3909_SPI_ReadReg(hmcp, address, hmcp->pRxBuf, readType);
}

// User library functions
// SPI Utility functions

// DMA Tx Function
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

// DMA TRx function
uint8_t mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer, uint8_t readType) {
#ifdef DEBUG
	assert_param(hmcp);
	assert_param(address <= CONFIG);
	assert_param(readType <= READ_ALL);
#endif

	if(hmcp->readType != readType){
		hmcp->readType = readType;	// Update Handle status
		MODIFY_REG(hmcp->registers[STATUS], (0b11 << READ_MODE_OFFSET), readType << READ_MODE_OFFSET);	// Update register data

		// Assemble CONTROL BYTE to write STATUS register
		// | 0 | 1 | STATUS | W |
		// 0 1 0 1 0 0 1 0
		(hmcp->pTxBuf)[0] = 0x52;

		// uint32_t to uint8_t array
		(hmcp->pTxBuf)[3] = (hmcp->registers[STATUS]) & 0xFF;
		(hmcp->pTxBuf)[2] = ((hmcp->registers[STATUS]) >> 8)  & 0xFF;
		(hmcp->pTxBuf)[1] = ((hmcp->registers[STATUS]) >> 16) & 0xFF;

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

// Synchronous blocking Tx function - DO NOT USE IN RTOS AFTER SCHEDULER START UP
uint8_t mcp3909_SPI_WriteRegSync(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * data, uint8_t length, uint32_t timeout){
#ifdef DEBUG
	assert_param(address <= CONFIG);		// Address check
	assert_param(hmcp);						// Handle check
#endif

	// Assemble CONTROL BYTE
	// | 0 | 1 | A4 | A3 | A2 | A1 | A0 | W |
	(hmcp->pTxBuf)[0] = 0x40;
	(hmcp->pTxBuf)[0] |= address << 1;

	// Use DMA to transmit data to SPI
	if(HAL_SPI_Transmit(hmcp->hspi, hmcp->pTxBuf, REG_LEN + CTRL_LEN, timeout) == HAL_OK){
		return pdTRUE;
	} else {
		return pdFALSE;
	}
}

// Synchronous blokcing TRx function - DO NOT USE IN RTOS AFTER SCHEDULER START UP
uint8_t mcp3909_SPI_ReadRegSync(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer, uint8_t readType, uint32_t timeout) {
#ifdef DEBUG
	assert_param(hmcp);
	assert_param(address <= CONFIG);
	assert_param(readType <= READ_ALL);
#endif

	if(hmcp->readType != readType){
		hmcp->readType = readType;	// Update Handle status
		MODIFY_REG(hmcp->registers[STATUS], (0b11 << READ_MODE_OFFSET), readType << READ_MODE_OFFSET);	// Update register data

		// Assemble CONTROL BYTE to write STATUS register
		// | 0 | 1 | STATUS | W |
		// 0 1 0 1 0 0 1 0
		(hmcp->pTxBuf)[0] = 0x52;

		// uint32_t to uint8_t array
		(hmcp->pTxBuf)[3] = (hmcp->registers[STATUS]) & 0xFF;
		(hmcp->pTxBuf)[2] = ((hmcp->registers[STATUS]) >> 8)  & 0xFF;
		(hmcp->pTxBuf)[1] = ((hmcp->registers[STATUS]) >> 16) & 0xFF;

		if(mcp3909_SPI_WriteRegSync(hmcp, STATUS, hmcp->pTxBuf, REG_LEN + CTRL_LEN, timeout) != pdTRUE){
			return pdFALSE;
		}
	}

	// Modify CONTROL BYTE
	// | 0 | 1 | A4 | A3 | A2 | A1 | R |
	(hmcp->pTxBuf)[0] = 0x1;    // Read control frame (0b01000001)
	(hmcp->pTxBuf)[0] |= address << 1;

	// Use synchronous blocking call to transmit and receive data from SPI
	if(HAL_SPI_TransmitReceive(hmcp->hspi, hmcp->pTxBuf, buffer, REG_LEN + CTRL_LEN, timeout) == HAL_OK){
		return pdTRUE;
	} else {
		return pdFALSE;
	}
}

inline uint8_t mcp3909_SPI_ReadGroup(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){
	return mcp3909_SPI_ReadReg(hmcp, address, buffer, READ_GROUP);
}

inline uint8_t mcp3909_SPI_ReadType(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){
	return mcp3909_SPI_ReadReg(hmcp, address, buffer, READ_TYPE);
}

inline uint8_t mcp3909_SPI_ReadAll(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer){
	return mcp3909_SPI_ReadReg(hmcp, address, buffer, READ_ALL);
}

// Initialization
uint8_t mcp3909_init(MCP3909HandleTypeDef * hmcp){
  // Global chip settings:
  // First byte of REG_CONFIG
  hmcp->registers[CONFIG] |= hmcp->extCLK;							// Clock source setting
  hmcp->registers[CONFIG] |= (hmcp->extVREF) << EXTVREF_OFFSET;		// Voltage reference setting
  hmcp->registers[CONFIG] |= (hmcp->prescale) << PRESCALE_OFFSET;	// Prescaler setting
  hmcp->registers[CONFIG] |= (hmcp->osr) << OSR_OFFSET;				// Over Sampling Ratio setting

  hmcp->registers[STATUS] |= (hmcp->readType) << READ_MODE_OFFSET;	// Read configuration to register type
  hmcp->registers[STATUS] |= DR_LTY_ON << DR_LTY_OFFSET;			// 3 Cycle latency to let sinc3 settle

  // Channel pair phase delays
  // Set phase registers
  hmcp->registers[PHASE] = bytesToReg(hmcp->phase);

  // Channel specific settings:
  for(uint8_t i = 0; i < MAX_CHANNEL_NUM; i++){
	  hmcp->registers[CONFIG] |= (hmcp->channel[i].dither) << (DITHER_CHN_OFFSET+i);	// Dither controls

	  // Set mode at last; ADC_SHUTDOWN will override clock and vref source settings
	  hmcp->registers[CONFIG] |= (hmcp->channel[i].shutdown) << (SHUTDOWN_CHN_OFFSET+i);
	  hmcp->registers[CONFIG] |= (hmcp->channel[i].reset) << (RESET_CHN_OFFSET+i);

	  // Set resolution
	  hmcp->registers[STATUS] |= ((hmcp->channel[i]).resolution) << (RES_CHN_OFFSET+i);

	  // Set channel gain
	  hmcp->registers[GAIN] |= ((hmcp->channel[i]).PGA) << (PGA_BOOST_LEN*i);
	  hmcp->registers[GAIN] |= ((hmcp->channel[i]).boost) << (PGA_BOOST_LEN*i + BOOST_OFFSET);
  }

  uint8_t tempRegBytes[3];
  regToBytes(&(hmcp->registers[PHASE]),tempRegBytes);
  if(mcp3909_SPI_WriteRegSync(hmcp, PHASE, tempRegBytes, REG_LEN + CTRL_LEN, SPI_TIMEOUT) != pdTRUE){
	  return pdFALSE;
  }

  regToBytes(&(hmcp->registers[GAIN]),tempRegBytes);
  if(mcp3909_SPI_WriteRegSync(hmcp, GAIN, tempRegBytes, REG_LEN + CTRL_LEN, SPI_TIMEOUT) != pdTRUE){
  	  return pdFALSE;
  }

  regToBytes(&(hmcp->registers[STATUS]),tempRegBytes);
  if(mcp3909_SPI_WriteRegSync(hmcp, STATUS, tempRegBytes, REG_LEN + CTRL_LEN, SPI_TIMEOUT) != pdTRUE){
	  return pdFALSE;
  }

  regToBytes(&(hmcp->registers[CONFIG]),tempRegBytes);
  if(mcp3909_SPI_WriteRegSync(hmcp, CONFIG, tempRegBytes, REG_LEN + CTRL_LEN, SPI_TIMEOUT) != pdTRUE){
  	  return pdFALSE;
  }

  // TODO: Delay 50us


  return mcp3909_verify(hmcp);
}

// Setting verification
// Returns 1 if verificaiton success
// Returns 0 if verification failed or error
uint8_t mcp3909_verify(MCP3909HandleTypeDef * hmcp){
	if(mcp3909_SPI_ReadRegSync(hmcp, MOD, (hmcp->pRxBuf), READ_TYPE, SPI_TIMEOUT) != pdTRUE){
		return pdFALSE;
	}
	uint32_t tempRegister = 0;
	for(uint8_t i = MAX_CHANNEL_NUM + 1; i < REGS_NUM; i++){
		// Ignore the MOD register output values
		tempRegister = bytesToReg((hmcp->pRxBuf) + (i - MAX_CHANNEL_NUM) * REG_LEN);	// Assemble the bytes into register data
		if((hmcp->registers)[i] != tempRegister){
			return pdFALSE;
		}
	}
	return pdTRUE;
}


// Enter low-power mode
uint8_t mcp3909_sleep(MCP3909HandleTypeDef * hmcp){
	// Update Handle object
	for(uint8_t i = 0 ; i < MAX_CHANNEL_NUM; i++){
		hmcp->channel[i].shutdown = SHUTDOWN_ON;
		hmcp->channel[i].reset = RESET_ON;
	}

	// Assemble register data
	hmcp->registers[CONFIG] |= (0x1F << SHUTDOWN_CHN_OFFSET);
	hmcp->registers[CONFIG] |= (0x1F << RESET_CHN_OFFSET);

	// Load CONFIG register data to SPI Tx buffer
	(hmcp->pTxBuf)[3] = (hmcp->registers[CONFIG]) & 0xFF;
	(hmcp->pTxBuf)[2] = ((hmcp->registers[CONFIG]) >> 8)  & 0xFF;
	(hmcp->pTxBuf)[1] = ((hmcp->registers[CONFIG]) >> 16) & 0xFF;

	// TODO: Disable GPIO DR Interrupt

	// Write a single register configuration
	return _mcp3909_SPI_WriteReg(hmcp, CONFIG);
}

// Exit low-power mode
uint8_t mcp3909_wakeup(MCP3909HandleTypeDef * hmcp){
	// Update Handle object
	for(uint8_t i = 0 ; i < MAX_CHANNEL_NUM; i++){
		hmcp->channel[i].shutdown = SHUTDOWN_OFF;
		hmcp->channel[i].reset = RESET_OFF;
	}

	// Assemble register data
	hmcp->registers[CONFIG] |= 0x0FFF;

	// Load CONFIG register data to SPI Tx buffer
	(hmcp->pTxBuf)[3] = (hmcp->registers[CONFIG]) & 0xFF;
	(hmcp->pTxBuf)[2] = ((hmcp->registers[CONFIG]) >> 8)  & 0xFF;
	(hmcp->pTxBuf)[1] = ((hmcp->registers[CONFIG]) >> 16) & 0xFF;

	if(_mcp3909_SPI_WriteReg(hmcp, CONFIG)){
		// TODO: Delay 50us power on reset time

		// TODO: Enable GPIO DR Interrupt

		return pdTRUE;
	}
	return pdFALSE;
}

// Obtain channel info
inline uint8_t  mcp3909_readAllChannels(MCP3909HandleTypeDef * hmcp, uint8_t * buffer){
	return mcp3909_SPI_ReadType(hmcp, CHANNEL_0, buffer);
}

inline uint8_t mcp3909_readChannel(MCP3909HandleTypeDef * hmcp, uint8_t channelNum, uint8_t * buffer){
#ifdef DEBUG
	assert_param(channelNum < MAX_CHANNEL_NUM);
#endif
	return mcp3909_SPI_ReadType(hmcp, channelNum, buffer);
}

inline uint32_t bytesToReg(uint8_t * byte){
	return (byte[2] | (byte[1] << 8) | (byte[0] << 16));
}

inline void regToBytes(uint32_t * reg, uint8_t * bytes){
	bytes[2] = (*reg) & 0xFF;
	bytes[1] = ((*reg) >> 8)  & 0xFF;
	bytes[0] = ((*reg) >> 16) & 0xFF;
}

inline void mcp3909_parseChannelData(MCP3909HandleTypeDef * hmcp){
	for(uint8_t i = CHANNEL_0; i < CHANNEL_0+MAX_CHANNEL_NUM; i++){
		(hmcp->registers)[i] = bytesToReg((hmcp->pRxBuf) + REG_LEN * i);
	}
}
