/*
 * nodeHelpers.h
 *
 *  Created on: Jan 4, 2017
 *      Author: frank
 */

#ifndef NODEHELPERS_H_
#define NODEHELPERS_H_

// Microsecond delay
// Multiply by 20 for O2 and O3
// Multiply by 16 for O0, O1, and Os
#ifdef DEBUG
#define delayUs(US) 	_delayUS_ASM(US * 12)
#else
#define delayUs(US) 	_delayUS_ASM(US * 20)
#endif

#define _delayUS_ASM(X) \
	asm volatile (	"MOV R0,#" #X  "\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t"\
		      );\


#endif /* NODEHELPERS_H_ */
