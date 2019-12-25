#include "I_cAPDUEncoder.h"
#include "I_cBACdec.h"
#include "bacapi.h"
#include "I_cBACLog.h"
//#include <I_cGlobal.h>

I_cAPDUEncoder::I_cAPDUEncoder()
{
}



I_cAPDUEncoder::~I_cAPDUEncoder()
{
}






int I_cAPDUEncoder::WhoIsAPDU(uint8_t *buffer, uint32_t from, uint32_t to, bool params)
{
 int len = 0;

 buffer[0] = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 buffer[1] = SERVICE_UNCONFIRMED_WHO_IS;   /* service choice */
 len = 2;

 if (params)
    {
	 if ((from >= 0) && (from <= BACNET_MAX_INSTANCE) &&
		 (to >= 0) && (to<= BACNET_MAX_INSTANCE))
		{
		 len += gDec->encode_context_unsigned(&buffer[len], 0, from);
		 len += gDec->encode_context_unsigned(&buffer[len], 1, to);
		}
    }

 return len;
}



int I_cAPDUEncoder::EncodeWhoHas(uint8_t *buffer, uint32_t from, uint32_t to, BACNET_OBJECT_ID *oid)
{
 int len = 0;

 buffer[0] = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 buffer[1] = SERVICE_UNCONFIRMED_WHO_HAS;   /* service choice */
 len = 2;

if ((from >= 0) && (from <= BACNET_MAX_INSTANCE) &&
		 (to >= 0) && (to<= BACNET_MAX_INSTANCE))
		{
		 len += gDec->encode_context_unsigned(&buffer[len], 0, from);
		 len += gDec->encode_context_unsigned(&buffer[len], 1, to);
		}


 len += gDec->encode_context_object_id(&buffer[len],2,oid->type,oid->instance);

 return len;
}




//======================================================================================//
//========================[ ENCODE READ PROP APDU  ]====================================//
//======================================================================================//
int I_cAPDUEncoder::ReadPropAPDU(uint8_t *buffer,BACNET_OBJECT_ID *oid, BACNET_PROPERTY_ID property, int arrayIndex, int invokeId, uint16_t max_apdu)
{
// gData->IncInd();
 int len = 0;

 buffer[0] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[0] = buffer[0] | SET_BIT(1);


 // TODO: gDec->encode_max_segs_max_apdu(0, BACNET_MAX_APDU_IP) - active DLL
 buffer[1] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[2] = invokeId;
 buffer[3] = SERVICE_CONFIRMED_READ_PROPERTY;      /* service choice */

 len = 4;

 len += gDec->encode_context_object_id(&buffer[len], 0, oid->type,oid->instance);
 len += gDec->encode_context_enumerated(&buffer[len], 1,property);


 if (arrayIndex >= 0)
    {
     len += gDec->encode_context_unsigned(&buffer[len],2,arrayIndex);
    }

 return len;
}
//______________________________________________________________________________________//




//======================================================================================//
//========================[ ENCODE READ RANGE ]=========================================//
//======================================================================================//
int I_cAPDUEncoder::EncodeReadRange(uint8_t *apdu, uint8_t invoke_id, BACNET_READ_RANGE *range, uint16_t max_apdu)
{
     int apdu_len = 0;   /* total length of the apdu, return value */

    if (apdu) {
        apdu[0] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
        apdu[1] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
        apdu[2] = invoke_id;
        apdu[3] = SERVICE_CONFIRMED_READ_RANGE; /* service choice */
        apdu_len = 4;

        apdu_len +=gDec->encode_context_object_id(&apdu[apdu_len], 0, range->oid.type,range->oid.instance);
        apdu_len +=gDec->encode_context_enumerated(&apdu[apdu_len], 1,range->property);

        /* optional array index */
        if (range->arrayIndex != BACNET_ARRAY_ALL)
           {
            apdu_len +=gDec->encode_context_unsigned(&apdu[apdu_len], 2,range->arrayIndex);
           }

        /* Build the appropriate (optional) range parameter based on the request type */

        switch (range->requestType)
               {
				case RANGE_BY_POSITION:
					apdu_len += gDec->encode_opening_tag(&apdu[apdu_len], 3);
					apdu_len += gDec->encode_application_unsigned(&apdu[apdu_len],range->rangeParam.index);
					apdu_len += gDec->encode_application_signed(&apdu[apdu_len], range->rangeParam.count);
					apdu_len += gDec->encode_closing_tag(&apdu[apdu_len], 3);
			        break;

			   case RANGE_BY_SEQUENCE:
				   apdu_len += gDec->encode_opening_tag(&apdu[apdu_len], 6);
				   apdu_len += gDec->encode_application_unsigned(&apdu[apdu_len],range->rangeParam.seqNumber);
				   apdu_len += gDec->encode_application_signed(&apdu[apdu_len], range->rangeParam.count);
				   apdu_len += gDec->encode_closing_tag(&apdu[apdu_len], 6);
				   break;

			   case RANGE_BY_TIME:
				   apdu_len += gDec->encode_opening_tag(&apdu[apdu_len], 7);
				   apdu_len += gDec->encode_application_date(&apdu[apdu_len], &range->rangeParam.datetime.date);
				   apdu_len += gDec->encode_application_time(&apdu[apdu_len], &range->rangeParam.datetime.time);
				   apdu_len += gDec->encode_application_signed(&apdu[apdu_len], range->rangeParam.count);
				   apdu_len += gDec->encode_closing_tag(&apdu[apdu_len], 7);
				   break;

            //case RR_READ_ALL:  /* to attempt a read of the whole array or list, omit the range parameter */
            //    break;

            default:
                break;
        }
    }

    return apdu_len;
}

//______________________________________________________________________________________//


//======================================================================================//
//========================[ ENCODE READ PROP APDU  ]====================================//
//======================================================================================//
int I_cAPDUEncoder::ReadPropALLAPDU(uint8_t *buffer,BACNET_OBJECT_ID *oid, int invokeId, uint16_t max_apdu)
{
 int len = 0;

 buffer[0] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[0] = buffer[0] | SET_BIT(1);
 buffer[1] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[2] = invokeId;
 buffer[3] = SERVICE_CONFIRMED_READ_PROP_MULTIPLE;      /* service choice */

 len = 4;

 len += gDec->encode_context_object_id(&buffer[len], 0, oid->type,oid->instance);

 len += gDec->encode_opening_tag(&buffer[len],1);
 len += gDec->encode_context_enumerated(&buffer[len], 0,PROP_ALL);
 len += gDec->encode_closing_tag(&buffer[len],1);




 return len;
}
//______________________________________________________________________________________//





int I_cAPDUEncoder::EncodeGetAllarmSummaryAPDU(int invokeId, uint8_t *buffer, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[0] = buffer[0] | SET_BIT(1);
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_GET_ALARM_SUMMARY;



 return len;
}



int I_cAPDUEncoder::EncodeEnrollmentSummaryAPDU(uint8_t *buffer, BACAPI_ENROLLMENT_SUMMARY *summary, uint8_t invoke_id, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[0] = buffer[0] | SET_BIT(1);
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invoke_id;
 buffer[len++] = SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY;

 // ack - filter - ctx[0]
 len += gDec->encode_context_enumerated(&buffer[len],0,(uint32_t)summary->ack_filter);
 size_t rp_count = summary->vEnrollmentList.size();

 for (int c=0;c<rp_count;c++)
     {
	  BACNET_RECIPIENT_PROCESS *rp = &summary->vEnrollmentList[c];

	  gDec->encode_opening_tag(&buffer[len++],1);

	  gDec->encode_opening_tag(&buffer[len++],0);
	      //len += gDec->encode_context_object_id(&buffer[len],0,rp->recipient.RecipientType.Device.type,rp->recipient.RecipientType.Device.instance);

	  if (rp->recipient.TagRecipientType == 0)//
	      len += gDec->encode_context_object_id(&buffer[len],0,rp->recipient.RecipientType.Device.type,rp->recipient.RecipientType.Device.instance);
	      else {
	            len += gDec->encode_context_bacnet_address(&buffer[len],1,&rp->recipient.RecipientType.Address);
	            //len += gDec->encode_application_unsigned(&buffer[len],rp->recipient.RecipientType.Address.networkNumber);
	            //BACNET_OCTET_STRING *str = BACAPI_CreateOctetString(rp->recipient.RecipientType.Address.adr,rp->recipient.RecipientType.Address.len);
	            //len += gDec->encode_application_octet_string(&buffer[len],str);
	            //delete str;
	           }


	   gDec->encode_closing_tag(&buffer[len++],0);
	   len += gDec->encode_context_unsigned(&buffer[len],1,rp->recipientProcessId);
	   gDec->encode_closing_tag(&buffer[len++],1);
	  }

 if (summary->state < 6)
     len += gDec->encode_context_enumerated(&buffer[len],2,(uint32_t)summary->state);

 if (summary->type < 12)
     len += gDec->encode_context_enumerated(&buffer[len],3,(uint32_t)summary->type);

 if (summary->priority_min && summary->priority_max)
    {
     gDec->encode_opening_tag(&buffer[len++],4);

     len += gDec->encode_context_unsigned(&buffer[len],0,summary->priority_min);
     len += gDec->encode_context_unsigned(&buffer[len],1,summary->priority_max);

     gDec->encode_closing_tag(&buffer[len++],4);
    }

 len += gDec->encode_context_unsigned(&buffer[len],5,summary->notification_class);

 return len;
}



