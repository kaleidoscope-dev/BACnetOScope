#include "bacerr.h"
#include "bacevent.h"
#include "bacapi.h"
//#include "I_cSettings.h"




void BACAPI_Error_Test_Severity(BACAPI_SEVERITY severity)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;
 event->message = new BACAPI_MESSAGE;

 char tmp[64];
 sprintf(tmp,"Severity Test: %s",BACAPI_BACnetSeverity(severity));
 event->message->err_text = tmp;

 event->message->severity = severity;
 event->message->description = tmp;

 event->message->type = BACAPI_ERROR_INTERFACE_NO_INTERFACE;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif

 BACAPI_PushEvent(event);
}



void BACAPI_Message(string txt, string desc, uint8_t severity)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->err_text = txt;
 event->message->severity = (BACAPI_SEVERITY)severity;
 event->message->description = desc;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif

 BACAPI_PushEvent(event);
}


void BACAPI_Messagef(uint8_t severity, const char *desc, const char *msg, ...)
{
 char out[256];
 va_list arglist;

 va_start( arglist, msg );
 vsprintf(out, msg, arglist );
 va_end( arglist );
 
 BACAPI_Message(out,desc,severity);
}


void BACAPI_Message_UnableToCreateClient(void)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->err_text = "Unable to create client.";
 event->message->severity = BACAPI_SEVERITY_CRITICAL;
 event->message->description = "BACnet Lookout requires at least one communication interface to be opened to function properly."
                               "To open communication interface Click Edit->Settings and then select communication option."
                               "Enable BACnet/IP, BACnet/Ethernet or BACnet/MSTP and restart the application."
                               "It is also possible that there are other BACnet application using the same Ethernet port.";
 event->message->type = BACAPI_ERROR_INTERFACE_NO_INTERFACE;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif



 BACAPI_PushEvent(event);
}






// javlja se u ReceivedReadPropMultiACK kada se dobiveni invoke_id nemoze naci u listi
// rezerviranih invokova
void BACAPI_Message_InvalidInvokeId(BACNET_OBJECT *obj, uint8_t invoke_id)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_INVALID_INVOKE_ID;
 event->message->obj = obj;
 event->message->description = "Invalid Invoke";
 event->message->severity = BACAPI_SEVERITY_ERROR;
 event->message->err_text = "Invalid Invoke";
 event->message->invoke_id = invoke_id;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_WhoIsRequest(BACNET_NPDU_DATA *npdu)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->dlink = npdu->dataLink;
 event->message->adr = BACAPI_CreateBACnetAddress(npdu->remote);
 event->message->pdu_type = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
 event->message->service = (uint8_t)SERVICE_UNCONFIRMED_WHO_IS;
 event->message->severity = BACAPI_SEVERITY_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_WHO_IS;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif

 event->message->description = "Who-Is Service was correctly received from the device on the network.";

 
 char txt[256];
 sprintf(txt,"Received Who-Is from Device Adr - %s, DataLink: %s",BACAPI_GetAddressString(npdu->remote).c_str(),BACAPI_BACnetDataLinkName(npdu->dataLink));
 event->message->err_text = txt; 


 BACAPI_PushEvent(event);
}


void BACAPI_Message_ReceivedI_AM(BACNET_DEVICE *dev)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_I_AM;
 event->message->dev = BACAPI_CreateBACnetDevice(dev);
 event->message->severity = BACAPI_SEVERITY_MESSAGE;


 char txt[256];
 sprintf(txt,"Received I AM from Device - %d, %s, DataLink: %s",dev->oid.instance,BACAPI_BACnetVendorName(dev->vendorId),BACAPI_BACnetDataLinkName(dev->npduData->dataLink));
 event->message->err_text = txt;

 event->message->description = "I-Am Service was correctly received from the device on the network.";
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif

 BACAPI_PushEvent(event);
}




