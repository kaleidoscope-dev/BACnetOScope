#pragma once

#include "I_cBACClientIP.h"


class I_cBACClient
{
 public:
        I_cBACClient();
       ~I_cBACClient();


        void Init(I_cBACClientIP *clientIP);

 private:
         I_cBACClientIP *clientIP_;


 public:
        /* Definicije BACnet servisa kako su opisani u BACnet standardu
           Parametri funkcija opisani su u bacdef.h za strukture definicije i enumeracije nativne za BACnet standard, (BACNET_* prefix)
           i u bacapi.h za definicije, strukture i enumeracije nativne za BACapi. (BACAPI_* prefix)
        */
				
				/*
        BACAPI_ERROR WhoIs(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_ADDRESS *adr, bool params);
        BACAPI_ERROR WhoHas(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_OBJECT_ID *oid, BACNET_ADDRESS *adr);

        BACAPI_ERROR ReadProperty(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, BACNET_PROPERTY_ID prop, int arrayIndex, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadPropertyALL(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid,  int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadRange(BACNET_DATALINK datalink, BACNET_READ_RANGE *range, uint8_t invokeId,  BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadRange_ACK(BACNET_DATALINK datalink,BACNET_OBJECT_ID *oid , BACNET_OBJECT_PROPERTY *property, BACNET_READ_RANGE *range, uint8_t invoke, BACNET_ADDRESS *net, BACNET_ADDRESS *dest);



        BACAPI_ERROR WriteProperty(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *prop, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);

        BACAPI_ERROR GetAlarmSummary(BACNET_DATALINK datalink, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR GetEnrollmentSummary(BACNET_DATALINK datalink,BACAPI_ENROLLMENT_SUMMARY *summary, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR GetEventInformation(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);

        BACAPI_ERROR SubscribeCOVProperty(BACNET_DATALINK datalink, BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR SubscribeCOV(BACNET_DATALINK datalink, BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR SegmentACK(BACNET_DATALINK datalink, BACNET_SEGMENT_ACK *ack, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR CreateObject(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR DeleteObject(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR TimeSync(BACNET_DATALINK datalink, BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR TimeSyncUTC(BACNET_DATALINK datalink, BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);

        BACAPI_ERROR AtomicReadFileRec(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id,uint16_t rec_start_pos, uint32_t rec_count, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR AtomicReadFileOct(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id,uint16_t file_start_pos, uint32_t octet_count, BACNET_ADDRESS *dest);
        BACAPI_ERROR AtomicWriteFileRec(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id,BACNET_FILE *file_data, BACNET_ADDRESS *dest);
        BACAPI_ERROR AtomicWriteFileOct(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id,uint16_t file_start_pos, uint32_t octet_count, BACNET_FILE *file_data, BACNET_ADDRESS *dest);
        BACAPI_ERROR AtomicWriteAck(BACNET_DATALINK datalink, uint8_t invoke_id, uint8_t access, uint16_t file_start_pos, BACNET_ADDRESS *dest);

        BACAPI_ERROR DeviceCommunicationControl(BACNET_DATALINK datalink, uint16_t duration, uint32_t enable, string password, BACNET_ADDRESS *adr);
        BACAPI_ERROR ReinitDevice(BACNET_DATALINK datalink, uint32_t start, string password, BACNET_ADDRESS *adr);

        // Ova grupa funkcija radi samo s IP klijentom i tice se BBMD-a i foreign devicea
        BACAPI_ERROR ReadBDT(BACNET_ADDRESS *dest);
        BACAPI_ERROR WriteBDT(BACNET_ADDRESS *dest, BACNET_BVLC_BDT *table);
        BACAPI_ERROR RegisterFD(BACNET_ADDRESS *dest,uint16_t ttl);
        BACAPI_ERROR ReadFDT(BACNET_ADDRESS *dest);
        BACAPI_ERROR DeleteFDTEntry(BACNET_ADDRESS *dest,uint32_t address, uint16_t port);



        // server
        BACAPI_ERROR IAm(BACNET_DEVICE *device, BACNET_ADDRESS *dest, BACNET_ADDRESS *src);
        BACAPI_ERROR ReadProperty_ACK(BACNET_DATALINK datalink, BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *property, int arrayIndex, int invokeId, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadPropertyMultiple_ALL_ACK(BACNET_DATALINK datalink, BACNET_OBJECT *object, int invokeId, BACNET_ADDRESS *dest);
        BACAPI_ERROR ReadPropertyMultiple_ALL_ACK_bad_coding(BACNET_OBJECT *object, int invokeId); // test LEGACY!
        BACAPI_ERROR UnconfirmedEventNotification(BACNET_EVENT_NOTIFICATION *eventNotification);
        BACAPI_ERROR IHave(BACNET_OBJECT_ID device, BACNET_OBJECT_ID object, string name);
        BACAPI_ERROR AtomicReadFile_ACK(BACNET_DATALINK datalink, BACNET_FILE *bac_file, uint8_t invoke, BACNET_ADDRESS *dest);


        // ACK
        BACAPI_ERROR Simple_ACK(BACNET_DATALINK datalink, BACNET_CONFIRMED_SERVICE service, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR AcknowledgeAlarm(BACNET_DATALINK datalink, BACNET_ACK_ALARM ack_alarm, int invokeId, BACNET_ADDRESS *dest);


        BACAPI_ERROR Error_ACK(BACNET_DATALINK datalink, BACNET_ERROR bacerr,int invokeId,BACNET_CONFIRMED_SERVICE service, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest);
        BACAPI_ERROR Reject_ACK(BACNET_DATALINK datalink, BACNET_REJECT_CODE code,int invokeId, BACNET_ADDRESS *dest);
        BACAPI_ERROR Abort_ACK(BACNET_DATALINK datalink, BACNET_ABORT abortcode, BACNET_ADDRESS *dest);

        // WIP: router
        BACAPI_ERROR WhoIs_RouterToNetwork(BACNET_ADDRESS *dest);
        BACAPI_ERROR I_Am_RouterToNetwork(BACNET_DATALINK dlink, vector<uint16_t>dnetList, BACNET_ADDRESS *dest);
        BACAPI_ERROR Init_RoutingTable(BACNET_ADDRESS *dest);



 public:
        I_cBACClient802 *GetClient802(void);
        I_cBACClientIP *GetClientIP(void);
        I_cBACClientMSTP *GetClientMSTP(void);
        
        // prije svakog novo citanja moramo isprazniti segment buffer iz decodedAPDU
        void ClearSegmentBuffer(void);
        
 private: 
        // Legacy: Ne koristiti
        void Execute(void);


        


				*/

};