int I_cAPDUEncoder::EncodeGetEventInformationAPDU(uint8_t *buffer, BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[0] = buffer[0] | SET_BIT(1);
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invoke_id;
 buffer[len++] = SERVICE_CONFIRMED_GET_EVENT_INFORMATION;

 if (oid)
    len += gDec->encode_context_object_id(&buffer[len],0,oid->type,oid->instance);

 return len;
}





int I_cAPDUEncoder::EncodeWritePropAPDU(uint8_t *buffer,BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *property, int invokeId, uint16_t max_apdu)
{
 int len = 0;

 buffer[0] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[0] = buffer[0] | SET_BIT(1);
 buffer[1] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[2] = invokeId;
 buffer[3] = SERVICE_CONFIRMED_WRITE_PROPERTY;      /* service choice */

 len = 4;

 len += gDec->encode_context_object_id(&buffer[len], 0, oid->type,oid->instance);
 len += gDec->encode_context_enumerated(&buffer[len], 1,property->property);

 if (property->index != (uint16_t)(BACNET_ARRAY_ALL))
    {
     len += gDec->encode_context_unsigned(&buffer[len],2,property->index);
    }

 len += gDec->encode_opening_tag(&buffer[len],3);



 if (BACAPI_IsComplexProperty(property->property))
    {
     len += EncodePropAPDUComplex(&buffer[len],property, property->index);
     len += gDec->encode_closing_tag(&buffer[len],3);   // closing tag
     return len;

    }

 BACNET_APPLICATION_DATA_VALUE *value = BACAPI_GetPropertyValue(property);

 if (value)
    {
	 switch (value->application_tag)
			{
			 case BACNET_APPLICATION_TAG_NULL:

			 case BACNET_APPLICATION_TAG_OSTRING:
			      len += gDec->encode_application_octet_string(&buffer[len],&value->type.Octet_String);
				  break;

			 case BACNET_APPLICATION_TAG_BSTRING:
			 break;

			 case BACNET_APPLICATION_TAG_BOOL:
				  len += gDec->encode_application_boolean(&buffer[len],value->type.Boolean);
				  break;

			 case BACNET_APPLICATION_TAG_DATE:
				  len += gDec->encode_application_date(&buffer[len],&value->type.Date);
				  break;

			 case BACNET_APPLICATION_TAG_TIME:
				  len += gDec->encode_application_time(&buffer[len],&value->type.Time);
				  break;

			 case BACNET_APPLICATION_TAG_OID:
				  len += gDec->encode_application_object_id(&buffer[len],value->type.Object_Id.type, value->type.Object_Id.instance);
				  break;

			 case BACNET_APPLICATION_TAG_ENUM:
				  len += gDec->encode_application_enumerated(&buffer[len],value->type.Enumerated);
				  break;

			 case BACNET_APPLICATION_TAG_REAL:
				  len += gDec->encode_application_real(&buffer[len],value->type.Real);
				  break;

			 case BACNET_APPLICATION_TAG_DOUBLE:
				  len += gDec->encode_application_double(&buffer[len],value->type.Double);
				  break;

			 case BACNET_APPLICATION_TAG_SINT:
				  len += gDec->encode_application_signed(&buffer[len],value->type.Signed_Int);
				  break;

			 case BACNET_APPLICATION_TAG_UINT:
				  len += gDec->encode_application_unsigned(&buffer[len],value->type.Unsigned_Int);
				  break;
			 case BACNET_APPLICATION_TAG_CSTRING:
			      len += gDec->encode_application_character_string(&buffer[len],&value->type.character_String);
			      break;

			 case BACNET_APPLICATION_TAG_RESERVED_13:
			 case BACNET_APPLICATION_TAG_RESERVED_14:
			 case BACNET_APPLICATION_TAG_RESERVED_15:
			      break;

			}
    }

 len += gDec->encode_closing_tag(&buffer[len],3);


 if (property->priority > 0 && property->priority <=16)
    {
     len += gDec->encode_context_unsigned(&buffer[len],4,property->priority);
    }


 return len;
}








int I_cAPDUEncoder::EncodePropAPDUComplex(uint8_t *buffer,BACNET_OBJECT_PROPERTY *property, int array_index)
{

 

 int len = 0;
 switch (property->property)
        {
        case PROP_SUBORDINATE_LIST:
        case PROP_ACCESS_EVENT_CREDENTIAL:
        case PROP_ACCESS_DOORS:
        case PROP_ZONE_FROM:
        case PROP_ZONE_TO:
        case PROP_CREDENTIALS_IN_ZONE:
        case PROP_LAST_CREDENTIAL_ADDED:
        case PROP_LAST_CREDENTIAL_REMOVED:
        case PROP_MEMBERS:
        case PROP_CREDENTIALS:
        //case PROP_ACCOMPANIED:
        case PROP_BELONGS_TO:
        case PROP_LAST_ACCESS_POINT:
        case PROP_LOG_DEVICE_OBJECT_PROPERTY:
        case PROP_LINK_REFERENCE_IN:
        case PROP_LINK_REFERENCE_OUT:
        case PROP_GROUP_MEMBERS:
			 {
			  len += EncodeDeviceObjectPropertyRef(buffer, property);
			  return len;
             }

        case PROP_TIME_OF_ACTIVE_TIME_RESET:
        case PROP_TIME_OF_STATE_COUNT_RESET:
        case PROP_CHANGE_OF_STATE_TIME:
        case PROP_MAXIMUM_VALUE_TIMESTAMP:
        case PROP_MINIMUM_VALUE_TIMESTAMP:
        case PROP_VALUE_CHANGE_TIME:
        case PROP_START_TIME:
        case PROP_STOP_TIME:
        case PROP_MODIFICATION_DATE:
        case PROP_UPDATE_TIME:
        case PROP_COUNT_CHANGE_TIME:
        case PROP_LAST_CREDENTIAL_ADDED_TIME:
        case PROP_LAST_CREDENTIAL_REMOVED_TIME:
        case PROP_ACTIVATION_TIME:
        case PROP_EXPIRATION_TIME:
        case PROP_LAST_USE_TIME:
			  {
			   len += EncodeDateTime(buffer,property);
			   return len;
			  }


        case PROP_DOOR_MEMBERS:
        case PROP_ZONE_MEMBERS:
        case PROP_MEMBER_OF:
        case PROP_ENTRY_POINTS:
        case PROP_EXIT_POINTS:
        case PROP_ACCOMPANIMENT:
		     {
			  len = EncodeDeviceObjectRef(buffer,property);
			  return len;
			 }


         case PROP_SETPOINT_REFERENCE:
         case PROP_MANIPULATED_VARIABLE_REFERENCE:
         case PROP_CONTROLLED_VARIABLE_REFERENCE:
         case PROP_INPUT_REFERENCE:
         case PROP_OBJECT_PROPERTY_REFERENCE:
         case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
         case PROP_EVENT_ALGORITHM_INHIBIT_REF:
			  {
			   len = EncodeObjectPropertyRef(buffer,property);
			   return len;
			  }




         case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
         case PROP_RESTART_NOTIFICATION_RECIPIENTS:
         case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
         case PROP_COVU_RECIPIENTS:
              {
               len = EncodeBACnetRecipient(buffer,property);
               return len;
              }
         break;



         case PROP_RECIPIENT_LIST:
			  {
			   len = EncodeBACDestination(buffer,property);
			   return len;
			  }
         break;


         case PROP_EFFECTIVE_PERIOD:
              {
               len = EncodeEffectivePeriod(buffer,property);
               return len;
               break;
              }


         case PROP_EVENT_TIME_STAMPS:
         case PROP_LAST_RESTORE_TIME:
         case PROP_TIME_OF_DEVICE_RESTART:
         case PROP_ACCESS_EVENT_TIME:
			  {
			   len = EncodeTimeTamps(buffer,property);
			   return len;
			   break;
			  }
         break;

         case PROP_ACTIVE_COV_SUBSCRIPTIONS:
              {
               // READ ONLY
               break;
              }
         break;



         case PROP_DATE_LIST:
              {
               len = EncodeCalendarDateList(buffer,property);
               return len;
               break;
              }
         break;

         case PROP_PRIORITY_ARRAY:
			  {
			   len = EncodePriority(buffer,property,array_index);
			   return len;
			   break;
			  }
         break;



         case PROP_WEEKLY_SCHEDULE:
              {
               len = EncodeWeeklySchedule(buffer,property);
               return len;
              }
         break;

         case PROP_EXCEPTION_SCHEDULE:
              {
               len = EncodeExceptionSchedule(buffer,property);
               return len;
              }
         break;


         case PROP_LIST_OF_GROUP_MEMBERS:
              {
               //RefreshReadAccessSpecification(parentItem,property);
			   break;
              }

         case PROP_DEVICE_ADDRESS_BINDING:
         case PROP_SLAVE_ADDRESS_BINDING:
         case PROP_MANUAL_SLAVE_ADDRESS_BINDING:
			  {
			   len = EncodeDeviceAddressBinding(buffer,property);
			   return len;
			   break;
			  }

         case PROP_ACTION:
              {
               len = EncodeAction(buffer,property);
               return len;
               break;
              }


         case PROP_ACTION_TEXT:
              {
               //RefreshActionText(parentItem,property);
               break;
              }


		 case PROP_ACCEPTED_MODES:
		 case PROP_LIFE_SAFETY_ALARM_VALUES:
		 case PROP_ALARM_VALUES:
		 case PROP_FAULT_VALUES:
		      {
		       len = EncodeEnumList(buffer,property);
		       return len;
		      }

		 case PROP_EVENT_PARAMETERS:
		      {
		       //gLog->AppendLog("CMainFrame::RefreshComplexProperties: FALIM! RefreshEventParameters();\n");
		       break;
		      }

         case PROP_LOG_BUFFER:
              {
               // Samo S ReadRange Servisom
               break;
              }

         case PROP_SCALE:
              {
               //RefreshScale(parentItem, property);
               break;
              }

         case PROP_PRESCALE:
              {
               //RefreshPreScale(parentItem, property);
               break;
              }
         case PROP_LOGGING_RECORD:
              {
               len = EncodeLoggingRecord(buffer,property);
               return len;
              }

         case PROP_FAULT_PARAMETERS:
              {
               len = EncodeBACnetFaultParameters(buffer,property);
               return len;
              }
         case PROP_AUTHENTICATION_POLICY_LIST:
             {
              len = this->EncodeAuthenticationPolicyList(buffer,property);
              return len;
             }
         case PROP_NEGATIVE_ACCESS_RULES:
         case PROP_POSITIVE_ACCESS_RULES:
         {
          len = this->EncodeBACnetAccessRule(buffer,property);
          return len;
         }

         case PROP_AUTHENTICATION_FACTORS:
         {
          len = EncodeAuthenticationFactors(buffer,property);
          return len;
         }

         case PROP_ASSIGNED_ACCESS_RIGHTS:
         {
          len = EncodeAssignedAccessRights(buffer,property);
          return len;
         }

         case PROP_NETWORK_ACCESS_SECURITY_POLICIES:
         {
          len = EncodeNetworkSecurityPolicy(buffer,property);
          return len;
         }

         case PROP_KEY_SETS:
         {
          len = EncodeSecurityKeySet(buffer,property);
          return len;
         }
         case PROP_SUBSCRIBED_RECIPIENTS:
         {
          len = EncodeEventNotificationSub(buffer,property);
          return len;
         }
         case PROP_PORT_FILTER:
         {
          len = EncodePortPermission(buffer,property);
          return len;
         }
         case PROP_PROCESS_IDENTIFIER_FILTER:
         {
          len = EncodeProcessIdSelection(buffer, property);
          return len;
         }

         case PROP_LIGHTING_COMMAND:
         {
          len = EncodeLightingCommand(buffer, property);
          return len;
         }
         
         case PROP_FD_BBMD_ADDRESS:
         case PROP_BACNET_IP_GLOBAL_ADDRESS:
         {
          len = EncodeBACnetHostNPort(buffer,property);
          return len;
         }
         
         case PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE:
         {
          len = EncodeBACnetBDTEntry(buffer,property);
          return len;
         }
         
         case PROP_BBMD_FOREIGN_DEVICE_TABLE:
         {
          len = EncodeBACnetFDTEntry(buffer,property);
          return len;
         }

         //case PROP_FD_BBMD_ADDRESS:
         {
          //len = gDec->encode_bacnet_host_port(buffer,property->complexValue.hostnPort);
          //return len;
         }
         
         case PROP_VIRTUAL_MAC_ADDRESS_TABLE:
         {
          len = EncodeVMACAddressTable(buffer,property);
          return len;
         }
         
         case PROP_ROUTING_TABLE:
         { 
          len = EncodeRoutingTable(buffer,property);
          return len;
         }
        }

 return 0;
}