void BACAPI_Message_ReceivedI_Have(BACNET_DEVICE *dev,BACNET_OBJECT *obj)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_I_HAVE;
 event->message->dev = BACAPI_CreateBACnetDevice(dev);
 event->message->obj = BACAPI_CreateBACnetObject(obj->oid.type,obj->oid.instance);
 event->message->severity = BACAPI_SEVERITY_MESSAGE;


 char txt[64];
 sprintf(txt,"Received I-Have from Device - %d, %s - %d",dev->oid.instance,BACAPI_BACnetObjectName(obj->oid.type),obj->oid.instance);
 event->message->err_text = txt;

 event->message->description = "I-Have Service was correctly received from the device on the network.";
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_ReceivedAlarmSummary(BACNET_ALARM *alarm)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_ALARM_SUMMARY;
 event->message->severity = BACAPI_SEVERITY_MESSAGE;


 char txt[64];

 if (alarm->vAlarmSummary.size() == 0)
    {
     sprintf(txt,"There are no Objects on the device with active alarm state.");
     event->message->description = "Service was correctly received. There are no active alarms on the device.";
    }else {
		   sprintf(txt,"Received Alarm List");
		   event->message->description = "Service was correctly received.";
          }

 event->message->err_text = txt;
 event->message->description = "Service was correctly received.";
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_ReceivedEnrollmentSummary(BACNET_ENROLLMENT_SUMMARY *summary)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_ENROLLMENT_SUMMARY;
 event->message->severity = BACAPI_SEVERITY_MESSAGE;


 char txt[64];

 if (summary->vEnrollmentSummary.size() == 0)
    {
     sprintf(txt,"There are no Event Initiating Objects currently on the Device.");
     event->message->description = "Service was correctly received. There are no Event Initiating Objects on the Device.";
    }else {
		   sprintf(txt,"Received Enrollment List");
          }

 event->message->err_text = txt;
 event->message->description = "Service was correctly received.";
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_ReceivedEventInformation(BACNET_EVENT_INFORMATION *info)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_EVENT_INFORMATION;
 event->message->severity = BACAPI_SEVERITY_MESSAGE;


 char txt[64];

 if (info->vEventInformationList.size() == 0)
    {
     sprintf(txt,"There are no Event Initiating Objects currently on the Device.");
     event->message->description = "Service was correctly received. There are no Event Initiating Objects on the Device.";
    }else {
		   sprintf(txt,"Received Event Information List");
          }

 event->message->err_text = txt;
 event->message->description = "Service was correctly received.";
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);

}



void BACAPI_Message_ReceivedEventNotification(BACNET_EVENT_NOTIFICATION *notification)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_EVENT_NOTIFICATION;
 event->message->severity = BACAPI_SEVERITY_MESSAGE;



 event->message->err_text = "Received Event Notification.";
 event->message->description = "Service was correctly received.";
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_ReceivedConfirmedCOVNotification(BACNET_COV_NOTIFICATION *cov)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_CONFIRMED_COV_NOTIFICATION;
 event->message->severity = BACAPI_SEVERITY_MESSAGE;


 event->message->err_text = "Received Confirmed COV Notification";
 event->message->description = "Received Confirmed COV Notification.";
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_ReceivedUnconfirmedCOVNotification(BACNET_COV_NOTIFICATION *cov)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_RECEIVED_UNCONFIRMED_COV_NOTIFICATION;
 event->message->severity = BACAPI_SEVERITY_MESSAGE;


 event->message->err_text = "Received Unconfirmed COV Notification";
 event->message->description = "Received Unconfirmed COV Notification.";
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_UnknownDeviceConfirmedCOV(BACNET_COV_NOTIFICATION *cov)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_WARNING_UNKNOWN_DEVICE_CONF_COV;
 event->message->severity = BACAPI_SEVERITY_WARNING;


 event->message->err_text = "Unknown Device on Confirmed COV notification";
 event->message->description = "Unknown Device on Confirmed COV notification.";
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_InvalidTag(BACNET_PDU_TYPE pdu_type, uint8_t service, uint8_t invoke_id, string description)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_INVALID_TAG;
 event->message->err_text = "Invalid Tag";
 event->message->description = description;
 event->message->severity = BACAPI_SEVERITY_ERROR;
 event->message->invoke_id = invoke_id;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


