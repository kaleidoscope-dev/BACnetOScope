#pragma once
#include "bacnet.h"



class I_cNPDUEncoder
{
 public:
        I_cNPDUEncoder();
       ~I_cNPDUEncoder();
       
 public:
        void SetNPDUData(BACNET_NPDU_DATA *npduData, bool reply, int bacnetMessagePriority, int hopcount, bool net_message = false);       
        int EncodeNPDU(uint8_t *buffer, BACNET_NPDU_DATA *npduData, BACNET_ADDRESS *dest, BACNET_ADDRESS *src);
        int EncodeBVLC(uint8_t *buffer, uint16_t len, int transmission);
        
        int EncodeBVLC_WriteBDT(uint8_t *buffer, BACNET_BVLC_BDT *table);
        
        int EncodeBVLC_RegisterFD(uint8_t *buffer, uint16_t ttl);
        int EncodeBVLC_ReadFDT(uint8_t *buffer);
        int EncodeBVLC_DeleteFDTEntry(uint8_t *buffer, uint32_t adress, uint16_t port);
};