// TODO: loggin record je lista vrijednosti
int I_cAPDUEncoder::EncodeLoggingRecord(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 BACNET_ACCUMULATOR_RECORD *rec = &property->complexValue.accuRecord;

 len += gDec->encode_opening_tag(&buffer[len],0);
 len += gDec->encode_application_datetime(&buffer[len],&rec->timeStamp);
 len += gDec->encode_closing_tag(&buffer[len],0);

 len += gDec->encode_context_unsigned(&buffer[len],1,rec->presentValue);
 len += gDec->encode_context_unsigned(&buffer[len],2,rec->accumulatedValue);
 len += gDec->encode_context_enumerated(&buffer[len],3,rec->accumulatedValue);

 return len;
}



// TODO: Lista
int I_cAPDUEncoder::EncodeLogBuffer(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 size_t cnt = property->complexValue.vEventLogRecordList.size();

 if (cnt > 0)
    {
	 BACNET_EVENT_LOG_RECORD *rec = property->complexValue.vEventLogRecordList[0];

	 len += gDec->decode_application_date(&buffer[len],&rec->timeStamp.date);
	 len += gDec->decode_application_time(&buffer[len],&rec->timeStamp.time);
 /*
 len++;  // closing tag [0F]

	 if (gDec->decode_is_opening_tag_number(&data[len],1))  // // [1] logDatum
		{
		 len++;

		 gDec->decode_tag_number_and_value(&data[len], &tag, &len_val);
         rec->type = tag;

         switch (tag)
		        {
				 case 0: break; // log status - ne hendlamo
				 case 1: // TODO: ConfirmedEventNotification
				  break;
				 case 2: len += gDec->decode_context_real(&data[len],9,&rec->logDatum.time_change); break;
				 default:
				 return -1;
		        }
         len++;  // closing tag [1F]
         property->complexValue.vEventLogRecordList.push_back(rec);
		}
 */
    }

  return len;
}


int I_cAPDUEncoder::EncodeDeviceObjectRef(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 int count = (size_t)property->complexValue.vDeviceObjectReference.size();

 for (int c=0;c<count;c++)
     {
      BACNET_DEVICE_OBJECT_REFERENCE *ref = property->complexValue.vDeviceObjectReference[c];
      len += gDec->bacapp_encode_device_obj_ref(&buffer[len],ref);
     }

 return len;
}

// KOMENTIRANO 13.11.2013
int I_cAPDUEncoder::EncodeEnumList(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 //
 //int count = (size_t)property->vValueList.size();
 //
 //for (int c=0;c<count;c++)
 //    {
 //     BACNET_APPLICATION_DATA_VALUE *value = property->vValueList[c];
 //     len += gDec->bacapp_encode_application_data(&buffer[len],linkedValue);
 //    }


 return len;
}




int I_cAPDUEncoder::EncodeObjectPropertyRef(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 size_t count = property->complexValue.vObjectPropertyReference.size();
 for (int c=0;c<count;c++)
     {
      BACNET_OBJECT_PROPERTY_REFERENCE *ref = property->complexValue.vObjectPropertyReference[c];
      len += gDec->bacapp_encode_obj_property_ref(&buffer[len],ref);
     }
 return len;
}




int I_cAPDUEncoder::EncodeDeviceObjectPropertyRef(uint8_t *buffer,BACNET_OBJECT_PROPERTY *property )
{
 int len = 0;

 size_t count = property->complexValue.vDeviceObjectPropertyReference.size();
 for (size_t c=0;c<count;c++)
     {
      BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *ref = property->complexValue.vDeviceObjectPropertyReference[c];
      len += gDec->bacapp_encode_device_obj_property_ref(&buffer[len],ref);
     }

 return len;
}




int I_cAPDUEncoder::EncodeBACDestination(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 int cnt = property->complexValue.vDestinationList.size();

 for (int c=0;c<cnt;c++)
      {
       BACNET_DESTINATION *linkedValue = property->complexValue.vDestinationList[c];
       len += gDec->encode_destination(&buffer[len],linkedValue);
      }
 return len;
}





// nova
int I_cAPDUEncoder::EncodeBACnetRecipient(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 int count = (int)property->complexValue.vRecipientList.size();

 for (int c=0;c<count;c++)
     {
      BACNET_RECIPIENT *ptr_recipient = property->complexValue.vRecipientList[c];
      len += gDec->encode_recipient(&buffer[len],ptr_recipient);
     }

 return len;
}




int I_cAPDUEncoder::EncodeWeeklySchedule(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 BACNET_WEEKLY_SCHEDULE *value = &property->complexValue.weeklySchedule;

 len += gDec->encode_weekly_schedule(&buffer[len],value);

 return len;
}




/*
BACnetSpecialEvent::= SEQUENCE {
period  CHOICE {
                calendarEntry [0] - BACnetCalendarEntry,
                                       - [0] Date
                                       - [1] BACnetDateRange
                                       - [2] BACnetWeeknDay
                calendarReference [1] - BACnetObjectIdentifier
               },
listOfTimeValues [2] - SEQUENCE OF BACnetTimeValue,
eventPriority [3] Unsigned (1..16)
}
*/
int I_cAPDUEncoder::EncodeExceptionSchedule(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0, ret = 0;
 size_t count = property->complexValue.vSpecialEventList.size();

 for (size_t c=0;c<count;c++)
     {
      BACNET_SPECIAL_EVENT *ptr_special_event = &property->complexValue.vSpecialEventList[c];
      switch (ptr_special_event->TagSpecialEventType)
             {
              case 0:
              {
               len += gDec->encode_opening_tag(&buffer[len],0);
               switch (ptr_special_event->period.calendarEntry.TagEntryType)
                      {
                       case 0: // date
                        len += gDec->encode_context_date(&buffer[len],0,&ptr_special_event->period.calendarEntry.Entry.date);
                        break;
                       case 1: // date range
                        len += gDec->encode_context_daterange(&buffer[len],1,&ptr_special_event->period.calendarEntry.Entry.dateRange);
                        break;
                       case 2: // weeknday
                        len += gDec->encode_context_weenkday(&buffer[len],2,&ptr_special_event->period.calendarEntry.Entry.weekNDay);
                        break;
                       default:
                       return -1;  // nesto je poslo po krivu
                      }
               len += gDec->encode_closing_tag(&buffer[len],0);
               break;
              }
              case 1:
              len += gDec->encode_context_object_id(&buffer[len],1,ptr_special_event->period.calendarReference.type,ptr_special_event->period.calendarReference.instance);
              break;
             }

      // time_value parovi
      gDec->encode_opening_tag(&buffer[len],2);
      len++;
      size_t time_val_count = ptr_special_event->vTimeValueList.size();
      for (size_t i=0;i<time_val_count;i++)
          {
           BACNET_TIME_VALUE *ptr_time_value = &ptr_special_event->vTimeValueList[i];
           ret = gDec->encode_time_and_value(&buffer[len],1407,ptr_time_value);
           if (ret < 0)
              {
               return -1;
              }
           len += ret;
          }
       gDec->encode_closing_tag(&buffer[len],2);
       len++;

       // priority
       len += gDec->encode_context_unsigned(&buffer[len],3,(uint8_t)ptr_special_event->eventPriority);
      }

 return len;
}


