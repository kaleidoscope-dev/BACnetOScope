#pragma once
#include "bacnet.h"



class I_cNPDUDecoder
{
 public:
        I_cNPDUDecoder();
       ~I_cNPDUDecoder();
       
 public:
        int DecodeNPDU(uint8_t *packet, BACNET_NPDU_DATA *npduData);
        

        
        int DecodeBVLC(uint8_t *packet, BACNET_BVLC *bvlc, uint16_t *blvc_result, BACNET_ADDRESS *source);      
        int DecodeLLC(uint8_t *packet, BACNET_LLC *llc);  
        int DecodeBVLC_BDTable(BACNET_BVLC_BDT *table, uint8_t *packet, int size);
        int DecodeBVLC_FDTable(BACNET_BVLC_FDT *table, uint8_t *packet, int size);
        BACNET_BVLC_FDT_ENTRY *DecodeBVLC_RegisterFD(uint8_t *packet, int size);
        
        void HandleBVLCResult(BACNET_ADDRESS *source, uint16_t result);
        
        
};