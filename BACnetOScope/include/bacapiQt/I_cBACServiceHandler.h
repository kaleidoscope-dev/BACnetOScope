#pragma once
#include "bacnet.h"
#include "bacerr.h"

// TODO:
// - DecodeCreateObjectRequest ce preskociti listu init vrijednosti ako dodje



class I_cBACServiceHandler
{
 public:
        I_cBACServiceHandler();
       ~I_cBACServiceHandler();
       
 public:
        void Handle_Unconfirmed(uint8_t *apdu, BACNET_NPDU_DATA *npdu, BACNET_UNCONFIRMED_SERVICE service, int apdulen, int bvlc_len);  // bvlc_len samo kod WHO_IS zbog paddinga
        void Handle_Confirmed(uint8_t *apdu, BACNET_NPDU_DATA *npdu, BACNET_CONFIRMED_SERVICE service, int apduLen, int invokeId);
        void Handle_ComplexACK(uint8_t *apdu, BACNET_NPDU_DATA *npdu, BACNET_CONFIRMED_SERVICE service, int apduLen, uint8_t invokeId);
       
 private: 
         BACAPI_ERROR DecodeIAmRequest(uint8_t *apdu, BACNET_DEVICE *device, int *ret_len);
         BACAPI_ERROR DecodeReadPropertyComplexAck(uint8_t *data, int dataLen, BACNET_OBJECT_PROPERTY *property,BACNET_OBJECT_ID *oid, int *ret_len);
         BACAPI_ERROR DecodeReadPropertyMultipleComplexAck(uint8_t *apdu, int dataLen, BACNET_OBJECT *object, int *ret_len);
         int DecodeReadRangeComplexAck(uint8_t *apdu, BACNET_OBJECT *object);
         