int I_cAPDUEncoder::EncodeEffectivePeriod(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 BACNET_DATE_RANGE *dateRange = &property->complexValue.dateRange;

 len += gDec->encode_daterange(&buffer[len],dateRange);

 return len;
}



int I_cAPDUEncoder::EncodeCalendarDateList(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 size_t count = property->complexValue.vCalendarEntryList.size();
 for (int c=0;c<count;c++)
     {
      BACNET_CALENDAR_ENTRY *ptr_entry = property->complexValue.vCalendarEntryList[c];
      len += gDec->encode_calendar_entry(&buffer[len],BACNET_MAX_APDU_IP, ptr_entry);
     }

 return len;
}





int I_cAPDUEncoder::EncodeSubscribeCOVPropertyAPDU(uint8_t *buffer, BACAPI_SUBSCRIBE_COV *cov, int invokeId, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_SUBSCRIBE_COV_PROPERTY;



 len += gDec->encode_context_unsigned(&buffer[len], 0,cov->processId);
 len += gDec->encode_context_object_id(&buffer[len], 1, (int) cov->oid.type, cov->oid.instance);


 if (!cov->cancel)
    {
     len += gDec->encode_context_boolean(&buffer[len], 2, cov->confirmedNotification);
     len += gDec->encode_context_unsigned(&buffer[len], 3, cov->lifetime);

	 len += gDec->encode_opening_tag(&buffer[len], 4);
	 len += gDec->encode_context_enumerated(&buffer[len], 0, cov->propId);
	 len += gDec->encode_closing_tag(&buffer[len], 4);

	 if (cov->increment > 0)
		{
		 len += gDec->encode_context_real(&buffer[len], 5, cov->increment);
		}
    }else {
		   len += gDec->encode_opening_tag(&buffer[len], 4);
		   len += gDec->encode_context_enumerated(&buffer[len], 0, cov->propId);
		   len += gDec->encode_closing_tag(&buffer[len], 4);
          }



 return len;
}





int I_cAPDUEncoder::EncodeSubscribeCOVAPDU(uint8_t *buffer, BACAPI_SUBSCRIBE_COV *cov, int invokeId, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_SUBSCRIBE_COV;



 len += gDec->encode_context_unsigned(&buffer[len], 0,cov->processId);
 len += gDec->encode_context_object_id(&buffer[len], 1, (int) cov->oid.type, cov->oid.instance);


 if (!cov->cancel)
    {
     len += gDec->encode_context_boolean(&buffer[len], 2, cov->confirmedNotification);
	 len += gDec->encode_context_unsigned(&buffer[len], 3, cov->lifetime);
	}


 return len;
}



int I_cAPDUEncoder::EncodeSegmentACK(uint8_t *buffer, BACNET_SEGMENT_ACK *ack)
{
 int len = 0;
 buffer[len] = 0;
 buffer[len] = ack->pduType;
 if (ack->negativeACK)
    {
     buffer[len] = buffer[len] | SET_BIT(4);
    }
 if (ack->server)
    {
     buffer[len] = buffer[len] | SET_BIT(5);
    }

 len++;

 buffer[len++] = ack->invokeId;
 buffer[len++] = ack->sequenceNum;
 buffer[len++] = ack->windowSize;

 return len;
}


int I_cAPDUEncoder::EncodeAtomicReadFileOct(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, uint16_t start_oct, uint32_t oct_count, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invoke_id;
 buffer[len++] = SERVICE_CONFIRMED_ATOMIC_READ_FILE;

 len += gDec->encode_application_object_id(&buffer[len],file_oid->type,file_oid->instance);
 len += gDec->encode_opening_tag(&buffer[len],0);
 len += gDec->encode_application_signed(&buffer[len],start_oct);
 len += gDec->encode_application_unsigned(&buffer[len],oct_count);
 len += gDec->encode_closing_tag(&buffer[len],0);

 return len;
}


int I_cAPDUEncoder::EncodeAtomicReadFileRec(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, uint16_t start_rec, uint32_t rec_count, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invoke_id;
 buffer[len++] = SERVICE_CONFIRMED_ATOMIC_READ_FILE;

 len += gDec->encode_application_object_id(&buffer[len],file_oid->type,file_oid->instance);
 len += gDec->encode_opening_tag(&buffer[len],1);
 len += gDec->encode_application_signed(&buffer[len],start_rec);
 len += gDec->encode_application_unsigned(&buffer[len],rec_count);
 len += gDec->encode_closing_tag(&buffer[len],1);

 return len;
}



int I_cAPDUEncoder::EncodeAtomicWriteFileOct(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, uint16_t start_oct, uint32_t octet_count, BACNET_FILE *file, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invoke_id;
 buffer[len++] = SERVICE_CONFIRMED_ATOMIC_WRITE_FILE;

 len += gDec->encode_application_object_id(&buffer[len],file_oid->type,file_oid->instance);
 len += gDec->encode_opening_tag(&buffer[len],0);
 len += gDec->encode_application_signed(&buffer[len],start_oct);
 len += gDec->encode_application_octet_string_range(&buffer[len],start_oct, octet_count,&file->data);
 len += gDec->encode_closing_tag(&buffer[len],0);

 return len;
}


int I_cAPDUEncoder::EncodeAtomicWriteFileRec(uint8_t *buffer, BACNET_OBJECT_ID *file_oid, uint8_t invoke_id, BACNET_FILE *file, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invoke_id;
 buffer[len++] = SERVICE_CONFIRMED_ATOMIC_WRITE_FILE;

 len += gDec->encode_application_object_id(&buffer[len],file_oid->type,file_oid->instance);
 len += gDec->encode_opening_tag(&buffer[len],1);
 len += gDec->encode_application_signed(&buffer[len],file->start_pos);
 len += gDec->encode_application_octet_string(&buffer[len],&file->data);
 len += gDec->encode_closing_tag(&buffer[len],1);

 return len;
}


int I_cAPDUEncoder::EncodeAtomicWriteAck(uint8_t *buffer, uint8_t invoke_id, uint8_t access, uint16_t file_start_pos, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_COMPLEX_ACK;
 //buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invoke_id;
 buffer[len++] = SERVICE_CONFIRMED_ATOMIC_WRITE_FILE;

 len += gDec->encode_context_signed(&buffer[len],access,file_start_pos);

 return len;
}





int I_cAPDUEncoder::EncodeCreateObject(uint8_t *buffer, uint16_t type, uint32_t instance ,int invokeId, uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_CREATE_OBJECT;


 // opening tag za object specifier
 len += gDec->encode_opening_tag(&buffer[len],0);
 if (instance == (~0))  // ne postoji instance, enkodiraj samo type
    {
     len += gDec->encode_context_unsigned(&buffer[len],0,type);
    }else len += gDec->encode_context_object_id(&buffer[len],1,type,instance);

 len += gDec->encode_closing_tag(&buffer[len],0);

 return len;
}
//______________________________________________________________________________________//






int I_cAPDUEncoder::EncodeDeleteObject(uint8_t *buffer, uint16_t type, uint32_t instance ,int invokeId,uint16_t max_apdu)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 buffer[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, max_apdu);
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_DELETE_OBJECT;


 len += gDec->encode_application_object_id(&buffer[len],type,instance);

 return len;
}







int I_cAPDUEncoder::EncodeTimeSync(uint8_t *buffer, BACNET_DATE_TIME *dateTime)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 buffer[len++] = SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION;
 len += gDec->encode_application_date(&buffer[len], &dateTime->date);
 len += gDec->encode_application_time(&buffer[len], &dateTime->time);

 return len;
}




int I_cAPDUEncoder::EncodeTimeSyncUTC(uint8_t *buffer, BACNET_DATE_TIME *dateTime)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 buffer[len++] = SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION;
 len += gDec->encode_application_date(&buffer[len], &dateTime->date);
 len += gDec->encode_application_time(&buffer[len], &dateTime->time);

 return len;
}



int I_cAPDUEncoder::EncodeIAm(uint8_t *buffer, BACNET_DEVICE *device)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 buffer[len++] = SERVICE_UNCONFIRMED_I_AM;
 len += gDec->encode_application_object_id(&buffer[len], device->oid.type,device->oid.instance);
 len += gDec->encode_application_unsigned(&buffer[len], device->maximumAPDU);
 len += gDec->encode_application_enumerated(&buffer[len],(uint32_t) device->segmentation);
 len += gDec->encode_application_unsigned(&buffer[len], device->vendorId);

 return len;
}


int I_cAPDUEncoder::EncodeIHave(uint8_t *buffer, BACNET_OBJECT_ID dev, BACNET_OBJECT_ID obj, string name)
{
 int len = 0;
 buffer[len++] = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 buffer[len++] = SERVICE_UNCONFIRMED_I_HAVE;
 len += gDec->encode_application_object_id(&buffer[len], dev.type,dev.instance);
 len += gDec->encode_application_object_id(&buffer[len], obj.type,obj.instance);


 BACNET_CHARACTER_STRING str;
 str.encoding = CHAR_SET_ANSI_X34;
 str.value8 = name;

 len += gDec->encode_application_character_string(&buffer[len],&str);

 return len;
}



