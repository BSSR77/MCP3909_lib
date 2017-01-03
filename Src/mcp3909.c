/*
 * mcp3909.c
 *
 *  Created on: Jan 2, 2017
 *      Author: frank
 */

#include "mcp3909.h"

uint8_t mcp3909_SPI_Write(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t length) {

  // Wake the chip up if it's currently in off state
  if(hmcp->state != STATE_INIT && hmcp->state == STATE_OFF){
    mcp3909_wakeup(hmcp);
  }

  // Aseemble CONTROL BYTE
  // | 0 | 1 | A4 | A3 | A2 | A1 | R/!W |
  uint8_t CONTROL_BYTE = 0;
  CONTROL_BYTE = address << 1;
  CONTROL_BYTE |= 1 << 6;
  CONTROL_BYTE |= 1;    // Write to address

  hmcp->pTxBuf[0] = CONTROL_BYTE;	// Set the first byte of the Tx Buffer to CONTROL_BYTE

  // TODO: SPI Send control byte and data
}

uint8_t mcp3909_init(SPI_HandleTypeDef * hspi, MCP3909HandleTypeDef * hmcp){
  hmcp->hspi = hspi;
  // Set up temporary register containers for modification
  uint32_t REG_PHASE = 0;
  uint32_t REG_GAIN = 0;
  uint32_t REG_STATUS = 0;
  uint32_t REG_CONFIG = 0;

  // Global chip settings:
  // First byte of REG_CONFIG
  REG_CONFIG |= hmcp->extCLK;			// Clock source setting
  REG_CONFIG |= (hmcp->extVREF) << 1;	// Voltage reference setting
  REG_CONFIG |= (hmcp->prescale) << 2;	// Prescaler setting
  REG_CONFIG |= (hmcp->osr) << 4;		// Over Sampling Ratio setting

  REG_STATUS |= READ_TYPE << 22;		// Read configuration to register type
  REG_STATUS |= 1 << 14;				// 3 Cycle latency to let sinc3 settle

  // Channel specific settings:
  for(uint8_t i = 0; i < MAX_CHANNELS; i++){
	  REG_CONFIG |= (hmcp->channel[i].dither) << (6+i);	// Dither controls

	  // Set mode at last; ADC_SHUTDOWN will override clock and vref source settings
	  switch((hmcp->channel[i]).mode){
	  case ADC_SHUTDOWN:
		  REG_CONFIG |= EXT_CLK;
		  REG_CONFIG |= EXT_VREF << 1;
		  REG_CONFIG |= 1 << (12+i);
		  REG_CONFIG |= 0 << (18+i);
		  break;

	  case	ADC_RESET:
		  REG_CONFIG |= 0 << (12+i);
		  REG_CONFIG |= 1 << (18+i);
		  break;

	  case ADC_ON:
		  REG_CONFIG |= 0 << (12+i);
		  REG_CONFIG |= 0 << (18+i);
		  break;
	  }

	  // Set resolution
	  REG_STATUS |= ((hmcp->channel[i]).resolution) << (15+i);

	  // Set channel gain
	  REG_GAIN |= ((hmcp->channel[i]).PGA) << (4*i);
	  REG_GAIN |= ((hmcp->channel[i]).boost) << (4*i + 3);
  }

  // Set phase registers
  REG_PHASE |= (hmcp->phase[0]);		// CH4 & CH5
  REG_PHASE |= (hmcp->phase[1]) << 8;	// CH3 & CH2
  REG_PHASE |= (hmcp->phase[2]) << 16;	// CH1 & CH0

  // TODO: Check the conversion code below
  uint8_t txTemp[3];
  for(uint8_t i = 2; i >= 0; i--){
	  txTemp[i] = 0;
	  txTemp[i] = (REG_PHASE >> 8*(2-i)) & (0xFF);
  }
  // TODO: SPI SEND

  for(uint8_t i = 2; i >= 0; i--){
	txTemp[i] = 0;
	txTemp[i] = (REG_GAIN >> 8*(2-i)) & (0xFF);
  }
  // TODO: SPI SEND

  for(uint8_t i = 2; i >= 0; i--){
	txTemp[i] = 0;
	txTemp[i] = (REG_STATUS >> 8*(2-i)) & (0xFF);
  }
  // TODO: SPI SEND

  for(uint8_t i = 2; i >= 0; i--){
	txTemp[i] = 0;
	txTemp[i] = (REG_CONFIG >> 8*(2-i)) & (0xFF);
  }
  // TODO: SPI SEND

  return mcp3909_verify(hmcp);
}
