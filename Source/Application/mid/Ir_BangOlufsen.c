#include "main.h"

/************************************
 * Start of send and decode functions
 ************************************/

/*
 * TODO aNumberOfRepeats are handled not correctly if ENABLE_BEO_WITHOUT_FRAME_GAP is defined
 */
void sendBangOlufsen(uint16_t aHeader, uint8_t aData, uint8_t aNumberOfRepeats, int8_t aNumberOfHeaderBits)
{
//    for (int8_t i = 0; i < aNumberOfRepeats + 1; ++i)
//    {
//        sendBangOlufsenRaw((uint32_t(aHeader) << 8) | aData, aNumberOfHeaderBits + 8, i != 0);
//    }
}

void sendBangOlufsenDataLink(uint32_t aHeader, uint8_t aData, uint8_t aNumberOfRepeats, int8_t aNumberOfHeaderBits)
{
//    for (int8_t i = 0; i < aNumberOfRepeats + 1; ++i)
//    {
//        sendBangOlufsenRawDataLink((uint64_t(aHeader) << 8) | aData, aNumberOfHeaderBits + 8, i != 0, true);
//    }
}