int I_cAPDUEncoder::EncodeReadPropertyACK(uint8_t *buffer, BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *property, int arrayIndex, int invokeId)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_COMPLEX_ACK; // TODO + PDU FLAGS
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_READ_PROPERTY;


 len += gDec->encode_context_object_id(&buffer[len],0,object->oid.type,object->oid.instance);    // context 0 object ID
 len += gDec->encode_context_enumerated(&buffer[len],1,property->property);  // context 1 property

 // ako postoji - enkodiraj array index
 if (arrayIndex != BACNET_ARRAY_ALL)  // postoji array index
    {
     len += gDec->encode_context_unsigned(&buffer[len],2,arrayIndex);
    }


 len += gDec->encode_opening_tag(&buffer[len],3);  // opening start properties

 int ret = EncodeProperty(&buffer[len],property,arrayIndex);
 if (ret < 0)  //
    {
     return ret;
    }
 len += ret;
 len += gDec->encode_closing_tag(&buffer[len],3);  // opening start properties

 return len;
}


int I_cAPDUEncoder::EncodeReadRangeACK(uint8_t *buffer, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *property, BACNET_READ_RANGE *range, int invokeId)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_COMPLEX_ACK; // TODO + PDU FLAGS
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_READ_RANGE;


 len += gDec->encode_context_object_id(&buffer[len],0,oid->type,oid->instance);    // context 0 object ID
 len += gDec->encode_context_enumerated(&buffer[len],1,property->property);  // context 1 property

 uint32_t item_count = property->complexValue.vLogRecordList.size();

 // enkodiraj result flags. prvi_item? zadnji_item? more_items?
 BACNET_BIT_STRING tmp;
 gDec->bitstring_init(&tmp);
 if (range->rangeParam.index == 0)
     gDec->bitstring_set_bit(&tmp,0,true);
     else gDec->bitstring_set_bit(&tmp,0,false);

 if (range->rangeParam.count >= item_count)
     gDec->bitstring_set_bit(&tmp,1,true);
     else gDec->bitstring_set_bit(&tmp,1,false);

 // Can't fit any more in! We just set the result flag to say there
 // was more and drop out of the loop early
 gDec->bitstring_set_bit(&tmp,2,false);

 // enkodiraj result_flags
 len += gDec->encode_context_bitstring(&buffer[len],3,&tmp);


 // record_count
 uint32_t record_count = 0;
 switch (range->requestType)
        {
         case RANGE_BY_POSITION:
         record_count = range->rangeParam.count - range->rangeParam.index;
         break;


		 case RANGE_BY_SEQUENCE:        /* ReadRange by sequence number */
		 record_count = range->rangeParam.count - record_count;
		 break;


         case RANGE_BY_TIME:        // sva vremena poslje specificiranog vremena
         record_count = range->rangeParam.count - record_count;
         break;
        }

 len += gDec->encode_context_unsigned(&buffer[len],4,record_count);

 BACNET_LOG_RECORD **ptr_rec_list = BACAPI_GetLogRecordList(property,range);
 if (ptr_rec_list == NULL) return len; // nema upisa

 // item data
 len += gDec->encode_opening_tag(&buffer[len],5);  // start of records
 for (int c=0;c<record_count;c++)
     {
      len += gDec->encode_opening_tag(&buffer[len],0);  // date time [OPEN]
      len += gDec->encode_application_date(&buffer[len],&ptr_rec_list[c]->timeStamp.date);
      len += gDec->encode_application_time(&buffer[len],&ptr_rec_list[c]->timeStamp.time);
      len += gDec->encode_closing_tag(&buffer[len],0);  // date time [CLOSE]

      len += gDec->encode_opening_tag(&buffer[len],1);  // value [OPEN]
      uint8_t tag_type = ptr_rec_list[c]->recType;
      switch (tag_type)
             {
              case 0: break; // log status - ne hendlamo
              case 1:
              len += gDec->encode_context_boolean(&buffer[len],1,ptr_rec_list[c]->logDatum.Boolean);
              break;
              case 2:
              len += gDec->encode_context_real(&buffer[len],2,ptr_rec_list[c]->logDatum.Real);
              break;
              case 3: len += gDec->encode_context_enumerated(&buffer[len],3,ptr_rec_list[c]->logDatum.Enum); break;
              case 4: len += gDec->encode_context_unsigned(&buffer[len],4,ptr_rec_list[c]->logDatum.Unsigned); break;
              case 5: len += gDec->encode_context_signed(&buffer[len],5,ptr_rec_list[c]->logDatum.Signed); break;
              case 6: len += gDec->encode_context_bitstring(&buffer[len],6,&ptr_rec_list[c]->logDatum.Bit_String); break;
              case 7: buffer[len++] = 0; break;
              case 8: break; // error - ne hendlamo
              case 9: len += gDec->encode_context_real(&buffer[len],9,ptr_rec_list[c]->logDatum.Time_Change); break;
              case 10: break;  // fuck ne
              default:
              return -1;
             }
      len += gDec->encode_closing_tag(&buffer[len],1);  // value [CLOSE]
      //len += gDec->encode_context_bitstring(&buffer[len],2,rec->statusFlags);  // opcionalni
     }
 len += gDec->encode_closing_tag(&buffer[len],5);  // end of records

 return len;
}



int I_cAPDUEncoder::EncodeReadRangeMultiACK(uint8_t *buffer, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *property, BACNET_READ_RANGE *range, int invokeId)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_COMPLEX_ACK; // TODO + PDU FLAGS
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_READ_RANGE;


 len += gDec->encode_context_object_id(&buffer[len],0,oid->type,oid->instance);    // context 0 object ID
 len += gDec->encode_context_enumerated(&buffer[len],1,property->property);  // context 1 property

 uint32_t item_count = property->complexValue.vLogRecordList.size();

 // enkodiraj result flags. prvi_item? zadnji_item? more_items?
 BACNET_BIT_STRING tmp;
 gDec->bitstring_init(&tmp);
 if (range->rangeParam.index == 0)
     gDec->bitstring_set_bit(&tmp,0,true);
     else gDec->bitstring_set_bit(&tmp,0,false);

 if (range->rangeParam.count >= item_count)
     gDec->bitstring_set_bit(&tmp,1,true);
     else gDec->bitstring_set_bit(&tmp,1,false);

 // Can't fit any more in! We just set the result flag to say there
 // was more and drop out of the loop early
 gDec->bitstring_set_bit(&tmp,2,false);

 // enkodiraj result_flags
 len += gDec->encode_context_bitstring(&buffer[len],3,&tmp);


 // record_count
 uint32_t record_count = 0;
 switch (range->requestType)
        {
         case RANGE_BY_POSITION:
         record_count = range->rangeParam.count - range->rangeParam.index;
         break;


		 case RANGE_BY_SEQUENCE:        /* ReadRange by sequence number */
		 record_count = range->rangeParam.count - record_count;
		 break;


         case RANGE_BY_TIME:        // sva vremena poslje specificiranog vremena
         record_count = range->rangeParam.count - record_count;
         break;
        }

 // nesmijemo dozvoliti kodiranje vise podataka nego ih ima u listi
 if (record_count > property->complexValue.vLogMultipleList.size())
     record_count = property->complexValue.vLogMultipleList.size();

 len += gDec->encode_context_unsigned(&buffer[len],4,record_count);

 BACNET_LOG_MULTIPLE_RECORD **ptr_rec_list = BACAPI_GetLogMultipleRecordList(property,range);
 if (ptr_rec_list == NULL) return len; // nema upisa

 // item data
 len += gDec->encode_opening_tag(&buffer[len],5);  // start of records
 for (int c=0;c<record_count;c++)
     {
      len += gDec->encode_opening_tag(&buffer[len],0);  // date time [OPEN]
      len += gDec->encode_application_date(&buffer[len],&ptr_rec_list[c]->timeStamp.date);
      len += gDec->encode_application_time(&buffer[len],&ptr_rec_list[c]->timeStamp.time);
      len += gDec->encode_closing_tag(&buffer[len],0);  // date time [CLOSE]

      len += gDec->encode_opening_tag(&buffer[len],1);  // logData [OPEN]

      int logCount = ptr_rec_list[c]->logData.vLogData.size();
      for (int i=0;i<logCount;i++)
          {
           // switch primitive type
           uint8_t tag_type = ptr_rec_list[c]->logData.recType;
           switch(tag_type)
                 {
				  case 0:
				  len += gDec->encode_context_boolean(&buffer[len],0,ptr_rec_list[c]->logData.vLogData[i].Boolean);
				  break;
				  case 1:
				  len += gDec->encode_context_real(&buffer[len],1,ptr_rec_list[c]->logData.vLogData[i].Real);
				  break;
				  case 2: len += gDec->encode_context_enumerated(&buffer[len],2,ptr_rec_list[c]->logData.vLogData[i].Enum); break;
				  case 3: len += gDec->encode_context_unsigned(&buffer[len],3,ptr_rec_list[c]->logData.vLogData[i].Unsigned); break;
				  case 4: len += gDec->encode_context_signed(&buffer[len],4,ptr_rec_list[c]->logData.vLogData[i].Signed); break;
				  case 5: len += gDec->encode_context_bitstring(&buffer[len],5,&ptr_rec_list[c]->logData.vLogData[i].Bit_String); break;
				  case 6: buffer[len++] = 0; break;
				  case 7: break; // error - ne hendlamo
				  case 8: break;  // fuck ne
				  default:
				  return -1;
                 }
          }
        len += gDec->encode_closing_tag(&buffer[len],1); // lodData [CLOSE]
       }
 len += gDec->encode_closing_tag(&buffer[len],5);  // end of records
 return len;
}




