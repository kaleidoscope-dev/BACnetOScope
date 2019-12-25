#pragma once
#include "bacnet.h"
#include "bacapi.h"
#include "I_cBACServiceHandler.h"



class I_cAPDUDecoder
{
 public:
        I_cAPDUDecoder();
       ~I_cAPDUDecoder();

 public:
        void Init(bool routing_enable);

        int DecodePDUType(uint8_t *packetBuffer, BACNET_PDU_TYPE *pduType);
        
        // NOTE: blvc_len se koristi samo kod who_is zbog paddinga ako dolazi s neta
        // TODO: propraviti to.
        void DecodeAPDU(uint8_t *apdu, BACNET_NPDU_DATA *npdu, int apdulen, int bvlc_len);  
        int DecodeConfService(uint8_t *packetBuffer, BACNET_SERVICE_CHOICE_CONFIRMED *serviceData);
        int DecodeComplexACK(uint8_t *packetBuffer, BACNET_SERVICE_CHOICE_CONFIRMED *serviceData);
        void DecodeAndExecBVLC_NAK(uint8_t *packet, BACNET_BVLC *bvlc,uint16_t bvlc_result);
        void HandleAPDU(uint8_t *apdu, BACNET_NPDU_DATA *npdu, int apdulen, uint8_t bvlc_len);
        void DecodeAndRoute(uint8_t *apdu, BACNET_NPDU_DATA *npdu, int apdulen);
        void ClearSegmentBuffer(void);

        bool routingEnabled_; // hack. moramo razdvojiti API i Stack. za sada ovako

 private:
         int DecodeUnconfService(uint8_t *packetBuffer, BACNET_UNCONFIRMED_SERVICE *service);
         BACAPI_ERROR DecodeError(uint8_t *packet , int apdu_len, BACNET_ERROR *error, uint8_t *invoke_id, uint8_t *service, int *ret_len);
         int DecodeReject(uint8_t *packet , int apdu_len, BACNET_REJECT_CODE *rejCode, uint8_t *invoke_id);
         int DecodeAbort(uint8_t *packet , int apdu_len, BACNET_ABORT *abort);

 // interne privatne funkcije.
 private:
         bool IsSegmentedPacket(BACNET_SERVICE_CHOICE_CONFIRMED *service);
         bool IsSegmentedEnd(BACNET_SERVICE_CHOICE_CONFIRMED *service);
         int HandleSegmentedPacket(uint8_t *apdu, int apdulen, BACNET_SERVICE_CHOICE_CONFIRMED *service);
         uint8_t *HandleSegmentedEnd(uint8_t *apdu, int padulen, BACNET_SERVICE_CHOICE_CONFIRMED *service);
         void SortSegmentedPackets(void);
         bool SegmentOutOfSequence(int seq_num);


 private:
         I_cBACServiceHandler *serviceHandler_;
         vector<BACAPI_SEGMENT>vSegmentedPacket;
         uint8_t segmented_sequence_;
         


};
