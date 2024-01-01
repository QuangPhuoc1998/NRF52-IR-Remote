#ifndef _OP_HEADER_VARIABLE_H_
#define _OP_HEADER_VARIABLE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef GLOBAL_DEFINE
#define HEADER_VARIABLE
#else
#define HEADER_VARIABLE extern
#endif

// Variable for timer count
HEADER_VARIABLE uint8_t g_ubScanIRCount;
HEADER_VARIABLE volatile uint8_t g_ubDelayCount;
// Varialbe for IR modele
HEADER_VARIABLE struct irparams_struct irparams;
HEADER_VARIABLE IRData decodedIRData;
HEADER_VARIABLE decode_type_t lastDecodedProtocol;
HEADER_VARIABLE uint32_t lastDecodedCommand;
HEADER_VARIABLE uint32_t lastDecodedAddress;
// Varialbe for IR code
HEADER_VARIABLE IRData_t t_IRDataCommom[MAX_IR_CODE];
HEADER_VARIABLE uint8_t g_ubIRCount;

#endif