int I_cAPDUEncoder::EncodeAtomicReadFileACK(uint8_t *buffer, uint8_t invoke, BACNET_FILE *bac_file)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_COMPLEX_ACK; // TODO + PDU FLAGS
 buffer[len++] = invoke;
 buffer[len++] = SERVICE_CONFIRMED_ATOMIC_READ_FILE;

 len += gDec->encode_application_boolean(&buffer[len],bac_file->eof);
 len += gDec->encode_opening_tag(&buffer[len],bac_file->access_type);
 len += gDec->encode_application_signed(&buffer[len], bac_file->start_pos);
 len += gDec->encode_application_octet_string(&buffer[len],&bac_file->data);
 len += gDec->encode_closing_tag(&buffer[len],bac_file->access_type);

 return len;

}



int I_cAPDUEncoder::EncodeReadPropertyMultipleALLACK_v2(uint8_t *buffer, BACNET_OBJECT *object, int invokeId)
{
 int len = 0;
 uint8_t pdu_header[9];

 pdu_header[len++] = PDU_TYPE_COMPLEX_ACK; // TODO + PDU FLAGS
 pdu_header[len++] = invokeId;
 pdu_header[len++] = SERVICE_CONFIRMED_READ_PROP_MULTIPLE;

 len += gDec->encode_context_object_id(&buffer[len],0,object->oid.type,object->oid.instance);    // context 0 object ID
 len += gDec->encode_opening_tag(&buffer[len],1);  // opening start properties

 int count = object->vProperties.size();
 for (int c=0;c<count;c++)
     {
	  BACNET_OBJECT_PROPERTY *property = object->vProperties[c];


	  len += gDec->encode_context_enumerated(&buffer[len],2,property->property);

	  len += gDec->encode_opening_tag(&buffer[len],4);  // opening start value
	  int ret = EncodeProperty(&buffer[len],property,BACNET_ARRAY_ALL);
	  if (ret >= 0)
		 {
		  len+=ret;
         }
	  len += gDec->encode_closing_tag(&buffer[len],4);  // opening end value
     }


 return 0;
}


int I_cAPDUEncoder::EncodeReadPropertyMultipleALLACK(uint8_t *buffer, BACNET_OBJECT *object, int invokeId)
{
 int len = 0;

 buffer[len++] = PDU_TYPE_COMPLEX_ACK; // TODO + PDU FLAGS
 buffer[len++] = invokeId;
 buffer[len++] = SERVICE_CONFIRMED_READ_PROP_MULTIPLE;



 len += gDec->encode_context_object_id(&buffer[len],0,object->oid.type,object->oid.instance);    // context 0 object ID
 len += gDec->encode_opening_tag(&buffer[len],1);  // opening start properties

 int count = object->vProperties.size();

 for (int c=0;c<count;c++)
     {
	  BACNET_OBJECT_PROPERTY *property = object->vProperties[c];


	  //gLog->AppendLog("I_cAPDUEncoder::enkodiram property %s",BACAPI_BACnetPropertyName(property->property));



	  len += gDec->encode_context_enumerated(&buffer[len],2,property->property);

	  len += gDec->encode_opening_tag(&buffer[len],4);  // opening start value
	  int ret = EncodeProperty(&buffer[len],property,BACNET_ARRAY_ALL);
	  if (ret >= 0)
		 {
		  len+=ret;
         }
	  len += gDec->encode_closing_tag(&buffer[len],4);  // opening end value
     }

 len += gDec->encode_closing_tag(&buffer[len],1);  // opening start properties

 return len;
}








int I_cAPDUEncoder::EncodeProperty(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property,int arrayIndex)
{
 int len = 0;

 if (BACAPI_IsComplexProperty(property->property))
    {
     len += EncodePropAPDUComplex(&buffer[len],property, arrayIndex);
     return len;
    }

 // nema vrijednosti u
 int count = (size_t)property->vValueList.size();
 if (count == 0)
    {
     return len;
    }

 // WORKAROUND! treba provjeriti da li je ovo arra property ali nek se gone u pm
 // BTL: 9.18.2.1 - A: Reading Non-Array Properties with array index
 if (arrayIndex > count)
    {
     return BACAPI_ERRROR_INVALID_ARRAY_INDEX;
    }

 if (property->property == PROP_PROPERTY_LIST)
    {
     printf("");
    }

 for (int c=0;c<count;c++)
     {
      BACNET_APPLICATION_DATA_VALUE *value = property->vValueList[c];

      //gLog->AppendLog("I_cAPDUEncoder::EncodeProperty: enkodiram property count = %d\n",c);

      if (!value) return -1;

	  switch (value->application_tag)
			 {
			  case BACNET_APPLICATION_TAG_NULL:
			       break;
			  case BACNET_APPLICATION_TAG_OSTRING: 
			       len += gDec->encode_application_octet_string(&buffer[len],&value->type.Octet_String);
			       break;

			  case BACNET_APPLICATION_TAG_BSTRING:
			      len += gDec->encode_application_bitstring(&buffer[len],&value->type.Bit_String);
			      break;

			  case BACNET_APPLICATION_TAG_BOOL:
			       len += gDec->encode_application_boolean(&buffer[len],value->type.Boolean);
			       break;

			  case BACNET_APPLICATION_TAG_DATE:
			       len += gDec->encode_application_date(&buffer[len],&value->type.Date);
			       break;

			  case BACNET_APPLICATION_TAG_TIME:
			       len += gDec->encode_application_time(&buffer[len],&value->type.Time);
			       break;

			  case BACNET_APPLICATION_TAG_OID:
				   {
					 // u slucaju da samo trebamo jednu vrijednost
					if (arrayIndex != BACNET_ARRAY_ALL)
					   {
						// ako je arrayIndex == 0 onda se vraca broj elemenata,
						if (arrayIndex == 0)
						   {
							len += gDec->encode_application_unsigned(&buffer[len],property->vValueList.size());
							return len;
						   }

						// NOTE: -1 zato sto vektor pocinje od nule
						BACNET_APPLICATION_DATA_VALUE *val = property->vValueList[arrayIndex-1];
						if (val == NULL)
						   {

							return BACAPI_ERRROR_INVALID_ARRAY_INDEX; // nije dobro, ne postoji ova vrijednost
						   }
						// postoji ovaj index u arrayu - enkodiraj
						len += gDec->encode_application_object_id(&buffer[len],val->type.Object_Id.type, val->type.Object_Id.instance);
						return len;
					   }

				   len += gDec->encode_application_object_id(&buffer[len],value->type.Object_Id.type, value->type.Object_Id.instance);
				   if (len > BACNET_MAX_APDU_IP)
					  {
					   // trenutno ne podrzavamo segmentaciju
					   return BACAPI_ERROR_SEGMENTATION_NOT_SUPPORTED;
					  }
				   break;
				  }

			  case BACNET_APPLICATION_TAG_ENUM:
			       len += gDec->encode_application_enumerated(&buffer[len],value->type.Enumerated);
			       break;

			  case BACNET_APPLICATION_TAG_DOUBLE:
			       len += gDec->encode_application_double(&buffer[len],value->type.Double);
			       break;

			  case BACNET_APPLICATION_TAG_SINT:
			       len += gDec->encode_application_signed(&buffer[len],value->type.Signed_Int);
			       break;

			  case BACNET_APPLICATION_TAG_REAL:
			       len += gDec->encode_application_real(&buffer[len],value->type.Real);
			       break;
			  case BACNET_APPLICATION_TAG_UINT:
			       len += gDec->encode_application_unsigned(&buffer[len],value->type.Unsigned_Int);
			       break;
			  case BACNET_APPLICATION_TAG_CSTRING:
                   len += gDec->encode_application_character_string(&buffer[len],&value->type.character_String);
                   break;
             }
        }

 return len;
}




int I_cAPDUEncoder::EncodeSimpleACK(uint8_t *apdu, BACNET_CONFIRMED_SERVICE service, int invokeId)
{
 int len = 0;

 apdu[len++] = PDU_TYPE_SIMPLE_ACK;
 apdu[len++] = invokeId;
 apdu[len++] = service;

 return len;
}



int I_cAPDUEncoder::EncodeErrorACK(uint8_t *apdu, BACNET_ERROR bacerr,BACNET_CONFIRMED_SERVICE service, int invokeId)
{
 int len = 0;

 apdu[len++] = PDU_TYPE_ERROR;
 apdu[len++] = invokeId;
 apdu[len++] = service;



 if (service == SERVICE_CONFIRMED_CREATE_OBJECT)
    {
     gDec->encode_opening_tag(&apdu[len++],0);
    }

 len += gDec->encode_application_enumerated(&apdu[len],bacerr.errClass);
 len += gDec->encode_application_enumerated(&apdu[len],bacerr.errCode);

 if (service == SERVICE_CONFIRMED_CREATE_OBJECT)
    {
     //first failed element number
     apdu[len++] = 0;
     // close
     gDec->encode_closing_tag(&apdu[len++],0);
    }


 return len;
}


int I_cAPDUEncoder::EncodeRejectACK(uint8_t *apdu, BACNET_REJECT_CODE code,int invokeId)
{
 int len = 0;

 apdu[len++] = PDU_TYPE_REJECT;
 apdu[len++] = invokeId;
 apdu[len++] = code;

 return len;
}



int I_cAPDUEncoder::EncodeAbortACK(uint8_t *apdu, BACNET_ABORT_REASON code,int invokeId)
{
 int len = 0;

 apdu[len++] = PDU_TYPE_ABORT;
 apdu[len++] = invokeId;
 apdu[len++] = code;

 return len;
}