// Zakomentirano 24.5.2018
// Provjeriti Reqiured Value
void BACAPI_Message_PropertyValueMissing(BACNET_DEVICE *dev, BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop)
{
// BACAPI_Event *event = BACAPI_CreateEvent();
// event->type = BACAPI_EVENT_MESSAGE;
//
// event->message = new BACAPI_MESSAGE;
// event->message->type = BACAPI_ERROR_MISSING_PROPERTY_VALUE;
// event->message->dev = dev;
// event->message->obj = obj;
// event->message->prop = prop;
// event->message->severity = BACAPI_SEVERITY_WARNING;
// 
//
// char txt[256];
// sprintf(txt,"Required Value Missing: Device: %d, %s: %d. %s",
//              dev->oid.instance, 
//              BACAPI_BACnetObjectName(obj->oid.type),obj->oid.instance,
//              BACAPI_BACnetPropertyName(prop->property));
//
// event->message->err_text = txt;
//
//
// event->message->description = txt;
//#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
//#endif
//
//
// BACAPI_PushEvent(event);
}






void BACAPI_Message_InvalidDateTimeValue(BACNET_DEVICE *dev, BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_INVALID_DATETIME_VALUE;
 event->message->dev = dev;
 event->message->obj = obj;
 event->message->prop = prop;
 event->message->err_text = "Invalid Date or Time Value";
 char txt[256];
 sprintf(txt,"Invalid Date or Time Value in Property %s in %s(%d) on Device: %d.",
              BACAPI_BACnetPropertyName(prop->property),
              BACAPI_BACnetObjectName(obj->oid.type),obj->oid.instance,
              dev->oid.instance);



 event->message->description = txt;
 event->message->severity = BACAPI_SEVERITY_ERROR;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}





void BACAPI_Message_DateRangeAny(BACNET_DEVICE *dev, BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_DATERANGE_ANY;
 event->message->dev = dev;
 event->message->obj = obj;
 event->message->prop = prop;
 event->message->err_text = "Property Uses ANY as a Type";
 char txt[256];
 sprintf(txt,"Property %s DateRange value in %s(%d) on Device: %d is specified as ANY in the device."
             "BACnet Lookout will display this value as a current Date.",
              BACAPI_BACnetPropertyName(prop->property),
              BACAPI_BACnetObjectName(obj->oid.type),obj->oid.instance,
              dev->oid.instance);



 event->message->description = txt;
 event->message->severity = BACAPI_SEVERITY_WARNING;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_PropertyAccessError(BACNET_DEVICE *dev, BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_WARNING_PROPERTY_ACCESS_ERROR;
 event->message->dev = dev;
 event->message->obj = obj;
 event->message->prop = prop;
 char err_txt[128];
 sprintf(err_txt,"Property Access Error. Device: [%d], [%s](%d) Property: %d - [%s].",
              dev->oid.instance,BACAPI_BACnetObjectName(obj->oid.type),obj->oid.instance,
              prop->property, BACAPI_BACnetPropertyName(prop->property));
 event->message->err_text = err_txt;

 char desc_txt[256];
 sprintf(desc_txt,"Values of Property: [%s] in the Object [%s](%d) on Device: [%d] could not be accessed. "
             "Because the device specified <%s><%s> when trying to read property value",
              BACAPI_BACnetPropertyName(prop->property),
              BACAPI_BACnetObjectName(obj->oid.type),obj->oid.instance,
              dev->oid.instance,BACAPI_BACnetErrorClassName(prop->bac_err.errClass),BACAPI_BACnetErrorCodeName(prop->bac_err.errCode));



 event->message->description = desc_txt;
 event->message->severity = BACAPI_SEVERITY_WARNING;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}





// BACAPI ne podrzava dekodiranje
void BACAPI_Message_UnsupportedService(BACNET_PDU_TYPE pdu_type, uint8_t service, BACNET_ADDRESS *source)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->description = "This service call is not supported by BACnet Lookout.";
 event->message->type = BACAPI_ERROR_UNSUPPORTED_SERVICE;
 event->message->pdu_type = pdu_type;
 event->message->service = (uint8_t)service;
 event->message->severity = BACAPI_SEVERITY_WARNING;
 event->message->err_text = "Unsupported Service";
 event->message->adr = source;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_Error(BACAPI_MESSAGE *msg)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 switch(msg->type)
       {
        case BACAPI_ERROR_CONTEXT_OBJECT_ID:
         msg->err_text = "Error while decoding Object Id. Packet is malformed.";
         msg->description = "Service Handler encountered an error while trying to decode incoming APDU packet. "
                            "This error hapened while trying to decode Object Id part of the APDU and it usually "
                            "indicates that received packet is not properly coded at the source device. "
                            "This error appeared while trying to decode byte %d of the APDU packet.";
         break;

        case BACAPI_ERROR_CONTEXT_ENUMERATED:
         break;


        case BACAPI_ERROR_MALFORMED_PACKET:
         msg->err_text = "Malformed Packet.";
         msg->description = "Service Handler encountered an error while trying to decode incoming APDU packet.";
         break;

        default:
         break;
       }


 event->message = msg;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif

 BACAPI_PushEvent(event);
}