         int DecodeReadRangeAck(uint8_t *apdu, BACNET_OBJECT *object);
         int DecodeReadRangeMultiAck(uint8_t *apdu, BACNET_OBJECT *object);
         
         
         BACAPI_ERROR DecodePrimitiveValue(uint8_t *apdu, int dataLen, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodePropriateryPrimitiveValue(uint8_t *apdu, int dataLen, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         
         
         
         int DecodeComplexPropertyValue(uint8_t *data, BACNET_OBJECT_PROPERTY *property);


		 // testovi
	     char *get_string_val( BACNET_APPLICATION_DATA_VALUE *value);
	     
	     
 private:  // SERVER 	     
         BACAPI_ERROR DecodeWhoIsRequest(uint8_t *apdu, BACNET_DEVICE_RANGE *range, uint16_t *ret_len);
         int DecodeReadPropertyRequest(uint8_t *apdu, int apduLen, BACNET_OBJECT_PROPERTY_REFERENCE *ref);
         int DecodeReadPropertyMultipleRequest(uint8_t *apdu, int apduLen, BACNET_OBJECT_PROPERTY_REFERENCE *ref);
         int DecodeWritePropertyRequest(uint8_t *apdu, int apduLen, BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *prop);
         int DecodeCreateObjectRequest(uint8_t *apdu,BACNET_OBJECT_ID *oid);
         int DecodeDeleteObjectRequest(uint8_t *apdu,BACNET_OBJECT_ID *oid);
         int DecodeWhoHasRequest(uint8_t *apdu, uint32_t *low, uint32_t *hi, BACNET_OBJECT_ID *oid, string *name);
         int DecodeAtomicReadFileRequest(uint8_t *apdu, BACNET_FILE *bac_file);
         int DecodeReadRangeRequest(uint8_t *apdu,BACNET_READ_RANGE *range);
         
         int DecodeIHaveACK(uint8_t *apdu,BACNET_DEVICE *dev, BACNET_OBJECT *obj);
         
 private:
		 //void InitComplexList(void);
		 //BACNET_PROPERTY_ID BACAPI_ComplexPropertyList[52];        

  public:		 
 //private: 
		 // privatne dekodacije za komplekse propertije
		 int DecodePriorityList(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeCOVSubscriptions(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeDeviceAddressBind(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeObjectList(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeWeeklySchedule(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 BACAPI_ERROR DecodeDateRange(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
		 int DecodeSpecialEvent(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeObjectPropertyReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeBACDectination(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeDeviceObjectReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property);

		 int DecodeBACnetActionList(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 BACAPI_ERROR DecodeBACnetActionList(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         
         int DecodeBACnetAction(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 
		 int DecodeActionText(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 BACAPI_ERROR DecodeDateTime(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
		 int DecodeCalendarEntry(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeReadAccessSpecification(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeSetPointObjectPropertyReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property); 
		 int DecodeEventTimeStamps(uint8_t *data, BACNET_OBJECT_PROPERTY *property); 
		 int DecodeDeviceObjectPropertyReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property); 
		 int DecodeEnumList(uint8_t *data, BACNET_OBJECT_PROPERTY *property); 
		 int DecodeEventParameters(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
		 int DecodeLogBuffer(uint8_t *data, BACNET_OBJECT_PROPERTY *property);   // property ne postoji u novom standardu
         int DecodeScale(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodePreScale(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeShedLevel(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeRecipient(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeActiveVTSessions(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeLoggingRecord(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeFaultParameters(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeAuthenticationPolicyList(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeBACnetAccessRule(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         BACAPI_ERROR DecodeCredentialAuthenticationFactors(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodeAssignedAccessRights(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         int DecodeAuthenticationFactorFormat(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         //int DecodeNetworkSecurityPolicy(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         BACAPI_ERROR DecodeEventNotificationSub(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         int DecodePortPermission(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeProcessIdSelection(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         int DecodeLightingCommand(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         BACAPI_ERROR DecodeSecurityKeySets(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodeNetworkAccessSecurityPolicies(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         //int DecodeSecurityKeySet(uint8_t *data, BACNET_OBJECT_PROPERTY *property);
         
         
         BACAPI_ERROR DecodeBACnetPropertyAccessResult(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodeBACnetChannelValue(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodeReadAccessResult(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);        
         BACAPI_ERROR DecodeScheduleAnyValue(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         
         BACAPI_ERROR DecodePropriateryValue(uint8_t *data, int datalen, BACNET_OBJECT_PROPERTY *property, int *ret_len);
        
        
         // dekodiranje ostalih servisa
         BACAPI_ERROR DecodeAlarmSummary(uint8_t *data, BACNET_ALARM *alarm, int payloadLen, int *ret_len);
         BACAPI_ERROR DecodeConfirmedCOVNotification(uint8_t *data, BACNET_COV_NOTIFICATION *cov, int payloadLen, int *ret_len);
         BACAPI_ERROR DecodeUnconfirmedCOVNotification(uint8_t *data, BACNET_COV_NOTIFICATION *cov, int payloadLen, int *ret_len);
         BACAPI_ERROR DecodeEnrollmentSummary(uint8_t *data, BACNET_ENROLLMENT_SUMMARY *summary, int apdu_len, int *ret_len);
         BACAPI_ERROR DecodeEventInformation(uint8_t *data, BACNET_EVENT_INFORMATION *info, int apdu_len, int *ret_len);

         
         
         BACAPI_ERROR DecodeAtomicReadFileAck(uint8_t *data, BACNET_FILE *file_data, int *ret_len);
         BACAPI_ERROR DecodeAtomicWriteFileAck(uint8_t *data, BACNET_FILE *file_data);
         
         int DecodeAtomicWriteFile(uint8_t *data, BACNET_FILE *file_data);         
         
         BACAPI_ERROR DecodeUnconfirmedEventNotification(uint8_t *data, BACNET_EVENT_NOTIFICATION *notification, int *ret_len);
         BACAPI_ERROR DecodeConfirmedEventNotification(uint8_t *data, BACNET_EVENT_NOTIFICATION *notification, int *ret_len);
        		 
		 

         //int DecodeIsPropertyAccessError(uint8_t *data, BACNET_PROPERTY_ID propid);
         
         
         void Handle_Error_State(BACAPI_ERROR state, uint8_t invoke, BACNET_ADDRESS *remote);
       
		 int tmp_decode_special_event(uint8_t *data, BACNET_SPECIAL_EVENT *ptr_special);
		 int tmp_decode_context_time_value(uint8_t *data, BACNET_TIME_VALUE *time_value);
		 
		 BACAPI_ERROR DecodeSpecialProperty(uint8_t *data, int datalen, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *prop, int *ret_len);
		 
		 // specijalni slucaj kod dekodiranja [5E][5F] tagova - OVO NIJE PROPERTY
		 bool IsPropertyAccessError(uint8_t *apdu, BACNET_ERROR *bac_err, int *ret_len);
		 


         BACAPI_ERROR DecodeBACnetHostNPort(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodeBACnetBDTEntry(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodeBACnetFDTEntry(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
         BACAPI_ERROR DecodeVMACAddressTable(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
		 BACAPI_ERROR DecodeRoutingTable(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len);
		 BACAPI_ERROR DecodeTimeStampArray(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);
		 BACAPI_ERROR DecodeValueSource(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len);



		 
};