int I_cAPDUEncoder::EncodeAckAlarm(uint8_t *apdu, BACNET_ACK_ALARM ack_alarm, int invokeId)
{
 int len = 0;

 apdu[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 apdu[len++] = gDec->encode_max_segs_max_apdu(BACNET_MAX_SEGMENTS, BACNET_MAX_APDU_IP);
 apdu[len++] = invokeId;
 apdu[len++] = SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM; /* service choice */

 len += gDec->encode_context_unsigned(&apdu[len],0,ack_alarm.ackProcessId);
 len += gDec->encode_context_object_id(&apdu[len],1,ack_alarm.event_oid.type,ack_alarm.event_oid.instance);
 len += gDec->encode_context_unsigned(&apdu[len],2,(uint32_t)ack_alarm.eventState);
 len += gDec->encode_context_timestamp(&apdu[len],3,&ack_alarm.timeStamp);

 BACNET_CHARACTER_STRING str;
 str.encoding = CHAR_SET_ANSI_X34;
 str.value8 = ack_alarm.ackSource;

 len += gDec->encode_context_character_string(&apdu[len],4,&str);
 len += gDec->encode_context_timestamp(&apdu[len],5,&ack_alarm.timeStamp);


 return len;
}



//TODO: OStali tagovi
int I_cAPDUEncoder::EncodeUnconfirmedEventNotification(uint8_t *buffer, BACNET_EVENT_NOTIFICATION *eventNotification)
{
 int len = 0;


 buffer[len++] = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 buffer[len++] = SERVICE_UNCONFIRMED_EVENT_NOTIFICATION;

 len += gDec->encode_context_unsigned(&buffer[len],0,eventNotification->processId);   // process_id
 len += gDec->encode_context_object_id(&buffer[len],1,eventNotification->deviceId.type,eventNotification->deviceId.instance); // device_oid
 len += gDec->encode_context_object_id(&buffer[len],2,eventNotification->objectId.type,eventNotification->objectId.instance); // object_id

 len += gDec->encode_opening_tag(&buffer[len],3);  // opening_tag [3] time_stamp

 BACNET_TIMESTAMP *stamp = &eventNotification->timeStamp;
 switch (stamp->tag)
		{
		 case TIME_STAMP_TIME:
		 {

		  break;
		 }
		 case TIME_STAMP_SEQUENCE:
		 {
          len += gDec->encode_context_unsigned(&buffer[len],TIME_STAMP_SEQUENCE,stamp->value.sequenceNum);
		  break;
		 }
		 case TIME_STAMP_DATETIME:
		 {
		  break;
		 }
		}

 len += gDec->encode_closing_tag(&buffer[len],3);  // closing_tag [3] time_stamp


 len += gDec->encode_context_unsigned(&buffer[len],4,eventNotification->notificationClass);
 len += gDec->encode_context_unsigned(&buffer[len],5,eventNotification->priority);
 len += gDec->encode_context_enumerated(&buffer[len],6,eventNotification->eventType);


 //len += gDec->encode_con(&buffer[len],7,"");
 len += gDec->encode_context_enumerated(&buffer[len],8,eventNotification->notifyType);
 len += gDec->encode_context_boolean(&buffer[len],9,eventNotification->ackReq);

 len += gDec->encode_context_enumerated(&buffer[len],10,eventNotification->fromState);
 len += gDec->encode_context_enumerated(&buffer[len],11,eventNotification->toState);


 len += gDec->encode_opening_tag(&buffer[len],12);  // opening_tag [12] event_values
 len += gDec->encode_opening_tag(&buffer[len],eventNotification->eventType);  // opening_tag [12] event_values

 switch (eventNotification->eventType)
        {
		 case EVENT_CHANGE_OF_BITSTRING:
		 case EVENT_CHANGE_OF_STATE:
		 case EVENT_CHANGE_OF_VALUE:
		 case EVENT_COMMAND_FAILURE:
		 case EVENT_FLOATING_LIMIT:
		 case EVENT_OUT_OF_RANGE:
		 {
		  len += gDec->encode_context_real(&buffer[len],0,eventNotification->eventParams.notificationParams.outOfRange.exceedingValue);
		  len += gDec->encode_context_bitstring(&buffer[len],1,eventNotification->eventParams.notificationParams.outOfRange.statusFlags);
		  len += gDec->encode_context_real(&buffer[len],2,eventNotification->eventParams.notificationParams.outOfRange.deadband);
		  len += gDec->encode_context_real(&buffer[len],3,eventNotification->eventParams.notificationParams.outOfRange.exceededLimit);

		  break;
		 }
		 case EVENT_COMPLEX_EVENT_TYPE:
		 case EVENT_DEPRECATED:
		 case EVENT_CHANGE_OF_LIFE_SAFETY:
		 case EVENT_EXTENDED:
		 case EVENT_BUFFER_READY:
		 case EVENT_UNSIGNED_RANGE:
		 default: break;
        }

 len += gDec->encode_closing_tag(&buffer[len],eventNotification->eventType);  // closing_tag [3] time_stamp
 len += gDec->encode_closing_tag(&buffer[len],12);  //

 return len;
}




int I_cAPDUEncoder::EncodeDeviceAddressBinding(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 int count = (size_t)property->complexValue.vBACnetAddressBindingList.size();

 for (int c=0;c<count;c++)
     {
      BACNET_ADDRESS_BINDING *adr_bind = property->complexValue.vBACnetAddressBindingList[c];

	  len += gDec->encode_application_object_id(&buffer[len],adr_bind->deviceOid.type,adr_bind->deviceOid.instance);
	  len += gDec->encode_application_unsigned(&buffer[len],adr_bind->deviceAddress.networkNumber);

	  BACNET_OCTET_STRING ostr;
	  BACAPI_InitBACnetOctetString(&ostr,adr_bind->deviceAddress.adr,adr_bind->deviceAddress.len);

	  len += gDec->encode_application_octet_string(&buffer[len],&ostr);
     }


 return len;
}







int I_cAPDUEncoder::EncodeTimeTamps(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 size_t count = property->complexValue.vTimeStamps.size();
 for (size_t c=0;c<count;c++)
     {
      BACNET_TIMESTAMP *ptr_stamp = property->complexValue.vTimeStamps[c];
      switch (ptr_stamp->tag)
             {
              case TIME_STAMP_TIME:
              {
               len += gDec->encode_context_time(&buffer[len],TIME_STAMP_TIME,&ptr_stamp->value.time);
               break;
              }
              case TIME_STAMP_SEQUENCE:
              {
               len += gDec->encode_context_unsigned(&buffer[len],TIME_STAMP_SEQUENCE,ptr_stamp->value.sequenceNum);
               break;
              }
              case TIME_STAMP_DATETIME:
              {
               len += gDec->encode_opening_tag(&buffer[len],TIME_STAMP_DATETIME);
               len += gDec->encode_application_date(&buffer[len],&ptr_stamp->value.dateTime.date);
               len += gDec->encode_application_time(&buffer[len],&ptr_stamp->value.dateTime.time);
               len += gDec->encode_closing_tag(&buffer[len],TIME_STAMP_DATETIME);
               break;
              }

              default:
              break;
             }
       }


 return len;
}



int I_cAPDUEncoder::EncodePriority(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property, int array_index)
{
 int len = 0;
 BACNET_APPLICATION_DATA_VALUE *priority_value = NULL;
 if (array_index != BACNET_ARRAY_ALL)
    {
     // Ako je array_index == 0 onda se vraca velicina array indexa
     if (array_index == 0)
        {
         len += gDec->encode_application_unsigned(&buffer[len],16);
         return len;
        }


     int count = property->vValueList.size();
     if (count >= array_index)
        {
         priority_value = property->vValueList[array_index-1];
         len += gDec->bacapp_encode_application_data(&buffer[len],priority_value);
         return len;
        }
    }

 for (int c=0;c<property->vValueList.size();c++)
     {
      priority_value = property->vValueList[c];
      len += gDec->bacapp_encode_application_data(&buffer[len],priority_value);
     }



 return len;
}


int I_cAPDUEncoder::EncodeAction(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;


 // action list count
 int command_count = (int)property->complexValue.vBACnetActionList.size();

 for (int c=0;c<command_count;c++)
     {
      len += gDec->encode_opening_tag(&buffer[len],0);  // action list opening
      BACNET_ACTION_LIST *ptr_action_list = property->complexValue.vBACnetActionList[c];

      // command count
      int action_count = (int)ptr_action_list->vBACnetActionCommand.size();
      for (int i=0;i<action_count;i++)
          {
           BACNET_ACTION_COMMAND *ptr_command = &ptr_action_list->vBACnetActionCommand[i];

           // OPTIONAL DEVICE OID
           if (ptr_command->device.type != 65535)
              {
               len += gDec->encode_context_object_id(&buffer[len],0,ptr_command->device.type,ptr_command->device.instance);
              }
           // oid
           len += gDec->encode_context_object_id(&buffer[len],1,ptr_command->object.type,ptr_command->object.instance);

           // property
           //len += gDec->encode_context_enumerated(&buffer[len],2,ptr_command->property->property);
           len += gDec->encode_context_unsigned(&buffer[len],2,ptr_command->property->property);

           if (ptr_command->property->index != 65535)
              {
               len += gDec->encode_context_unsigned(&buffer[len],3,ptr_command->property->index);
              }

           len += gDec->encode_opening_tag(&buffer[len],4);
           len += gDec->bacapp_encode_application_data(&buffer[len],ptr_command->property->value);
           len += gDec->encode_closing_tag(&buffer[len],4);

           // priority - CONDITIONAL. samo ako je ovo objekt koji sadrzi priority
           if (BACAPI_IsCommandableObject((BACNET_OBJECT_TYPE)ptr_command->object.type))
              {
               if (ptr_command->property->priority != 65535)
                  {
                   len += gDec->encode_context_unsigned(&buffer[len],5,ptr_command->property->priority);
                  }
              }
           len += gDec->encode_context_unsigned(&buffer[len],6,ptr_command->postDelay);
           len += gDec->encode_context_boolean(&buffer[len],7,ptr_command->quitOnFail);
           len += gDec->encode_context_boolean(&buffer[len],8,ptr_command->writeSuccessful);
          }

      len += gDec->encode_closing_tag(&buffer[len],0);  // action list Closing
     }


 return len;
}




int I_cAPDUEncoder::EncodeDateTime(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 len += gDec->encode_application_datetime(buffer,&property->complexValue.dateTime);
 return len;
}




// TODO:
int I_cAPDUEncoder::EncodeBACnetFaultParameters(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 BACNET_FAULT_PARAMETER *param = &property->complexValue.faultParam;

 switch (param->type)
        {
         case FAULT_TYPE_NONE:  // [0]
         {
          len += gDec->encode_context_null(&buffer[len],0);
          return len;
         }

         case FAULT_TYPE_CHARACTER_STRING:
         {
          //len += gDec->encode_opening_tag(&buffer[len],1);// [1]
          //int cnt = (int)param->value.listOfFaultValues.size();
          //
          //len += gDec->encode_opening_tag(&buffer[len],0); // SEQUENCE of string [0]
          //for (int c=0;c<c++)
          //    {
          //     string str = param->value.listOfFaultValues[c];
          //     gDec->encode_application_character_string
          //    }
         }
         case FAULT_TYPE_EXTENDED:
         case FAULT_TYPE_LIFE_SAFETY:
         case FAULT_TYPE_STATE:
         case FAULT_TYPE_STATUS_FLAGS:
         break;
        }

 return len;
}


/*
typedef struct BACnetPolicy
{
 BACNET_DEVICE_OBJECT_REFERENCE *credential_data_input;  // [0]
 uint16_t index;  // [1]
}BACNET_POLICY;

typedef struct BACnetAuthenticationPolicy
{
 vector<BACNET_POLICY>vPolicyList; //[0]
 bool order_enforced;  // [1]
 uint16_t timeout; // [2]
}BACNET_AUTHENTICATION_POLICY;

*/


int I_cAPDUEncoder::EncodeAuthenticationPolicyList(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 size_t cnt = property->complexValue.vPolicyList.size();

 for (int c=0;c<cnt;c++)
     {
      BACNET_AUTHENTICATION_POLICY *pol = &property->complexValue.vPolicyList[c];

      len += gDec->encode_opening_tag(&buffer[len],0);

      int pol_size = pol->vPolicy.size();
      for (int i=0;i<pol_size;i++)
          {
           BACNET_POLICY *policy = &pol->vPolicy[i];

           len += gDec->encode_opening_tag(&buffer[len],0);
           len += gDec->bacapp_encode_device_obj_ref(&buffer[len],&policy->credential_data_input);
           len += gDec->encode_context_unsigned(&buffer[len],1,policy->index);
           len += gDec->encode_closing_tag(&buffer[len],0);
          }

      len += gDec->encode_closing_tag(&buffer[len],0);

      len += gDec->encode_context_boolean(&buffer[len],1,pol->order_enforced);
      len += gDec->encode_context_unsigned(&buffer[len],2,pol->timeout);

      len += gDec->encode_closing_tag(&buffer[len],0);
     }


 return len;
}



int I_cAPDUEncoder::EncodeBACnetAccessRule(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 size_t cnt = property->complexValue.vRuleList.size();

 for (int c=0;c<cnt;c++)
     {
      BACNET_ACCESS_RULE *rule = &property->complexValue.vRuleList[c];
      len += gDec->encode_context_enumerated(&buffer[len],0,rule->timeRangeSpecifier);

      if (rule->timeRangeSpecifier == 0)
         {
          len += gDec->bacapp_encode_device_obj_property_ref(&buffer[len],&rule->timeRange);
         }

      len += gDec->encode_context_enumerated(&buffer[len],2,rule->locationSpecifier);
      if (rule->locationSpecifier == 0)
         {
          len += gDec->bacapp_encode_device_obj_ref(&buffer[len],&rule->location);
         }
      len += gDec->encode_context_boolean(&buffer[len],4,rule->enable);
     }


 return len;
}


int I_cAPDUEncoder::EncodeAuthenticationFactors(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}


int I_cAPDUEncoder::EncodeAssignedAccessRights(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}


int I_cAPDUEncoder::EncodeAuthenticationFormat(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}

int I_cAPDUEncoder::EncodeNetworkSecurityPolicy(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}

int I_cAPDUEncoder::EncodeSecurityKeySet(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}



int I_cAPDUEncoder::EncodeEventNotificationSub(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}

int I_cAPDUEncoder::EncodePortPermission(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}


int I_cAPDUEncoder::EncodeProcessIdSelection(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}



int I_cAPDUEncoder::EncodeLightingCommand(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 return len;
}


int I_cAPDUEncoder::EncodeBACnetHostNPort(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 
 BACNET_HOST_NPORT *port = property->complexValue.hostnPort;
 
 // host address
 len += gDec->encode_opening_tag(&buffer[len],0);
 len += gDec->encode_bacnet_host_port(&buffer[len],port);
 len += gDec->encode_closing_tag(&buffer[len],0);

 len += gDec->encode_context_unsigned(&buffer[len],1, port->port);
 

 
 return len;
}


int I_cAPDUEncoder::EncodeBACnetBDTEntry(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 for (size_t c=0;c<property->complexValue.vBDTEntry.size();c++)
     {
      BACNET_BDT_ENTRY *ptr_entry = property->complexValue.vBDTEntry[c];
      len += gDec->encode_opening_tag(&buffer[len],0);
      len += gDec->encode_bacnet_host_port(&buffer[len],&ptr_entry->bbmd_address);
      len += gDec->encode_context_unsigned(&buffer[len],1, ptr_entry->bbmd_address.port);      
      len += gDec->encode_closing_tag(&buffer[len],0);
      
      //optional if bac/ip present, if ipv6 absent
      len += gDec->encode_context_octet_string(&buffer[len],1,&ptr_entry->broadcast_mask);
     }

 return len;     
}



int I_cAPDUEncoder::EncodeBACnetFDTEntry(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 for (size_t c=0;c<property->complexValue.vFDTEntry.size();c++)
     {
      BACNET_FDT_ENTRY *ptr_entry = property->complexValue.vFDTEntry[c];
            //optional if bac/ip present, if ipv6 absent
      len += gDec->encode_context_octet_string(&buffer[len],0,&ptr_entry->ip_address);
      len += gDec->encode_context_unsigned(&buffer[len],1, ptr_entry->ttl);
      len += gDec->encode_context_unsigned(&buffer[len],2, ptr_entry->rem_ttl);
     }

 return len;   
}


int I_cAPDUEncoder::EncodeVMACAddressTable(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 for (size_t c=0;c<property->complexValue.vVMACEntry.size();c++)
     {
      BACNET_VMAC_ENTRY *ptr_entry = property->complexValue.vVMACEntry[c];
      len += gDec->encode_context_octet_string(&buffer[len],0,&ptr_entry->virtual_mac_address);
      len += gDec->encode_context_octet_string(&buffer[len],1,&ptr_entry->native_mac_address);
     }

 return len;     
}



int I_cAPDUEncoder::EncodeRoutingTable(uint8_t *buffer, BACNET_OBJECT_PROPERTY *property)
{

 int len = 0;
 for (size_t c=0;c<property->complexValue.vRouterEntry.size();c++)
     {
      BACNET_ROUTER_ENTRY *ptr_entry = property->complexValue.vRouterEntry[c];
      len += gDec->encode_context_unsigned(&buffer[len],0, ptr_entry->network_number);
      len += gDec->encode_context_octet_string(&buffer[len],1,&ptr_entry->mac_address);
      len += gDec->encode_context_enumerated(&buffer[len],2,ptr_entry->status);
      len += gDec->encode_context_unsigned(&buffer[len],3, ptr_entry->performance_index);
     }

 return len;     
}








int I_cAPDUEncoder::EncodeDeviceCC(uint8_t *apdu,uint16_t duration, uint32_t enable, string password)
{
 int len = 0;

 apdu[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 apdu[len++] = 0x04;
 apdu[len++] = 1;
 apdu[len++] = SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL;



 if (duration > 0)
    {
     len += gDec->encode_context_unsigned(&apdu[len], 0, duration);
    }


 len += gDec->encode_context_enumerated(&apdu[len],1,enable);

 BACNET_CHARACTER_STRING str;
 str.encoding = CHAR_SET_ANSI_X34;
 str.value8 = password;

 len += gDec->encode_application_character_string(&apdu[len],&str);


 return len;
}










int I_cAPDUEncoder::EncodeReinitDevice(uint8_t *apdu,uint32_t state, string password)
{
 int len = 0;

 apdu[len++] = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
 apdu[len++] = 0x04;
 apdu[len++] = 1;
 apdu[len++] = SERVICE_CONFIRMED_REINITIALIZE_DEVICE;

 len += gDec->encode_context_enumerated(&apdu[len],0,state);

 BACNET_CHARACTER_STRING str;
 str.encoding = CHAR_SET_ANSI_X34;
 str.value8 = password;


 len += gDec->encode_context_character_string(&apdu[len],1,&str);


 return len;

}