void BACAPI_Message_BACnetReject_UnrecognizedService(uint8_t invokeid)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_REJECT;
 event->message->err_text = "Service Call was rejected by the device with Unrecognized-Service message.";

 event->message->description = "Service Call was rejected by the device with Unrecognized-Service message.";
 event->message->severity = BACAPI_SEVERITY_WARNING;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_BACnetError(BACNET_ERROR err, BACNET_ADDRESS *source, BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_BACERROR;

 char txt[256];
 if (obj && !prop)
    {
     sprintf(txt,"Service call failed with the: '%s' message. %s",BACAPI_BACnetErrorCodeName(err.errCode),BACAPI_GetObjectString(obj->oid).c_str());
    }else if (obj && prop)
             {
              // ako je u settingsu postavljeno da ne saljemo ako nema propertija,
              // onda se izbrisi event i vrati se
              // NOTE: ovo bi trebalo biti u MainFrame u Lookoutu a ne u APIu
              //if (gSettings->GetHideErrors())
              //   {
              //    BACAPI_ReleaseEvent(event);
              //    return;
              //   }else {
              //          sprintf(txt,"Service call failed with the: '%s' message. %s, %s",BACAPI_BACnetErrorCodeName(err.errCode),BACAPI_GetObjectString(obj->oid).c_str(),BACAPI_BACnetPropertyName(prop->property));
              //         }
             }
     else if (!obj && prop)
             {
              sprintf(txt,"Service call failed with the: '%s' message. %s",BACAPI_BACnetErrorCodeName(err.errCode),BACAPI_BACnetPropertyName(prop->property));
             }else  sprintf(txt,"Service call failed with the: '%s' message. ",BACAPI_BACnetErrorCodeName(err.errCode));
             

 event->message->err_text = txt;
 event->message->bac_err = err;
 event->message->adr = source;

 event->message->description = "Service call returned with the BACnet Error message from the Target Device.";
 event->message->severity = BACAPI_SEVERITY_ERROR;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}




