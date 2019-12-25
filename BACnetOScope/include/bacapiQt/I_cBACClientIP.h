#pragma once

#include "bacapi.h"
#include "I_cNPDUEncoder.h"
#include "I_cAPDUEncoder.h"
#include "I_cNPDUDecoder.h"
#include "I_cAPDUDecoder.h"
#include "I_cDecoder.h"


class I_cBACClientIP
{
 public:
        I_cBACClientIP();
       ~I_cBACClientIP();



 public:
        BACAPI_ERROR Init(unsigned short port, I_cNPDUEncoder *npdu, I_cAPDUEncoder *apdu, I_cNPDUDecoder *decnpdu, I_cAPDUDecoder *decapdu, I_cDecoder *dec);

 public:
 /*
        BACAPI_ERROR WhoIs(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_ADDRESS *adr, bool params);
        BACAPI_ERROR WhoHas(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_OBJECT_ID *oid, BACNET_ADDRESS *adr);

        BACAPI_ERROR ReadProperty(BACNET_OBJECT_ID *oid, BACNET_PROPERTY_ID prop, int arrayIndex, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadPropertyALL(BACNET_OBJECT_ID *oid, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadRange(BACNET_READ_RANGE *range, uint8_t invokeId,  BACNET_ADDRESS *net, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadRange_ACK(BACNET_OBJECT_ID *oid , BACNET_OBJECT_PROPERTY *property, BACNET_READ_RANGE *range, uint8_t invokeId,  BACNET_ADDRESS *net, BACNET_ADDRESS *dest);


        BACAPI_ERROR WriteProperty(BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *prop,int invokeId,BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR GetAlarmSummary(int invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);
        BACAPI_ERROR GetEnrollmentSummary(BACAPI_ENROLLMENT_SUMMARY *summary, int invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);
        BACAPI_ERROR GetEventInformation(int invokeId, BACNET_OBJECT_ID *oid, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);

        BACAPI_ERROR SubscribeCOVProperty(BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *net,  BACNET_ADDRESS *dest);
        BACAPI_ERROR SubscribeCOV(BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *net,  BACNET_ADDRESS *dest);
        BACAPI_ERROR SegmentACK(BACNET_SEGMENT_ACK *ack, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);
        BACAPI_ERROR CreateObject(BACNET_OBJECT_ID *oid, uint8_t invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);
        BACAPI_ERROR DeleteObject(BACNET_OBJECT_ID *oid, uint8_t invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);
        BACAPI_ERROR TimeSync(BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);
        BACAPI_ERROR TimeSyncUTC(BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);

        BACAPI_ERROR AtomicReadFileRec(BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t rec_start_pos, uint32_t rec_count, BACNET_ADDRESS *dest);
        BACAPI_ERROR AtomicReadFileOct(BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t file_start_pos, uint32_t octet_count, BACNET_ADDRESS *dest);

        BACAPI_ERROR AtomicWriteFileRec(BACNET_OBJECT_ID *oid, uint8_t invoke_id, BACNET_FILE *file_data, BACNET_ADDRESS *dest);
        BACAPI_ERROR AtomicWriteFileOct(BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t file_start_pos, uint32_t octet_count, BACNET_FILE *file_data, BACNET_ADDRESS *dest);

        BACAPI_ERROR AtomicWriteAck(uint8_t invoke_id, uint8_t access, uint16_t file_start_pos, BACNET_ADDRESS *dest);


        // odgovori sa servera
        BACAPI_ERROR Iam(BACNET_DEVICE *device,BACNET_ADDRESS *adr, BACNET_ADDRESS *src);
        BACAPI_ERROR ReadProperty_ACK(BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *property, int arrayIndex, BACNET_ADDRESS *adr, int invokeId);
        BACAPI_ERROR ReadPropertyMultiple_ALL_ACK(BACNET_OBJECT *object, BACNET_ADDRESS *adr, int invokeId);
        BACAPI_ERROR ReadPropertyMultiple_ALL_ACK_bad_coding(BACNET_OBJECT *object, BACNET_ADDRESS *adr, int invokeId);
        BACAPI_ERROR UnconfirmedEventNotification(BACNET_ADDRESS *adr, BACNET_EVENT_NOTIFICATION *eventNotification);
        BACAPI_ERROR IHave(BACNET_OBJECT_ID device, BACNET_OBJECT_ID object, string name,BACNET_ADDRESS *adr);
        BACAPI_ERROR AtomicReadFile_ACK(BACNET_FILE *bac_file, uint8_t invoke, BACNET_ADDRESS *dest);

        // ACKs
        BACAPI_ERROR Simple_ACK(BACNET_CONFIRMED_SERVICE service, int invokeId, BACNET_ADDRESS *adr);
        BACAPI_ERROR Error_ACK(BACNET_ERROR bacerr, int invokeId,BACNET_CONFIRMED_SERVICE service,BACNET_ADDRESS *adr);
        BACAPI_ERROR Reject_ACK(BACNET_REJECT_CODE code,int invokeId, BACNET_ADDRESS *adr);
        BACAPI_ERROR Abort_ACK(BACNET_ABORT abortcode, BACNET_ADDRESS *adr);
        BACAPI_ERROR AcknowledgeAlarm(BACNET_ACK_ALARM ack_alarm, int invokeId, BACNET_ADDRESS *dest);


        BACAPI_ERROR ReadBDT(BACNET_ADDRESS *dest);
        BACAPI_ERROR WriteBDT(BACNET_ADDRESS *dest, BACNET_BVLC_BDT *table);
        BACAPI_ERROR RegisterFD(BACNET_ADDRESS *dest,uint16_t ttl);
        BACAPI_ERROR ReadFDT(BACNET_ADDRESS *dest);
        BACAPI_ERROR DeleteFDTEntry(BACNET_ADDRESS *dest,uint32_t address, uint16_t port);

        BACAPI_ERROR DeviceCommunicationControl(BACNET_ADDRESS *adr, uint16_t duration, uint32_t enable, string password);
        BACAPI_ERROR ReinitDevice(BACNET_ADDRESS *adr, uint32_t state, string password);

        uint32_t GetBroadcastAddress(void);

        // router
        BACAPI_ERROR WhoIs_RouterToNetwork(BACNET_ADDRESS *dest);
        BACAPI_ERROR WhoIs_RouterToNetwork(BACNET_ADDRESS *src, BACNET_ADDRESS *dest);
        BACAPI_ERROR WhoIs_RouterToNetwork(uint16_t dnet, BACNET_ADDRESS *src, BACNET_ADDRESS *dest);
        BACAPI_ERROR I_Am_RouterToNetwork(vector<uint16_t> dnet_list, BACNET_ADDRESS *dest);
        
        BACAPI_ERROR Init_RoutingTable(BACNET_ADDRESS *dest);
        BACAPI_ERROR InitRoutingTableACK(vector<BACAPI_ROUTER_PORT*>vPortList, BACNET_ADDRESS *dest);

        //I_cAce *GetAce() {return ace_;}

        I_cNPDUEncoder *GetNPDUEnc() { return encoderNPDU_;}
        I_cAPDUDecoder *GetAPDUDec() { return decoderAPDU_;}
 private:
 */
         //I_cAce *ace_;
         uint16_t port_;
         I_cDecoder *decoder_;
         I_cNPDUEncoder *encoderNPDU_;
         I_cAPDUEncoder *encoderAPDU_;
         I_cNPDUDecoder *decoderNPDU_;
         I_cAPDUDecoder *decoderAPDU_;

				 /*
 private:
         DWORD Run(LPVOID);
				 */

};
