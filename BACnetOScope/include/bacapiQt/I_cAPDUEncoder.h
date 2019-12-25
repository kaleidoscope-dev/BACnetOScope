#pragma once
#include "bacnet.h"
#include "bacapi.h"



// TODO:
/*
   - EncodeCreateObject mora prihvacati i enkodirati listu init vrijednosti za objekt koji se stvara
*/   


class I_cAPDUEncoder
{
 public:
        I_cAPDUEncoder();
       ~I_cAPDUEncoder();
       
 public:
        int WhoIsAPDU(uint8_t *buffer, uint32_t from, uint32_t to, bool params);
        int ReadPropAPDU(uint8_t *buffer,BACNET_OBJECT_ID *oid, BACNET_PROPERTY_ID property, int arrayIndex, int invokeId, uint16_t max_apdu);
        int ReadPropALLAPDU(uint8_t *buffer,BACNET_OBJECT_ID *oid,  int invokeId,uint16_t max_apdu);
        int EncodeWritePropAPDU(uint8_t *buffer,BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *property, int invokeId,uint16_t max_apdu);
        int EncodeGetAllarmSummaryAPDU(int invokeId, uint8_t *buffer,uint16_t max_apdu);
        int EncodeEnrollmentSummaryAPDU(uint8_t *buffer, BACAPI_ENROLLMENT_SUMMARY *summary, uint8_t invoke_id, uint16_t max_apdu);
        int EncodeGetEventInformationAPDU(uint8_t *buffer, BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t max_apdu);
        int EncodeSubscribeCOVPropertyAPDU(uint8_t *buffer, BACAPI_SUBSCRIBE_COV *cov, int invokeId,uint16_t max_apdu);
        int EncodeSubscribeCOVAPDU(uint8_t *buffer, BACAPI_SUBSCRIBE_COV *cov, int invokeId,uint16_t max_apdu);
        int EncodeSegmentACK(uint8_t *buffer, BACNET_SEGMENT_ACK *ack);
        int EncodeCreateObject(uint8_t *buffer, uint16_t type, uint32_t instance ,int invokeId,uint16_t max_apdu);
        int EncodeDeleteObject(uint8_t *buffer, uint16_t type, uint32_t instance ,int invokeId,uint16_t max_apdu);
        int EncodeTimeSync(uint8_t *buffer, BACNET_DATE_TIME *dateTime);
        int EncodeTimeSyncUTC(uint8_t *buffer, BACNET_DATE_TIME *dateTime);
        int EncodeReadRange(uint8_t *apdu, uint8_t invoke_id, BACNET_READ_RANGE *range,uint16_t max_apdu);
        int EncodeWhoHas(uint8_t *buffer, uint32_t from, uint32_t to, BACNET_OBJECT_ID *oid);

        int EncodeAtomicReadFileOct(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, uint16_t start_oct, uint32_t oct_count,uint16_t max_apdu);
        int EncodeAtomicReadFileRec(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, uint16_t start_rec, uint32_t rec_count,uint16_t max_apdu);
        
        int EncodeAtomicWriteFileOct(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, uint16_t start_oct, uint32_t octet_count, BACNET_FILE *file,uint16_t max_apdu);
        int EncodeAtomicWriteFileRec(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, BACNET_FILE *file,uint16_t max_apdu);

        int EncodeAtomicWriteAck(uint8_t *buffer, uint8_t invoke_id, uint8_t access, uint16_t file_start_pos, uint16_t max_apdu);

         

        int EncodeProperty(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property, int arrayIndex);
        int EncodePropAPDUComplex(uint8_t *buffer,BACNET_OBJECT_PROPERTY *property, int array_index);
        
        // odgovori sa servera
        int EncodeIAm(uint8_t *buffer, BACNET_DEVICE *device);
        int EncodeReadPropertyACK(uint8_t *buffer, BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *property,int arrayIndex,  int invokeId);
        
        int EncodeReadPropertyMultipleALLACK(uint8_t *buffer, BACNET_OBJECT *object, int invokeId);
        int EncodeReadPropertyMultipleALLACK_v2(uint8_t *buffer, BACNET_OBJECT *object, int invokeId);
        
        int EncodeUnconfirmedEventNotification(uint8_t *buffer, BACNET_EVENT_NOTIFICATION *eventNotification);
        int EncodeIHave(uint8_t *buffer, BACNET_OBJECT_ID dev, BACNET_OBJECT_ID obj, string name);
        int EncodeAtomicReadFileACK(uint8_t *buffer, uint8_t invoke, BACNET_FILE *bac_file);
        
        int EncodeReadRangeACK(uint8_t *buffer, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *property, BACNET_READ_RANGE *range, int invokeId);
        int EncodeReadRangeMultiACK(uint8_t *buffer, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *property, BACNET_READ_RANGE *range, int invokeId);

        // ack
        int EncodeSimpleACK(uint8_t *apdu, BACNET_CONFIRMED_SERVICE service, int invokeId);
        int EncodeErrorACK(uint8_t *apdu, BACNET_ERROR bacerr,BACNET_CONFIRMED_SERVICE service, int invokeId);
        int EncodeRejectACK(uint8_t *apdu, BACNET_REJECT_CODE code,int invokeId);
        int EncodeAbortACK(uint8_t *apdu, BACNET_ABORT_REASON code,int invokeId);
        
        int EncodeAckAlarm(uint8_t *apdu, BACNET_ACK_ALARM ack_alarm, int invokeId);

        int EncodeDeviceCC(uint8_t *apdu,uint16_t duration, uint32_t enable, string password);
        int EncodeReinitDevice(uint8_t *apdu,uint32_t state, string password);

 private:
         int EncodeDeviceObjectPropertyRef(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeDeviceObjectRef(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeEnumList(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         //int EncodeObjectPropertyRef(uint8_t *buffer, BACNET_OBJECT_PROPERTY_REFERENCE *ref);
         int EncodeObjectPropertyRef(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeBACDestination(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeBACnetRecipient(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeWeeklySchedule(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeExceptionSchedule(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeEffectivePeriod(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeCalendarDateList(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeDeviceAddressBinding(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeTimeTamps(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodePriority(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property, int array_index);
         int EncodeAction(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeLogBuffer(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeDateTime(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeLoggingRecord(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeBACnetFaultParameters(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeAuthenticationPolicyList(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeBACnetAccessRule(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeAuthenticationFactors(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeAssignedAccessRights(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeAuthenticationFormat(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeNetworkSecurityPolicy(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeSecurityKeySet(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeEventNotificationSub(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodePortPermission(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeProcessIdSelection(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeLightingCommand(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeBACnetHostNPort(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeBACnetBDTEntry(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeBACnetFDTEntry(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeVMACAddressTable(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         int EncodeRoutingTable(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property);
         



};