void BACAPI_Message_BACnetAbort(BACNET_ABORT abt, BACNET_ADDRESS *source)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 char txt[256];
 sprintf(txt,"BACnet Abort: %s",BACAPI_BACnetAbortReasonName(abt.reason));

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_ABORT;
 event->message->err_text = txt;
 event->message->bac_abort = abt;
 event->message->adr = source;
 
 

 event->message->description = "BACnet Abort";
 event->message->severity = BACAPI_SEVERITY_ERROR;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_BACnetReject(BACNET_REJECT_CODE code, uint8_t invoke, BACNET_ADDRESS *source)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_REJECT;

 char txt[256];
 sprintf(txt,"Service was rejected with: '%s' code",BACAPI_BACnetRejectCodeName(code));


 event->message->err_text = txt;
 event->message->rej_code = code;
 event->message->invoke_id = invoke;



 event->message->adr = source;
 event->message->description = "Service call was rejected by the Target Device. This could be device not supporting the Service, invalid service parameter or other undefined behaviour.";
 event->message->severity = BACAPI_SEVERITY_ERROR;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_SimpleACK(uint8_t service, BACNET_ADDRESS *source)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_SIMPLE_ACK;

 char txt[256];
 sprintf(txt,"%s successfuly executed on %s",BACAPI_BACnetConfirmedServiceName((BACNET_CONFIRMED_SERVICE)service),BACAPI_GetAddressStringIP(source).c_str());
 event->message->err_text = txt;
 event->message->service = service;
 event->message->adr = source;

 event->message->description = "Received Simple ACK. This ussually means that BACnet operation or service call succefuly executed.";
 event->message->severity = BACAPI_SEVERITY_MESSAGE;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_BVLC_Read_BDT_NAK(BACNET_ADDRESS *source)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_BVLC_READ_BDT_NAK;

 event->message->err_text = "Received Negative-ACK";
 //event->message->service = service;
 //event->message->adr = source;

 event->message->description = "Received Negative-ACK.";
 event->message->severity = BACAPI_SEVERITY_ERROR;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_BVLC_Read_BDT_ACK(BACNET_DEVICE *dev)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_BVLC_READ_BDT_ACK;

 char txt[256];
 sprintf(txt,"Received Broadcast Distribution Table from Device: %d",dev->oid.instance);
 event->message->err_text = txt;

 event->message->description = "Received Broadcast Distribution Table.";
 event->message->severity = BACAPI_SEVERITY_MESSAGE;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_BVLC_Read_FDT_NAK(BACNET_ADDRESS *source)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_BVLC_READ_FDT_NAK;

 event->message->err_text = "Read Foreign Device Table Failed.";
 //event->message->service = service;
 //event->message->adr = source;

 event->message->description = "Received Negative-ACK.";
 event->message->severity = BACAPI_SEVERITY_ERROR;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}


void BACAPI_Message_BVLC_Register_FD_NAK(BACNET_ADDRESS *source)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_BVLC_READ_FDT_NAK;

 event->message->err_text = "Register Foreign Device Failed.";
 //event->message->service = service;
 //event->message->adr = source;

 event->message->description = "Received Negative-ACK.";
 event->message->severity = BACAPI_SEVERITY_ERROR;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event); 
}



void BACAPI_Message_BVLC_Read_FDT_ACK(BACNET_DEVICE *dev)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_BVLC_READ_FDT_ACK;

 char txt[256];
 sprintf(txt,"Received Foreign Device Table from Device: %d",dev->oid.instance);
 event->message->err_text = txt;

 event->message->description = "Received Foreign Device Table.";
 event->message->severity = BACAPI_SEVERITY_MESSAGE;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif

 BACAPI_PushEvent(event);
}



void BACAPI_Message_Named_Pipe_Fail(void)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_ERROR_NAMED_PIPE_FAIL;

 char txt[256];
 sprintf(txt,"BACnet lookout has failed to connect a named pipe.");
 event->message->err_text = txt;

 event->message->description = "BACnet lookout has failed to connect a named pipe. Process timed out after 15 seconds.";
 event->message->severity = BACAPI_SEVERITY_CRITICAL;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_Received_I_AM_Router(BACNET_ROUTER *router)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_I_AM_ROUTER;

 char txt[256];
 sprintf(txt,"Received I-Am-Router-To-Network");
 event->message->err_text = txt;

 event->message->description = "Received I-Am-Router-To-Network.";
 event->message->severity = BACAPI_SEVERITY_MESSAGE;
#ifdef BACAPI_BUILD_WIN32
 //GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



void BACAPI_Message_Received_Who_Is_Router(BACNET_ADDRESS *from)
{
 BACAPI_Event *event = BACAPI_CreateEvent();
 event->type = BACAPI_EVENT_MESSAGE;

 event->message = new BACAPI_MESSAGE;
 event->message->type = BACAPI_MESSAGE_WHO_IS_ROUTER;

 char txt[256];
 sprintf(txt,"Received Who-Is-Router-To-Network");
 event->message->err_text = txt;

 event->message->description = "Received Who-Is-Router-To-Network.";
 event->message->severity = BACAPI_SEVERITY_MESSAGE;
#ifdef BACAPI_BUILD_WIN32
// GetSystemTime(&event->message->systime);
#endif


 BACAPI_PushEvent(event);
}



