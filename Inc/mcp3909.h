/*
 * mcp3909.h
 *
 *  Created on: Jan 2, 2017
 *      Author: frank
 */

#ifndef MCP3909_H_
#define MCP3909_H_

#include "stm32l4xx_hal.h"

// NOTE: the Tx Buffer should always have the first byte empty for the CONTROL BYTE!
// Tx and Rx buffers must both be declared and allocated globally before the functions

// Register Addresses
#define CHANNEL_0   (0x0U)
#define CHANNEL_1   (0x1U)
#define CHANNEL_2   (0x2U)
#define CHANNEL_3   (0x3U)
#define CHANNEL_4   (0x4U)
#define CHANNEL_5   (0x5U)
#define MOD         (0x6U)
#define PHASE       (0x7U)
#define GAIN        (0x8U)
#define STATUS      (0x9U)
#define CONFIG      (0xAU)

// CHANNEL REGISTER START -----------------------------
// Channel data length
#define MAX_CHANNEL_NUM   (6U)
#define MAX_CHN_SET_NUM   (3U)
#define REG_LEN           (3U)
#define CTRL_LEN		  (1U)
#define CHN_GROUP_LEN     (2U) * REG_LEN
#define MOD_GROUP_LEN     (3U) * REG_LEN
#define STATUS_GROUP_LEN  (2U) * REG_LEN
#define CHN_TYPE_LEN      REG_LEN * MAX_CHANNEL_NUM
#define CONFIG_TYPE_LEN   MOD_GROUP_LEN + STATUS_GROUP_LEN
#define TOTAL_LEN         (11U) * REG_LEN
// CHANNEL REGISTER END   -----------------------------

// GAIN REGISTER START    -----------------------------
// PGA settings
#define PGA_1       (0U)
#define PGA_2       (1U)
#define PGA_4       (2U)
#define PGA_8       (3U)
#define PGA_16      (4U)
#define PGA_32      (5U)

#define BOOST_OFFSET     (3U)
#define PGA_BOOST_OFFSET (5U)

// Boost settings
#define BOOST_ON    (1U)
#define BOOST_OFF   (0U)
// GAIN REGISTER END      -----------------------------

// STATUS REGISTER START  -----------------------------
// Register read behavior
#define READ_SINGLE   (0U)
#define READ_GROUP    (1U)
#define READ_TYPE     (2U)
#define READ_ALL      (3U)

// Resolution
#define RES_24        (1U)
#define RES_16        (0U)

// 3 Cycle data latency for sinc3 filter settle time
#define DR_LTY_ON     (1U)
#define DR_LTY_OFF    (0U)

// High impedence when data NOT ready
#define DR_HIZ_ON     (0U)
#define DR_HIZ_OFF    (1U)

// Data ready link control
#define DR_LINK_ON    (1U)
#define DR_LINK_OFF   (0U)

// Data ready control Mode
#define DR_MODE_0    (0U)
#define DR_MODE_1    (1U)
#define DR_MODE_2    (2U)
#define DR_MODE_3    (3U)

// Bit offsets
#define DRSTATUS_CH_OFFSET  (0U)
#define DRA_MODE_OFFSET   (6U)
#define DRB_MODE_OFFSET   (8U)
#define DRC_MODE_OFFSET   (10U)
#define DR_LINK_OFFSET    (12U)
#define DR_HIZ_OFFSET     (13U)
#define DR_LTY_OFFSET     (14U)
#define CHN_RES_OFFSET    (15U)
#define READ_MODE_OFFSET  (22U)
// STATUS REGISTER END    -----------------------------


// CONFIG REGISTER START  -----------------------------
// Reset mode
#define RESET_ON          (1U)
#define RESET_OFF         (0U)

// Shutdown mode
#define SHUTDOWN_ON       (1U)
#define SHUTDOWN_OFF      (0U)

// Dither mode
#define DITHER_ON         (1U)
#define DITHER_OFF        (0U)

// Over Sampling ratio settings
#define OSR_32            (0U)
#define OSR_64            (1U)
#define OSR_128           (2U)
#define OSR_256           (3U)

// Prescaler settings
#define PRESCALE_1        (0U)
#define PRESCALE_2        (1U)
#define PRESCALE_4        (2U)
#define PRESCALE_8        (3U)

// External voltage reference select
#define EXTVREF_ON        (1U)
#define EXTVREF_OFF       (0U)

// External clock source select
#define EXTCLK_ON         (1U)
#define EXTCLK_OFF        (0U)

// Bit offsets
#define EXTCLK_OFFSET       (0U)
#define EXTVREF_OFFSET      (1U)
#define PRESCALE_OFFSET     (2U)
#define OSR_OFFSET          (4U)
#define DITHER_CHN_OFFSET   (6U)
#define SHUTDOWN_CHN_OFFSET (12U)
#define RESET_CHN_OFFSET    (18U)
// CONFIG REGISTER END    -----------------------------

// MCP3909 individual channel configurations
typedef struct {
  uint8_t   channel;      // Channel number
  uint8_t 	readType;
  uint8_t   PGA;          // ADC gain setting
  uint8_t   adcState;     // ADC operating mode selection
  uint8_t   dither;       // ADC dither filter
  uint8_t   resolution;   // ADC resolution
  uint8_t   boost;        // ADC boost mode
} Channel_Conf;

// MCP3909 Handle
typedef struct {
  SPI_HandleTypeDef *	hspi;	// SPI Handle object
  uint8_t *     pRxBuf;     // Rx Buffer
  uint8_t *     pTxBuf;     // Tx Buffer
  uint8_t		    readType;		// Read single, type, group, all registers
  uint8_t       prescale;
  uint8_t       osr;
  uint8_t       extCLK;
  uint8_t       extVREF;
  uint8_t       phase[MAX_CHN_SET_NUM];
  Channel_Conf  channel[MAX_CHANNEL_NUM];
} MCP3909HandleTypeDef;

// Internal Utility functions (All in DMA)
uint8_t _mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address);   // Send data that's stored in pTxBuf
uint8_t _mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t readType);    // Read data into default pRxBuf

// User library functions
// SPI Utility functions
uint8_t mcp3909_SPI_WriteReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * data); // Copies data into pTxBuf
uint8_t mcp3909_SPI_ReadReg(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer);  // Read data into user-defined buffer address
uint8_t mcp3909_SPI_ReadGroup(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer);  // Read data into user-defined buffer address
uint8_t mcp3909_SPI_ReadAll(MCP3909HandleTypeDef * hmcp, uint8_t address, uint8_t * buffer);  // Read data into user-defined buffer address

// Initialization
uint8_t mcp3909_init(MCP3909HandleTypeDef * hmcp);

// Enter low-power mode
uint8_t mcp3909_sleep(MCP3909HandleTypeDef * hmcp);

// Exit low-power mode
uint8_t mcp3909_wakeup(MCP3909HandleTypeDef * hmcp);

// Obtain channle info
uint8_t mcp3909_readAllChannels(MCP3909HandleTypeDef * hmcp, uint32_t * buffer);
uint8_t mcp3909_readAllChannel(MCP3909HandleTypeDef * hmcp, uint8_t channelNum, uint32_t * buffer);


#endif /* MCP3909_H_ */
