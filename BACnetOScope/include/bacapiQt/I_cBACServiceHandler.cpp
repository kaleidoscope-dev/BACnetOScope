#include "I_cBACServiceHandler.h"
#include "bacevent.h"
#include "bacapi.h"
#include "I_cBACdec.h"
#include "namedef.h"
#include "I_cBACLog.h"
#include "bacobject.h"
#include <string.h>


// NOTE while loops se pisu sa &&


I_cBACServiceHandler::I_cBACServiceHandler()
{
 //InitComplexList();
}


I_cBACServiceHandler::~I_cBACServiceHandler()
{
}



//=====================================================================================//
//===================[ HANDLE UNCONFIRMED SERVICE ]====================================//
// TODO: samo kod dekodiranja WHO_IS se upotrebljava bvlc_len i samo kod IP_DLINK.
//       zato sto kada se saljke WHO_IS preko neta, moze doci do paddinga. Ako se to dogodi
//       apdulen ce biti veci od ispravnog sto ce vratiti MALFORMED_PACKET kada se provjerava
//       if (apdu_len > 2) tj. da li je WHO_IS dosao s parametrima.
//       Trebalo bi cijeli kod prebaciti u bvlc_len ali ima problema kada se radi s segmentiranim porukama.
//=====================================================================================//
void I_cBACServiceHandler::Handle_Unconfirmed(uint8_t *apdu, BACNET_NPDU_DATA *npdu, BACNET_UNCONFIRMED_SERVICE service, int apdulen, int bvlc_len)
{
 switch (service)
        {
         case SERVICE_UNCONFIRMED_WHO_IS:
         {
          gLog->AppendLog("I_cBACServiceHandler::Handle_Unconfirmed: SERVICE_UNCONFIRMED_WHO_IS");
          BACAPI_ERROR err;
          uint32_t from=0, to=0;
          BACNET_DEVICE_RANGE range;

          // prije nego pokusas dekodirati from i to provjeri da li su uopce dosli
          // ti parametri. Ako nisu umjetno ih postavi posalji sve u redu event
          if (bvlc_len > 12)
             {
              uint16_t ret_len = 0;
              err = DecodeWhoIsRequest(apdu,&range, &ret_len);
              if (err != BACAPI_OK)
                 {
                  // lose kodiranje.
                  //gLog->AppendLog("ServiceHandler::Handle_Unconfirmed: SERVICE_UNCONFIRMED_WHO_IS: Error decoding Who-Is Request.");
                  //BACAPI_DeleteNPDUData(npdu);
                  return;
                 }
             }else {
                    //gLog->AppendLog("I_cBACServiceHandler::Handle_Unconfirmed: SERVICE_UNCONFIRMED_WHO_IS: No parameters, continuing...\n");

                    // WHO-IS je dosao bez parametara
                    range.from = 0;
                    range.to = 4194303;


                    BACAPI_Message_WhoIsRequest(npdu);
                   }
          //gLog->AppendLog("I_cBACServiceHandler::Handle_Unconfirmed: SERVICE_UNCONFIRMED_WHO_IS: Saljem Event!\n");

          // stvori i posalji event na stack
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->deviceRange = range;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          event->type = BACAPI_EVENT_WHO_IS;
          BACAPI_PushEvent(event);

          //if (apdu) delete []apdu;

          //BACAPI_DeleteNPDUData(npdu);  // npdu nam vise netreba. Sve najvaznije smo pokupili.
          break;
         }

         case SERVICE_UNCONFIRMED_I_AM:
         {
          int ret_len = 0;
          gLog->AppendLog("I_cBACServiceHandler::Handle_Unconfirmed: SERVICE_UNCONFIRMED_I_AM");
          BACNET_DEVICE *dev = BACAPI_CreateBACnetDevice();
          BACAPI_ERROR ret = DecodeIAmRequest(apdu,dev,&ret_len);
          if (ret != BACAPI_OK)
             {

             }

          dev->npduData = BACAPI_CreateNPDUData(npdu);
          dev->npduData->mstp_slave = false; // I-AM odgovori znace da ovaj device nije slave device

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_I_AM;
          event->device = dev;
          BACAPI_PushEvent(event);

          gLog->AppendLog("I_cBACServiceHandler::Handle_Unconfirmed: SERVICE_UNCONFIRMED_I_AM: Calling BACAPI_Message_ReceivedI_AM(dev)");

          BACAPI_Message_ReceivedI_AM(dev);

          //delete []apdu;
          break;
         }

         case SERVICE_UNCONFIRMED_EVENT_NOTIFICATION:
         {
          int ret_len = 0;
          gLog->AppendLog("SERVICE_UNCONFIRMED_EVENT_NOTIFICATION\n");
          BACNET_EVENT_NOTIFICATION *notification = new BACNET_EVENT_NOTIFICATION;

          BACAPI_ERROR bac_err = DecodeUnconfirmedEventNotification(apdu,notification,&ret_len);
          if (bac_err != BACAPI_OK)
             {
              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->pdu_type = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
              msg->type = bac_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_UNCONFIRMED_EVENT_NOTIFICATION;
              BACAPI_Message_Error(msg);
              return;
             }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_NOTIFICATION_EVENT;
          event->notification = notification;
          event->notification->source_device_address = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_PushEvent(event);

          BACAPI_Message_ReceivedEventNotification(notification);
          break;
         }


         case SERVICE_UNCONFIRMED_WHO_HAS:
         {

          uint32_t low = 0,hi = 0;
          BACNET_OBJECT_ID oid;
          string name;
          int ret = 0;

          ret = DecodeWhoHasRequest(apdu,&low, &hi, &oid, &name);
          if (ret < 0)
             {
              // TODO: BACAPI_ERROR_MALFORMED_PACKET
              return;
             }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_WHO_HAS_REQ;
          event->deviceRange;
          event->deviceRange.from = low;
          event->deviceRange.to = hi;
          event->oid.type = oid.type;
          event->oid.instance = oid.instance;
          event->extraText = name;
          BACAPI_PushEvent(event);
          //delete []apdu;
          break;
         }

		 case SERVICE_UNCONFIRMED_I_HAVE:
		 {
          int ret = 0;
          BACNET_DEVICE *dev = BACAPI_CreateBACnetDevice();
          BACNET_OBJECT *obj = BACAPI_CreateBACnetObject(0,0);
          ret = DecodeIHaveACK(apdu,dev,obj);
          if (ret < 0)
             {
              // TODO: BACAPI_ERROR_MALFORMED_PACKET
              return;
             }



          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_I_HAVE_ACK;
          event->device = dev;
          event->object = obj;
          BACAPI_PushEvent(event);

          //delete []apdu;
		  break;
		 }
		 case SERVICE_UNCONFIRMED_COV_NOTIFICATION:
         {
          int len = 0;
          BACNET_COV_NOTIFICATION *cov = new BACNET_COV_NOTIFICATION;

          BACAPI_ERROR bac_err = DecodeConfirmedCOVNotification(apdu,cov,apdulen,&len);

          if (bac_err != BACAPI_OK)
             {
              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->pdu_type = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
              msg->type = bac_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_UNCONFIRMED_COV_NOTIFICATION;
              BACAPI_Message_Error(msg);
              return;
             }
          BACAPI_Message_ReceivedConfirmedCOVNotification(cov);

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_UNCONFIRMED_COV_NOTIFICATION;
          event->cov = cov;
          BACAPI_PushEvent(event);
          //delete []apdu;
          break;
         }


         default:
         BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(npdu->remote);
         BACAPI_Message_UnsupportedService(PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST,service,adr);

         // NOTE: provjeriti gdje se prvo brise apdu za nepoznate servise
         //delete []apdu;
		 break;
        }

}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ HANDLE CONFIRMED SERVICE ]======================================//
// Servisi koji se bave dekodiranjem Ako smo mi Server
//=====================================================================================//
void I_cBACServiceHandler::Handle_Confirmed(uint8_t *apdu, BACNET_NPDU_DATA *npdu, BACNET_CONFIRMED_SERVICE service, int apduLen, int invokeId)
{
 int len = 0;
 switch (service)
        {
         case SERVICE_CONFIRMED_READ_PROPERTY:
         {
          BACNET_OBJECT_PROPERTY_REFERENCE *ref = new BACNET_OBJECT_PROPERTY_REFERENCE;

          int ret = DecodeReadPropertyRequest(apdu, apduLen, ref);
          if (ret < 0)
             {
              BACAPI_ERROR err_state = (BACAPI_ERROR)ret;
              BACNET_ADDRESS *remote_adr = new BACNET_ADDRESS;
              memcpy(remote_adr,npdu->remote,sizeof(BACNET_ADDRESS));
              Handle_Error_State(err_state,invokeId,remote_adr);

              delete ref;
              return;
             }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_RP_REQ;
          event->objPropRef = ref;
          event->invokeId = invokeId;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          event->srcDataLink = npdu->dataLink;
          BACAPI_PushEvent(event);
          break;
         }

         case SERVICE_CONFIRMED_COV_NOTIFICATION:
         {
          BACNET_COV_NOTIFICATION *cov = new BACNET_COV_NOTIFICATION;

          BACAPI_ERROR bac_err = DecodeConfirmedCOVNotification(apdu,cov,apduLen,&len);

          if (bac_err != BACAPI_OK)
             {
              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->invoke_id = invokeId;
              msg->pdu_type = PDU_TYPE_CONFIRMED_SERVICE_REQUEST;
              msg->type = bac_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_CONFIRMED_COV_NOTIFICATION;
              BACAPI_Message_Error(msg);
              return;
             }
          BACAPI_Message_ReceivedConfirmedCOVNotification(cov);

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_CONFIRMED_COV_NOTIFICATION;
          event->cov = cov;
          event->invokeId = invokeId;
          BACAPI_PushEvent(event);

          break;
         }




         case SERVICE_CONFIRMED_READ_PROP_MULTIPLE:
         {
          BACNET_OBJECT_PROPERTY_REFERENCE *ref = new BACNET_OBJECT_PROPERTY_REFERENCE;
          int ret = DecodeReadPropertyMultipleRequest(apdu, apduLen, ref);
          if (ret < 0)
             {
              // ERROR;
              // BACAPI_CreateErrorEvent();
              delete ref;
              return;
             }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_RPM_REQ;
          event->objPropRef = ref;
          event->invokeId = invokeId;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          event->srcDataLink = npdu->dataLink;
          BACAPI_PushEvent(event);
          break;
         }

         case SERVICE_CONFIRMED_WRITE_PROPERTY:
         {

          int ret = 0;
          BACNET_OBJECT *object = new BACNET_OBJECT;
          BACNET_OBJECT_PROPERTY *property = new BACNET_OBJECT_PROPERTY;

          ret = DecodeWritePropertyRequest(apdu,apduLen,object,property);

          if (ret < 0)
             {
              // ERROR;
              // BACAPI_CreateErrorEvent();
              BACAPI_DeleteObjectProperty(property);
              BACAPI_DeleteObject(object);
              return;
             }

          int size = property->complexValue.vDeviceObjectReference.size();
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_WP_REQ;
          event->object = object;
          event->property = property;
          event->invokeId = invokeId;
          event->srcDataLink = npdu->dataLink;
          //gLog->AppendLog("ServiceHandler: CONF_WRITE_PROP: adr = %s\n",npdu->remote->adr);
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          //gLog->AppendLog("ServiceHandler: CONF_WRITE_PROP: adr = %d, invoke = %d\n",event->address->len,invokeId);
          //for (int c=0;c<event->address->len;c++)
          //    {
          //      gLog->AppendLog("adr[%d] = %d\n",c,event->address->adr[c]);
          //    }
          BACAPI_PushEvent(event);
          break;
         }


         case SERVICE_CONFIRMED_CREATE_OBJECT:
         {
          BACNET_OBJECT_ID oid;

          int ret = DecodeCreateObjectRequest(apdu,&oid);
          if (ret < 0)
             {
              // ERROR;
              // BACAPI_CreateErrorEvent();
              return;
             }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_CREATE_OBJECT;
          event->oid.type = oid.type;
          event->oid.instance = oid.instance;
          event->invokeId = invokeId;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_PushEvent(event);

          break;
         }

         case SERVICE_CONFIRMED_DELETE_OBJECT:
         {
          BACNET_OBJECT_ID oid;

          int ret = DecodeDeleteObjectRequest(apdu,&oid);
          if (ret < 0)
             {
              // ERROR;
              // BACAPI_CreateErrorEvent();
              return;
             }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_DELETE_OBJECT;
          event->oid.type = oid.type;
          event->oid.instance = oid.instance;
          event->invokeId = invokeId;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_PushEvent(event);
          break;
         }

    	 case SERVICE_CONFIRMED_ATOMIC_READ_FILE:
    	 {
    	  gLog->AppendLog("Dobio atomic read file request\n");
    	  BACNET_FILE *bac_file = new BACNET_FILE;
    	  int ret = DecodeAtomicReadFileRequest(apdu,bac_file);
    	  if (ret < 0)
    	    {
    	     delete bac_file;
    	     // ERROR;
             // BACAPI_CreateErrorEvent();
             return;
    	    }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_ATOMIC_READ_FILE_REQ;
          event->file_data = bac_file;
          event->invokeId = invokeId;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_PushEvent(event);
          break;
    	 }

         case SERVICE_CONFIRMED_ATOMIC_WRITE_FILE:
         {
          int ret = 0;
          gLog->AppendLog("Service Handler :: ExecuteComplexAck: SERVICE_CONFIRMED_ATOMIC_WRITE_FILE_REQ\n");
          BACNET_FILE *file_data = new BACNET_FILE;
          ret = DecodeAtomicWriteFile(apdu,file_data);
          if (ret < 0)
             {
              delete file_data;
              return;
             }
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_ATOMIC_WRITE_FILE_REQ;
		  event->invokeId = invokeId;
		  event->file_data = file_data;
		  event->address = BACAPI_CreateBACnetAddress(npdu->remote);
		  BACAPI_PushEvent(event);
		  break;
         }


		 case SERVICE_CONFIRMED_READ_RANGE:
		 {
		  BACNET_READ_RANGE *range = new BACNET_READ_RANGE;
		  int ret = DecodeReadRangeRequest(apdu,range);
    	  if (ret < 0)
    	     {
              delete range;
     	     // ERROR;
             // BACAPI_CreateErrorEvent();
             return;
    	    }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_READ_RANGE_REQ;
          event->range_data = range;
          event->invokeId = invokeId;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_PushEvent(event);
          break;
		 }

		 case SERVICE_CONFIRMED_EVENT_NOTIFICATION:
		 {
          int ret_len = 0;
          gLog->AppendLog("SERVICE_CONFIRMED_EVENT_NOTIFICATION\n");
          BACNET_EVENT_NOTIFICATION *notification = new BACNET_EVENT_NOTIFICATION;

          BACAPI_ERROR bac_err = DecodeConfirmedEventNotification(apdu,notification,&ret_len);
          if (bac_err != BACAPI_OK)
             {
              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->pdu_type = PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST;
              msg->type = bac_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_UNCONFIRMED_EVENT_NOTIFICATION;
              BACAPI_Message_Error(msg);
              return;
             }

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_NOTIFICATION_EVENT;
          event->notification = notification;
          event->notification->source_device_address = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_PushEvent(event);

          BACAPI_Message_ReceivedEventNotification(notification);
          break;
		 }


         // ostali servisi - posalji REJECT-PDU
		 case SERVICE_CONFIRMED_ACKNOWLEDGE_ALARM:
		 case SERVICE_CONFIRMED_GET_ALARM_SUMMARY:
		 case SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY:
		 case SERVICE_CONFIRMED_GET_EVENT_INFORMATION:
		 case SERVICE_CONFIRMED_SUBSCRIBE_COV:
		 case SERVICE_CONFIRMED_SUBSCRIBE_COV_PROPERTY:
		 case SERVICE_CONFIRMED_LIFE_SAFETY_OPERATION:
		 case SERVICE_CONFIRMED_ADD_LIST_ELEMENT:
		 case SERVICE_CONFIRMED_REMOVE_LIST_ELEMENT:
		 case SERVICE_CONFIRMED_READ_PROP_CONDITIONAL:
		 case SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE:
		 case SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL:
		 case SERVICE_CONFIRMED_PRIVATE_TRANSFER:
		 case SERVICE_CONFIRMED_TEXT_MESSAGE:
		 case SERVICE_CONFIRMED_REINITIALIZE_DEVICE:
		 case SERVICE_CONFIRMED_VT_OPEN:
		 case SERVICE_CONFIRMED_VT_CLOSE:
		 case SERVICE_CONFIRMED_VT_DATA:
		 case SERVICE_CONFIRMED_AUTHENTICATE:
		 {
          BACAPI_ERROR err_state = BACAPI_ERROR_UNRECOGNIZED_SERVICE;
          BACNET_ADDRESS *remote_adr = new BACNET_ADDRESS;
          memcpy(remote_adr,npdu->remote,sizeof(BACNET_ADDRESS));   // Izbrisati NPDU?
          Handle_Error_State(err_state,invokeId,remote_adr);
		  break;
		 }

         default:
		 {
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_REJECT;
          event->rejCode = REJECT_UNRECOGNIZED_SERVICE;
          event->invokeId = invokeId;
          event->address = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_PushEvent(event);
		  break;
		 }
        }
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ HANDLE COMPLEX ACK ]============================================//
//=====================================================================================//
void I_cBACServiceHandler::Handle_ComplexACK(uint8_t *apdu, BACNET_NPDU_DATA *npdu,BACNET_CONFIRMED_SERVICE service, int apduLen, uint8_t invokeId)
{
 //gLog->AppendLog("I_cBACServiceHandler::Handle_ComplexACK: Service: %d",service);

 switch (service)
        {
         case SERVICE_CONFIRMED_READ_PROPERTY:
         {
          int ret_len = 0;
          gLog->AppendLog("I_cBACServiceHandler::Handle_ComplexACK: SERVICE_CONFIRMED_READ_PROPERTY:");
          BACNET_OBJECT_ID oid;
          BACNET_OBJECT_PROPERTY *property = BACAPI_CreateObjectProperty();

          BACAPI_ERROR err = DecodeReadPropertyComplexAck(apdu,apduLen,property,&oid, &ret_len);
          if (err != BACAPI_OK)
             {
              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->invoke_id = invokeId;
              msg->pdu_type = PDU_TYPE_COMPLEX_ACK;
              msg->type = err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_CONFIRMED_READ_PROPERTY;

              BACAPI_Message_Error(msg);
              return;
             }

          // 2 razlicita eventa, ako se citao samo element 0 onda posalji razliciti event on obicnog read propertija
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  unsigned int i = BACNET_ARRAY_ALL;

          if (property->index != (uint16_t)(BACNET_ARRAY_ALL))
             {
							event->type = BACAPI_EVENT_RP_ACK_ARRAY_ELEMENT;
							event->property = property;
							event->invokeId = invokeId;
							event->oid.instance = oid.instance;
							event->oid.type = oid.type;
							gLog->AppendLog("ServiceHandler::Handle_ComplexACK: index = %d\n",property->index);
             }else {
					event->type = BACAPI_EVENT_RP_ACK;
					event->property = property;
					event->invokeId = invokeId;
					event->oid.instance = oid.instance;
					event->oid.type = oid.type;
				   }


          event->address = BACAPI_CreateBACnetAddress(npdu->remote); // TODO: izbrisati alokaciju
          BACAPI_PushEvent(event);
          delete []apdu;

          break;
         }

         case SERVICE_CONFIRMED_READ_PROP_MULTIPLE:
         {
          int ret_len = 0;
          BACNET_OBJECT *object = new BACNET_OBJECT;
          BACAPI_ERROR ret_err = DecodeReadPropertyMultipleComplexAck(apdu,apduLen, object, &ret_len);
          if (ret_err != BACAPI_OK)
             {
              gLog->AppendLog("ServiceHandler::SERVICE_CONFIRMED_READ_PROP_MULTIPLE: DecodeReadPropertyMultiComplexAck(), ret < 0\n");
              BACAPI_SetLastError(BACAPI_ERROR_MALFORMED_PACKET);
              gLog->AppendLog("ServiceHandler::Handle_ComplexACK: Malformed packet! Discarding...\n");
             }

/*
 debagiranje za linux zbog sranja s imenima
 
            for (int c=0;c<object->vProperties.size();c++)
                  {
                   string s;
                   s = BACAPI_BACnetPropertyName(object->vProperties[c]->property);
                   s += ": ";

				   BACNET_OBJECT_PROPERTY *prop = object->vProperties[c];

                   if (object->vProperties[c]->vValueList.size() > 0)
                      {
                       BACNET_APPLICATION_DATA_VALUE *val = object->vProperties[c]->vValueList[0];
                       if (object->vProperties[c]->property == PROP_OBJECT_NAME)
                          {
                           printf("name");
                          }
                       printf("");
                       //printf("object value: %s\n"
                        s += BACAPI_GetStringValue(object->vProperties[c]->vValueList[0]);

                        printf("s = %s\n",s.c_str());
                      }else s =+ "No Value";
                  }
*/


		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_RPM_ACK;
		  event->object = object;
		  event->invokeId = invokeId;
		  BACAPI_PushEvent(event);


          break;
         }



         case SERVICE_CONFIRMED_GET_ALARM_SUMMARY:
         {
          gLog->AppendLog("Service Handler :: ExecuteComplexAck: SERVICE_CONFIRMED_GET_ALARM_SUMMARY\n");
          int ret_len = 0;
          BACNET_ALARM *alarm = new BACNET_ALARM;
          BACAPI_ERROR ret_err = DecodeAlarmSummary(apdu,alarm,apduLen,&ret_len);
          if (ret_err != BACAPI_OK)
             {
              BACAPI_DeleteBACnetAlarm(alarm);
              delete alarm;
              alarm = NULL;

              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->invoke_id = invokeId;
              msg->pdu_type = PDU_TYPE_COMPLEX_ACK;
              msg->type = ret_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_CONFIRMED_GET_ALARM_SUMMARY;
              BACAPI_Message_Error(msg);
              return;
             }

          // ako nema aktivnog alarma, alarm->alarmSummary ce biti NULL
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_ALARM_SUMMARY;
		  event->alarm = alarm;
		  event->invokeId = invokeId;
		  BACAPI_PushEvent(event);


          BACAPI_Message_ReceivedAlarmSummary(alarm);
          break;
         }

         case SERVICE_CONFIRMED_ATOMIC_READ_FILE:
         {
          int ret = 0;
          gLog->AppendLog("Service Handler :: ExecuteComplexAck: SERVICE_CONFIRMED_ATOMIC_READ_FILE");
          BACNET_FILE *file_data = new BACNET_FILE;
          BACAPI_ERROR ret_err = DecodeAtomicReadFileAck(apdu,file_data,&ret);
          if (ret_err != BACAPI_OK)
             {
              delete file_data;
              file_data = NULL;

              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->invoke_id = invokeId;
              msg->pdu_type = PDU_TYPE_COMPLEX_ACK;
              msg->type = ret_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_CONFIRMED_ATOMIC_READ_FILE;
              BACAPI_Message_Error(msg);
              return;
             }

		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_ATOMIC_READ_FILE_ACK;
		  event->file_data = file_data;
		  event->invokeId = invokeId;
		  event->pdu_type = PDU_TYPE_COMPLEX_ACK;
		  BACAPI_PushEvent(event);

          break;
         }

         case SERVICE_CONFIRMED_ATOMIC_WRITE_FILE:
         {
          gLog->AppendLog("Service Handler :: ComplexAck: SERVICE_CONFIRMED_ATOMIC_WRITE_FILE");
          BACNET_FILE *file_data = new BACNET_FILE;
          BACAPI_ERROR ret_err = DecodeAtomicWriteFileAck(apdu,file_data);
          if (ret_err != BACAPI_OK)
             {
              delete file_data;
              file_data = NULL;

              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->invoke_id = invokeId;
              msg->pdu_type = PDU_TYPE_COMPLEX_ACK;
              msg->type = ret_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_CONFIRMED_ATOMIC_WRITE_FILE;
              BACAPI_Message_Error(msg);
              return;
             }

		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_ATOMIC_WRITE_FILE_ACK;
		  event->file_data = file_data;
		  event->invokeId = invokeId;
		  event->pdu_type = PDU_TYPE_COMPLEX_ACK;
		  BACAPI_PushEvent(event);
          break;
         }



         case SERVICE_CONFIRMED_READ_RANGE:
         {
          int ret = 0;
          gLog->AppendLog("Service Handler :: ExecuteComplexAck: SERVICE_CONFIRMED_READ_RANGE\n");
          BACNET_OBJECT *object = new BACNET_OBJECT;
          ret = DecodeReadRangeComplexAck(apdu, object);
          if (ret < 0)
             {
              // izbrisi property ako je stvoren
              if (object->vProperties.size() > 0)
                 {
                  BACNET_OBJECT_PROPERTY *prop = object->vProperties[0];
                  BACAPI_DeleteObjectProperty(prop);  // delete PROP_LOG_BUFFER complex propertyu
                 }
              object->vProperties.clear();
              delete object;
              // TODO: API ERROR
              return;
             }

		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_READ_RANGE_ACK;
		  event->object = object;
		  event->invokeId = invokeId;
		  BACAPI_PushEvent(event);
          break;
         }

         case SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY:
         {
          BACAPI_ERROR bac_err = BACAPI_OK;
          int ret_len = 0;
          BACNET_ENROLLMENT_SUMMARY *summary = new BACNET_ENROLLMENT_SUMMARY;
          bac_err = DecodeEnrollmentSummary(apdu,summary,apduLen,&ret_len);
          if (bac_err != BACAPI_OK)
             {
              delete summary;

              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->invoke_id = invokeId;
              msg->pdu_type = PDU_TYPE_COMPLEX_ACK;
              msg->type = bac_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_CONFIRMED_GET_ENROLLMENT_SUMMARY;
              BACAPI_Message_Error(msg);
              return;
             }

		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_ENROLLMENT_SUMMARY;
		  event->summary = summary;
		  event->invokeId = invokeId;
		  BACAPI_PushEvent(event);

          BACAPI_Message_ReceivedEnrollmentSummary(summary);

          break;
         }

         case SERVICE_CONFIRMED_GET_EVENT_INFORMATION:
         {
          BACAPI_ERROR bac_err = BACAPI_OK;
          int ret_len = 0;
          BACNET_EVENT_INFORMATION *info = new BACNET_EVENT_INFORMATION;
          bac_err = DecodeEventInformation(apdu,info,apduLen,&ret_len);
          if (bac_err != BACAPI_OK)
             {
              delete info;

              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->invoke_id = invokeId;
              msg->pdu_type = PDU_TYPE_COMPLEX_ACK;
              msg->type = bac_err;
              msg->severity = BACAPI_SEVERITY_ERROR;
              msg->service = SERVICE_CONFIRMED_GET_EVENT_INFORMATION;
              BACAPI_Message_Error(msg);
              return;
             }

		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_EVENT_INFORMATION;
		  event->information = info;

		  event->invokeId = invokeId;
		  BACAPI_PushEvent(event);

          BACAPI_Message_ReceivedEventInformation(info);

          break;
         }

         default:
         break;
        }


}
//_____________________________________________________________________________________\\






//=====================================================================================//
//===================[ HANDLE ERROR STATE ]============================================//
//=====================================================================================//
void I_cBACServiceHandler::Handle_Error_State(BACAPI_ERROR state, uint8_t invoke, BACNET_ADDRESS *source)
{
 switch(state)
       {
        case BACAPI_ERROR_MISSING_REQUIRED_PARAMETER:
        {
         BACAPI_Event *event = BACAPI_CreateEvent();
         event->type = BACAPI_EVENT_REJECT;
         event->rejCode = REJECT_MISSING_REQUIRED_PARAMETER;
         event->invokeId = invoke;
         event->address = source;
         BACAPI_PushEvent(event);
         break;
        }


        case BACAPI_ERROR_UNRECOGNIZED_SERVICE:
        {
         BACAPI_Event *event = BACAPI_CreateEvent();
         event->type = BACAPI_EVENT_REJECT;
         event->rejCode = REJECT_UNRECOGNIZED_SERVICE;
         event->invokeId = invoke;
         event->address = source;
         BACAPI_PushEvent(event);
        }

        case BACAPI_ERROR_INVALID_TAG:
        {
         BACAPI_Event *event = BACAPI_CreateEvent();
         event->type = BACAPI_EVENT_REJECT;
         event->rejCode = REJECT_INVALID_TAG;
         event->invokeId = invoke;
         event->address = source;
         BACAPI_PushEvent(event);
        }

        default:
        break;
       }

}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ DECODE: I HAVE ]====================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeIHaveACK(uint8_t *apdu, BACNET_DEVICE *dev, BACNET_OBJECT *obj)
{
 int len = 0;
 BACNET_APPLICATION_DATA_VALUE val;
 len += gDec->bacapp_decode_application_data(apdu,1476,&val);
 dev->oid.type = val.type.Object_Id.type;
 dev->oid.instance = val.type.Object_Id.instance;
 len += gDec->bacapp_decode_application_data(&apdu[len],1476,&val);
 obj->oid.type = val.type.Object_Id.type;
 obj->oid.instance = val.type.Object_Id.instance;

 // TODO: object name decode (app tag)

 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ DECODE READ RANGE REQUEST ]=====================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeReadRangeRequest(uint8_t *apdu, BACNET_READ_RANGE *range)
{
 unsigned len = 0;
 unsigned TagLen = 0;
 uint8_t tag_number = 0;
 uint32_t len_value_type = 0;
 uint16_t type = 0;  /* for decoding */
 uint32_t UnsignedTemp;

 // Tag 0: Object ID
 if (!gDec->decode_is_context_tag(&apdu[len++], 0))
     return -1;

 len += gDec->decode_object_id(&apdu[len], &type, &range->oid.instance);
 range->oid.type= (BACNET_OBJECT_TYPE) type;

 // Tag 1: Property ID
 len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);

 if (tag_number != 1)
     return -1;

 len += gDec->decode_enumerated(&apdu[len], len_value_type, &UnsignedTemp);
 range->property = (BACNET_PROPERTY_ID) UnsignedTemp;
 //range-> = RR_OVERHEAD; // Start with the fixed overhead

 // Tag 2: Optional Array Index - set to ALL if not present
 range->arrayIndex = BACNET_ARRAY_ALL;  //Assuming this is the most common outcome...
 TagLen = (unsigned) gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
 // specificiran je array_index
 if (tag_number == 2)
    {
     len += TagLen;
     len += gDec->decode_unsigned(&apdu[len], len_value_type, &UnsignedTemp);
     range->arrayIndex = UnsignedTemp;
     //rrdata->Overhead += RR_INDEX_OVERHEAD;  // Allow for this in the response
    }

    //    /* And/or optional range selection- Tags 3, 6 and 7 */
    //    rrdata->RequestType = RR_READ_ALL;      /* Assume the worst to cut out explicit checking later */
    //    if (len < apdu_len) {
    //        /*
    //         * Note: We pick up the opening tag and then decode the parameter types we recognise.
    //         * We deal with the count and the closing tag in each case statement even though it
    //         * might appear that we could do them after the switch statement as common elements.
    //         * This is so that if we receive a tag we don't recognise, we don't try to decode it
    //         * blindly and make a mess of it.
    //         */
 len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
 uint8_t *tmp = &apdu[len];

 switch (tag_number)
        {
         case 3:        /* ReadRange by position */
          range->requestType = RANGE_BY_POSITION;
          len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
          len += gDec->decode_unsigned(&apdu[len], len_value_type,&range->rangeParam.index);
          len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);
          len += gDec->decode_signed(&apdu[len], len_value_type, &range->rangeParam.count);
          len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);  // closing tag [3]
          break;


		 case 6:        /* ReadRange by sequence number */
	      range->requestType = RANGE_BY_SEQUENCE;
	      len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
		  len += gDec->decode_unsigned(&apdu[len], len_value_type,&range->rangeParam.seqNumber);
		  len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);
		  len += gDec->decode_signed(&apdu[len], len_value_type,&range->rangeParam.count);
		  len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);  // closgin tag [6]
		  //rrdata->Overhead += RR_1ST_SEQ_OVERHEAD;    /* Allow for this in the response */
		  break;


         case 7:        /* ReadRange by time stamp */
          range->requestType = RANGE_BY_TIME;
          len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);
          len += gDec->decode_date(&apdu[len], &range->rangeParam.datetime.date);
          len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);
          len += gDec->decode_bacnet_time(&apdu[len], &range->rangeParam.datetime.time);
          len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value_type);
          len += gDec->decode_signed(&apdu[len], len_value_type,&range->rangeParam.count);
          len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number,&len_value_type);
          //rrdata->Overhead += RR_1ST_SEQ_OVERHEAD;    /* Allow for this in the response */
          break;

         default:
          return -1;
        }

 return len;
}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ DECODE: READ RANGE COMPLEX ACK ]================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeReadRangeComplexAck(uint8_t *apdu, BACNET_OBJECT *object)
{
 int ret = 0;
 uint32_t len_val = 0,val = 0;
 int len = 0;
 uint8_t tag = 0;

 ret = gDec->decode_context_object_id(&apdu[len],0,&object->oid.type,&object->oid.instance);
 if (ret <= 0) return -1;
 len += ret;

 // Trend_Log i Trend_Log_Multi se dekodiraju na drugaciji nacin
 if (object->oid.type == OBJECT_TREND_LOG)
    {
     len += DecodeReadRangeAck(&apdu[len],object);
    }else if (object->oid.type == OBJECT_TREND_LOG_MULTIPLE)
             {
              len += DecodeReadRangeMultiAck(&apdu[len],object);
             }
 else {
       return -1;
      }

 return len;
}


int I_cBACServiceHandler::DecodeReadRangeAck(uint8_t *apdu, BACNET_OBJECT *object)
{
 int ret = 0;
 uint32_t len_val = 0,val = 0;
 int len = 0;
 uint8_t tag = 0;

 len += ret;

 BACNET_OBJECT_PROPERTY *prop = new BACNET_OBJECT_PROPERTY;  // dummy property kontejner
 ret = gDec->decode_context_enumerated(&apdu[len],1,&val);
 if (ret <= 0) return -1;

 len += ret;
 prop->property = (BACNET_PROPERTY_ID)val;

 // NOTE: context tag [2] ????

 BACNET_BIT_STRING tmp_bit_str;
 ret = gDec->decode_context_bitstring(&apdu[len],3,&tmp_bit_str);
 if (ret <= 0) return -1;

 len += ret;

 // item count
 uint32_t item_count = 0;
 ret = gDec->decode_context_unsigned(&apdu[len],4,&item_count);
 if (ret <= 0) return -1;

 len += ret;

 if (gDec->decode_is_opening_tag_number(&apdu[len],5))
    {
     len++;
     // by position
     for (int c=0;c<item_count;c++)
         {
          uint8_t *tmp = &apdu[len];

		  if (gDec->decode_is_opening_tag_number(&apdu[len],0))
			 {
			  len++;  
			  
			  // [0] BACnetDateTime
			  BACNET_LOG_RECORD *rec = new BACNET_LOG_RECORD;
			  len += gDec->decode_application_date(&apdu[len],&rec->timeStamp.date);
			  len += gDec->decode_application_time(&apdu[len],&rec->timeStamp.time);
		      len++;  // closing tag [0F]

		       if (gDec->decode_is_opening_tag_number(&apdu[len],1))  // value
		          {
		           len++;

		           gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
		           rec->recType = tag;
		           switch (tag)
		                  {
						   case 0: break; // log status - ne hendlamo
						   case 1:
						   {
						    // nesmijem upotreijbiti gDec->decode_context_boolean(&apdu[len]);  zato sto
						    // ta funkcija ne dekodira konext tag a nesmijemo je ispraviti zato sto moramo
						    // provjeriti kako ce djelovati na API, tako da za sada moram rucno dekodirati ovo
						    if (gDec->decode_is_context_tag(&apdu[len], 1))
						       {
						        len++;  // context
						        uint8_t *tmp = &apdu[len];
						        rec->logDatum.Boolean = (bool)apdu[len];
						        len++;  // bool value
						       }else return -1;
						    tmp = &apdu[len];
						    break;
						   }
						   case 2: len += gDec->decode_context_real(&apdu[len],2,&rec->logDatum.Real);       break;
						   case 3: len += gDec->decode_context_enumerated(&apdu[len],3,&rec->logDatum.Enum); break;
						   case 4: len += gDec->decode_context_unsigned(&apdu[len],4,&rec->logDatum.Unsigned); break;
						   case 5: /*len += gDec->decode_context_unsigned(&apdu[len],5,&rec->logDatum.Signed);*/ break;
						   case 6: len += gDec->decode_context_bitstring(&apdu[len],6,&rec->logDatum.Bit_String); break;
						   case 7: break;
						   case 8: break; // error - ne hendlamo
						   case 9:
						    len += gDec->decode_context_real(&apdu[len],9,&rec->logDatum.Time_Change);
						   //printf("time_change = %f:\n",rec->logDatum.Time_Change);


						   break;
						   case 10: break;  // fuck ne
						   default:
						   return -1;
		                  }
		           //len += gDec->decode_context_real(&apdu[len],2,&rec->logDatum.Real);
		           uint8_t *tmp = &apdu[len];
                   len++;  // closing tag [1F]

                   // OPTIONAL status flags
                   if (gDec->decode_is_context_tag(&apdu[len],2))
                      {
					   BACNET_BIT_STRING tmp_bit_str;
					   ret = gDec->decode_context_bitstring(&apdu[len],2,&tmp_bit_str);
					   if (ret <= 0) return -1;
						else len += ret;
				      }
		          }
		      prop->complexValue.vLogRecordList.push_back(rec);
			 }

          }
    }

 object->vProperties.push_back(prop);
 return len;
}



int I_cBACServiceHandler::DecodeReadRangeMultiAck(uint8_t *apdu, BACNET_OBJECT *object)
{
 int ret = 0;
 uint32_t len_val = 0,val = 0;
 int len = 0;
 uint8_t tag = 0;
 BACNET_OBJECT_PROPERTY *prop = new BACNET_OBJECT_PROPERTY;  // dummy property kontejner

 // property [1]
 ret = gDec->decode_context_enumerated(&apdu[len],1,&val);
 if (ret <= 0) return -1;
 len += ret;
 prop->property = (BACNET_PROPERTY_ID)val;

 // NOTE: context tag [2] - array_index?

 // [2]
 BACNET_BIT_STRING tmp_bit_str;
 ret = gDec->decode_context_bitstring(&apdu[len],3,&tmp_bit_str);
 if (ret <= 0) return -1;

 len += ret;

 // record count [4]
 uint32_t item_count = 0;
 ret = gDec->decode_context_unsigned(&apdu[len],4,&item_count);
 if (ret <= 0) return -1;

 len += ret;

 if (gDec->decode_is_opening_tag_number(&apdu[len],5))
    {
     len++;
     // by position
     for (int c=0;c<item_count;c++)
         {
          uint8_t *tmp = &apdu[len];

		  if (gDec->decode_is_opening_tag_number(&apdu[len],0))
			 {
			  len++;
			  BACNET_LOG_MULTIPLE_RECORD *rec = new BACNET_LOG_MULTIPLE_RECORD;
			  len += gDec->decode_application_date(&apdu[len],&rec->timeStamp.date);
			  len += gDec->decode_application_time(&apdu[len],&rec->timeStamp.time);
		      len++;  // closing tag [0F]

		       if (gDec->decode_is_opening_tag_number(&apdu[len],1))  // opening value
		          {
		           len++;
                   // za svaki propety koji se prati
                   while (!gDec->decode_is_closing_tag_number(&apdu[len],1))
                         {
                          BACNET_LOG_DATA_TYPE ltype;
						  gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
						  rec->logData.recType = tag;
						  switch (tag)
								 {
								  case 0:  // bool
								  {
								   // nesmijem upotreijbiti gDec->decode_context_boolean(&apdu[len]);  zato sto
								   // ta funkcija ne dekodira konext tag a nesmijemo je ispraviti zato sto moramo
								   // provjeriti kako ce djelovati na API, tako da za sada moram rucno dekodirati ovo
								   if (gDec->decode_is_context_tag(&apdu[len], 0))
									  {
									   len++;  // context
									   uint8_t *tmp = &apdu[len];
									   ltype.Boolean = (bool)apdu[len];
									   len++;  // bool value
									  }else return -1;
								   tmp = &apdu[len];
								   break;
								  }
								  case 1: //  real
								   len += gDec->decode_context_real(&apdu[len],1,&ltype.Real);
								   break;
								  case 2: // enum
								   len += gDec->decode_context_enumerated(&apdu[len],2,&ltype.Enum);
								   break;
								  case 3:  // uint
								   len += gDec->decode_context_unsigned(&apdu[len],3,&ltype.Unsigned);
								   break;
								  case 4: len += gDec->decode_context_signed(&apdu[len],4,&ltype.Signed); break;
								  case 5: len += gDec->decode_context_bitstring(&apdu[len],5,&ltype.Bit_String); break;
								  case 6: break;
								  case 8: break;  // fuck ne
								  default:
								  return -1;
								 }
                          rec->logData.vLogData.push_back(ltype);

						  //len += gDec->decode_context_real(&apdu[len],2,&rec->logDatum.Real);
						  uint8_t *tmp = &apdu[len];

						  // OPTIONAL status flags
						  if (gDec->decode_is_context_tag(&apdu[len],2))
							 {
							  BACNET_BIT_STRING tmp_bit_str;
							  ret = gDec->decode_context_bitstring(&apdu[len],2,&tmp_bit_str);
							  if (ret <= 0) return -1;
							   else len += ret;
							 }

						 }
                   len++;  // closing tag [1F]
		          }
		      prop->complexValue.vLogMultipleList.push_back(rec);
			 }

          }
    }

 object->vProperties.push_back(prop);
 return len;
}




//=====================================================================================//
//===================[ DECODE: ATOMIC READ FILE REQUEST ]==============================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeAtomicReadFileRequest(uint8_t *apdu, BACNET_FILE *bac_file)
{
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0;

 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);

 BACNET_APPLICATION_DATA_VALUE val;
 val.application_tag = (BACNET_APPLICATION_TAG)tag;

 int ret = gDec->bacapp_decode_data(&apdu[len],tag,len_val,&val);
 if (ret < 0)
    {
     return -1;
    }
 len += ret;

 bac_file->oid.type = val.type.Object_Id.type;
 bac_file->oid.instance = val.type.Object_Id.instance;

 if (gDec->decode_is_opening_tag_number(&apdu[len],0))
     bac_file->access_type = 0;
     else if (gDec->decode_is_opening_tag_number(&apdu[len],1))
             bac_file->access_type = 1;
     else return -1;

 len++;

 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
 ret = gDec->bacapp_decode_data(&apdu[len],tag,len_val,&val);
 if (ret < 0)
    {
     return -1;
    }
 len += ret;
 bac_file->start_pos = val.type.Signed_Int;

 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
 ret = gDec->bacapp_decode_data(&apdu[len],tag,len_val,&val);
 if (ret < 0)
    {
     return -1;
    }
 len += ret;
 bac_file->octet_count = val.type.Unsigned_Int;

 len++;  // zatvarajuci tag
 return len;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ DECODE: ATOMIC WRITE FILE ACK ]==================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeAtomicWriteFile(uint8_t *data, BACNET_FILE *file_data)
{
 int ret = 0, len = 0;
 BACNET_APPLICATION_DATA_VALUE val;

 ret = gDec->bacapp_decode_application_data(&data[len],1476,&val);
 if (ret < 0)
    {
     return -1;
    }
 len += ret;
 file_data->oid.type = val.type.Object_Id.type;
 file_data->oid.instance = val.type.Object_Id.instance;


 // access mode
 ret = gDec->decode_is_opening_tag_number(&data[len],0); // stream access?
 if (ret < 0)
    {
     ret = gDec->decode_is_opening_tag_number(&data[len],1);  // record access?
     if (ret < 0)
        {
         return -1;  // krivo kodiranje
        }else {
               file_data->access_type = 1;
              }
     return -1;
    }else file_data->access_type = 0;
 len+=ret;

 // start pos
 ret = gDec->bacapp_decode_application_data(&data[len],1476,&val);
 if (ret < 0)
    {
     return -1;
    }
 len+=ret;
 file_data->start_pos = val.type.Signed_Int;

 // octet stream
 ret = gDec->bacapp_decode_application_data(&data[len],1476,&val);
 if (ret < 0)
    {
     return -1;
    }
 len += ret;


BACAPI_CopyBACnetOctetString(&file_data->data, &val.type.Octet_String);

 //file_data->data.length = val.type.Octet_String.length;
 //memcpy(file_data->data.value,val.type.Octet_String.value,val.type.Octet_String.length);


 ret = gDec->decode_is_closing_tag_number(&data[len],file_data->access_type);
 if (ret < 0)
    {
     return -1;
    }
 len += ret;
 return len;

}


//_____________________________________________________________________________________\\


//=====================================================================================//
//===================[ DECODE: ATOMIC READ FILE ACK ]==================================//
// TODO: detektirati i ispravno vratiti ako dodju greske
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeAtomicReadFileAck(uint8_t *data, BACNET_FILE *file_data, int *ret_len)
{
 int ret = 0, len = 0;
 uint32_t len_val = 0;
 int32_t value = 0;
 uint8_t tag = 0;
 BACNET_APPLICATION_DATA_VALUE val;

 // EOF?
 len += gDec->decode_tag_number_and_value(&data[0], &tag, &len_val);
 if (tag != BACNET_APPLICATION_TAG_BOOL)
    {
     return BACAPI_ERROR_INVALID_TAG;
    }
 file_data->eof = (bool)len_val;

 len += gDec->decode_tag_number(&data[len],&tag);

 if (tag != 0 && tag != 1) // stream access / record access
    {
     *ret_len = len;
     return BACAPI_ERROR_INVALID_TAG;
    }

 file_data->access_type = tag;
 len+=ret;

 // start pos
 len += gDec->decode_tag_number_and_value(&data[len], &tag, &len_val);
 if (tag != BACNET_APPLICATION_TAG_SINT)
    {
     *ret_len = len;
     return BACAPI_ERROR_INVALID_TAG;
    }

 len += gDec->decode_signed(&data[len],len_val,&value);
 file_data->start_pos = value;

 // octet str
 len += gDec->decode_tag_number_and_value(&data[len], &tag, &len_val);
 if (tag != BACNET_APPLICATION_TAG_OSTRING)
    {
     *ret_len = len;
     return BACAPI_ERROR_INVALID_TAG;
    }

 len += gDec->decode_octet_string(&data[len],len_val,&file_data->data);
 file_data->octet_count = (int)file_data->data.vValue.size();


 // closing access tag
 if (!gDec->decode_is_closing_tag_number(&data[len],file_data->access_type))
    {
     *ret_len = len;
     return BACAPI_ERROR_INVALID_TAG;
    }
 len++;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\



BACAPI_ERROR I_cBACServiceHandler::DecodeAtomicWriteFileAck(uint8_t *data, BACNET_FILE *file_data)
{
 int len = 0;
 uint32_t len_val = 0;
 int32_t value = 0;
 uint8_t tag = 0;

 gDec->decode_tag_number_and_value(&data[len], &tag, &len_val);
 if (tag != 0 && tag != 1)
    {
     return BACAPI_ERROR_INVALID_TAG;
    }

 gDec->decode_context_signed(&data[len],tag,&value);
 file_data->access_type = tag;
 file_data->start_pos = value;

 return BACAPI_OK;
}



//=====================================================================================//
//===================[ DECODE: WHO-HAS ]===============================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeWhoHasRequest(uint8_t *apdu, uint32_t *low, uint32_t *hi, BACNET_OBJECT_ID *oid, string *name)
{
 int len =0,ret = 0;
 uint32_t hi_val = 0, low_val = 0;
 uint16_t type = 0;
 uint32_t instance = 0;
 gLog->AppendLog("I_cBACServiceHandler::DecodeWhoHasRequest:\n");

 // context 0 low  - OPT


 ret = gDec->decode_context_unsigned(&apdu[len],0,&low_val);  // postoji li low_limit
 if (ret > 0)
    {
     // postoji to znaci da sljedeci parametar mora biti hi_limit
     len += ret;
     *low = low_val;
     ret = gDec->decode_context_unsigned(&apdu[len],1,&hi_val);
     if (ret < 0)
        {
         // OH-NOES!
         return -1;
        }
     len += ret;
     *hi = hi_val;
    }

 // ako smo dosli ovdje to znaci da mora postojati oid
 ret = gDec->decode_context_object_id(&apdu[len],2,&type,&instance);
 if (ret > 0)
    {
     oid->type = type;
     oid->instance = instance;
     return len;
    }else {
           // ako ne postoji object id onda mora postojati ime
           uint32_t len_val = 0;
           uint8_t tag = 0;

		   BACNET_CHARACTER_STRING txt;
		   ret = gDec->decode_context_character_string(&apdu[len],3,&txt);

		   if (ret < 0)
		      {
		       return -1;
		      }

		   *name = txt.value8;
          }


 return len;
}
//_____________________________________________________________________________________\\








//=====================================================================================//
//===================[ DECODE UNCONFIRMED EVENT NOTIFICATION ]=========================//
// TODO:
// bacapp_decode_timestamp koristi MAX_ASHRAE_PROPERTY_COUNT kao ulazni parametar. Zasto?!
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeUnconfirmedEventNotification(uint8_t *data, BACNET_EVENT_NOTIFICATION *notification, int *ret_len)
{
 int ret = 0;
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0, val = 0;
 uint8_t *pos = NULL;

 // process id: context 0
 ret = gDec->decode_context_unsigned(&data[len],0,&notification->processId);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;


 // device id: ctx 1
 ret = gDec->decode_context_object_id(&data[len],1,&notification->deviceId.type,&notification->deviceId.instance);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;


 // object_id: ctx 2
 ret = gDec->decode_context_object_id(&data[len],2,&notification->objectId.type,&notification->objectId.instance);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;

 gDec->decode_tag_number(&data[len], &tag);
 // time stamp: opening tag [3]
 if (!gDec->decode_is_opening_tag_number(&data[len],3))
    {
     return BACAPI_ERROR_INVALID_OPENING_TAG;
    }
 len++;

 ret = gDec->bacapp_decode_timestamp(&data[len],&notification->timeStamp,BACNET_PROPERTY_COUNT);
 if (ret < 0)
    {
     return BACAPI_ERROR_MALFORMED_PACKET;
    }
 len += ret;

 // time stamp: closing tag [3]
 if (!gDec->decode_is_closing_tag_number(&data[len],3))
    {
     return BACAPI_ERROR_INVALID_CLOSING_TAG;
    }
 len++;

 // notificationclass ctx [4]
 ret = gDec->decode_context_unsigned(&data[len],4,&notification->notificationClass);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;

 // priority ctx [5]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->priority = (uint8_t)val;


 // event type [6]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->eventType = (BACNET_EVENT_TYPE)val;


 // OPTIONAL: message_text [7]
 gDec->decode_tag_number(&data[len],&tag);
 if (tag == 7)
    {
	 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
     ret = gDec->bacapp_decode_data(&data[len],BACNET_APPLICATION_TAG_CSTRING,len_val,&notification->messageText);
	 if (ret < 0)
		{
		 // nepoznati character set, postavi cursor na len + text_len
		 len += len_val;
		}else len += ret;
    }



 // notify type [8]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->notifyType = (BACNET_NOTIFY_TYPE)val;


 // ack req [9]
 gDec->decode_tag_number(&data[len],&tag);
 if (tag == 9)
    {
     len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
     len += gDec->decode_unsigned(&data[len], len_val, &val);
     notification->ackReq = (bool)val;
    }


 pos = &data[len];

 // from state [10]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->fromState = (BACNET_EVENT_STATE)val;

 // from state [11]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->toState = (BACNET_EVENT_STATE)val;

 pos = &data[len];

 // opening ctx [12]
 // notification params
 if (!gDec->decode_is_opening_tag_number(&data[len],12))
     return BACAPI_ERROR_INVALID_OPENING_TAG;

 len++;

 ret = gDec->decode_tag_number(&data[len], &tag);
 notification->eventParams.paramTag = tag;
 len++;
 switch (tag)
        {
		 case EVENT_CHANGE_OF_BITSTRING:
		 {
          ret = gDec->decode_context_bitstring(&data[len],0,&notification->eventParams.notificationParams.changeOfBitString.bitString);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len+=ret;

          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.changeOfBitString.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_BITSTRING))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

          len++;
          break;
		 }

		 case EVENT_CHANGE_OF_STATE:
		 {
		  uint8_t *tmp = &data[len];

		  // event state
		  ret = gDec->bacapp_decode_context_property_state(&data[len],0,&notification->eventParams.notificationParams.changeOfState.newState);

		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

		  len += ret;

          // status flags
          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.changeOfState.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_STATE))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
		  break;
		 }




		 case EVENT_CHANGE_OF_VALUE:
		 {
		  // start of change of value [0]
		  if (!gDec->decode_is_opening_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_OPENING_TAG;

		  len++;

          gDec->decode_tag_number(&data[len],&tag);
          notification->eventParams.notificationParams.changeOfValue.tag = (CHANGE_OF_VALUE_TYPE)tag;

          switch (notification->eventParams.notificationParams.changeOfValue.tag)
                 {
                  case 0: // bit_string
                   ret = gDec->decode_context_bitstring(&data[len],0,&notification->eventParams.notificationParams.changeOfValue.newValue.changedBits);
                   if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
                   break;

                  case 1: // real value
				   ret = gDec->decode_context_real(&data[len],1,&notification->eventParams.notificationParams.changeOfValue.newValue.changeValue);
				   if (ret < 0 )return BACAPI_ERROR_INVALID_PARAMETER;
                   break;

                  default: return BACAPI_ERROR_INVALID_TAG;
                 }
          // end change of value [0]
          len += ret;
		  if (!gDec->decode_is_closing_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

          len++;

          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.changeOfValue.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_VALUE))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
          break;
		 }


		 case EVENT_COMMAND_FAILURE:
		 return BACAPI_ERROR_UNSUPPORTED_EVENT_PARAMETER;
		 break;

		 case EVENT_FLOATING_LIMIT:
		 {

          // reference_value - real
		  ret = gDec->decode_context_real(&data[len],0,&notification->eventParams.notificationParams.floatingLimit.referenceValue);
		  if (ret < 0)return BACAPI_ERROR_INVALID_PARAMETER;


		  len += ret;

          // status flags
          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.floatingLimit.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          // setpoint_val - real
		  ret = gDec->decode_context_real(&data[len],2,&notification->eventParams.notificationParams.floatingLimit.setPointValue);
		  if (ret < 0)return BACAPI_ERROR_INVALID_PARAMETER;


		  len += ret;

          // error limit - real
		  ret = gDec->decode_context_real(&data[len],3,&notification->eventParams.notificationParams.floatingLimit.errorLimit);
		  if (ret < 0)return BACAPI_ERROR_INVALID_PARAMETER;


		  len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_FLOATING_LIMIT))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;

		  break;
		 }


		 case EVENT_OUT_OF_RANGE:
		 {

		  ret = gDec->decode_context_real(&data[len],0,&notification->eventParams.notificationParams.outOfRange.exceedingValue);
		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

		  len += ret;

          notification->eventParams.notificationParams.outOfRange.statusFlags = new BACNET_BIT_STRING;
		  ret = gDec->decode_context_bitstring(&data[len],1,notification->eventParams.notificationParams.outOfRange.statusFlags);
		  if (ret < 0)
		     {
		      delete notification->eventParams.notificationParams.outOfRange.statusFlags;
		      return BACAPI_ERROR_INVALID_PARAMETER;
		     }

		  len += ret;


		  ret = gDec->decode_context_real(&data[len],2,&notification->eventParams.notificationParams.outOfRange.deadband);
		  if (ret < 0)
		     {
		      delete notification->eventParams.notificationParams.outOfRange.statusFlags;
		      return BACAPI_ERROR_INVALID_PARAMETER;
		     }
		  len += ret;

		  ret = gDec->decode_context_real(&data[len],3,&notification->eventParams.notificationParams.outOfRange.exceededLimit);
		  if (ret < 0)
		     {
		      delete notification->eventParams.notificationParams.outOfRange.statusFlags;
		      return BACAPI_ERROR_INVALID_PARAMETER;
		     }

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_OUT_OF_RANGE))
             {
              return BACAPI_ERROR_INVALID_CLOSING_TAG;
             }
          len++;
          break;
		 }

		 case EVENT_COMPLEX_EVENT_TYPE:
		 {
		  return BACAPI_ERROR_UNSUPPORTED_EVENT_PARAMETER;
		  break;
		 }


		 case EVENT_CHANGE_OF_LIFE_SAFETY:
		 {

		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  if (tag != 0) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  notification->eventParams.notificationParams.changeOfLifeSafety.newState = (BACNET_LIFE_SAFETY_STATE)val;

		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  if (tag != 1) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  notification->eventParams.notificationParams.changeOfLifeSafety.newMode = (BACNET_LIFE_SAFETY_MODE)val;


          // statusFlags
          uint8_t *pos = &data[len];
		  ret = gDec->decode_context_bitstring(&data[len],2,&notification->eventParams.notificationParams.changeOfLifeSafety.statusFlags);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  if (tag != 3) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  notification->eventParams.notificationParams.changeOfLifeSafety.operationExpected = (BACNET_LIFE_SAFETY_OPERATION)val;


          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_LIFE_SAFETY))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
		  break;
		 }


		 case EVENT_EXTENDED:
		 {
          // vendorId
          ret = gDec->decode_context_unsigned(&data[len],0,&notification->eventParams.notificationParams.extendedEvent.vendorId);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;
          len += ret;

          // extendedEventType
		  ret = gDec->decode_context_unsigned(&data[len],1,&notification->eventParams.notificationParams.extendedEvent.extendedEventType);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          // TODO: parameters
          // {
              // preskoci parametere za sada
          // }
          return BACAPI_OK;
		  break;
		 }

		 case EVENT_BUFFER_READY:
		 {

		  // start of DEVICE_OBJECT_PROPERTY_REFERENCE opening [0]
		  if (!gDec->decode_is_opening_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_OPENING_TAG;

		  len++;


		  // object id ref: ctx [0]
		  ret = gDec->decode_context_object_id(&data[len],0,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.objectIdentifier.type,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.objectIdentifier.instance);

		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += ret;

		  // property ctx [1]
		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  if (tag != 1)
		     return BACAPI_ERROR_INVALID_PARAMETER;

		  notification->eventParams.notificationParams.bufferReady.bufferProperty.propertyIdentifier = (BACNET_PROPERTY_ID)val;

		  // array index [2] ??
          gDec->decode_tag_number(&data[len], &tag);
          if (tag == 2)  // postoji array index
             {
              len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
              len += gDec->decode_unsigned(&data[len], len_val, &val);
              notification->eventParams.notificationParams.bufferReady.bufferProperty.arrayIndex = val;
             }else notification->eventParams.notificationParams.bufferReady.bufferProperty.arrayIndex = -1;


 		  // device id: ctx [3]
 		  uint8_t *pos = &data[len];

		  ret = gDec->decode_context_object_id(&data[len],3,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.deviceIndentifier.type,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.deviceIndentifier.instance);
		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += ret;

          // end of DEVICE_OBJECT_PROPERTY_REFERENCE [0]
		  if (!gDec->decode_is_closing_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;

          // previous notif
          len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
          len += gDec->decode_unsigned(&data[len], len_val, &val);
          notification->eventParams.notificationParams.bufferReady.previousNotification = val;

          // current notif
          len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
          len += gDec->decode_unsigned(&data[len], len_val, &val);
          notification->eventParams.notificationParams.bufferReady.currentNotification= val;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_BUFFER_READY))
             {
              return BACAPI_ERROR_INVALID_CLOSING_TAG;
             }
          len++;
          break;
		 }

		 case EVENT_UNSIGNED_RANGE:
		 {
          // exceedingValue
          ret = gDec->decode_context_unsigned(&data[len],0,&notification->eventParams.notificationParams.unsignedRange.exceedingValue);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;
          len += ret;

          // statusFlags
		  ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.unsignedRange.statusFlags);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          // exceededLimit
          ret = gDec->decode_context_unsigned(&data[len],2,&notification->eventParams.notificationParams.unsignedRange.exceededLimit);
          if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_UNSIGNED_RANGE))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
		  break;
		 }


		 default:
		 return BACAPI_ERROR_UNSUPPORTED_EVENT_PARAMETER;
		 break;
        }


 // closing ctx [12]
 // notification params
 if (!gDec->decode_is_closing_tag_number(&data[len],12))
    {
     return BACAPI_ERROR_INVALID_CLOSING_TAG;
    }
 len++;

 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ DECODE UNCONFIRMED EVENT NOTIFICATION ]=========================//
// TODO:
// bacapp_decode_timestamp koristi MAX_ASHRAE_PROPERTY_COUNT kao ulazni parametar. Zasto?
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeConfirmedEventNotification(uint8_t *data, BACNET_EVENT_NOTIFICATION *notification, int *ret_len)
{
 int ret = 0;
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0, val = 0;
 uint8_t *pos = NULL;

 // process id: context 0
 ret = gDec->decode_context_unsigned(&data[len],0,&notification->processId);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;


 // device id: ctx 1
 ret = gDec->decode_context_object_id(&data[len],1,&notification->deviceId.type,&notification->deviceId.instance);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;


 // object_id: ctx 2
 ret = gDec->decode_context_object_id(&data[len],2,&notification->objectId.type,&notification->objectId.instance);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;

 gDec->decode_tag_number(&data[len], &tag);
 // time stamp: opening tag [3]
 if (!gDec->decode_is_opening_tag_number(&data[len],3))
    {
     return BACAPI_ERROR_INVALID_OPENING_TAG;
    }
 len++;

 ret = gDec->bacapp_decode_timestamp(&data[len],&notification->timeStamp,BACNET_PROPERTY_COUNT);
 if (ret < 0)
    {
     return BACAPI_ERROR_MALFORMED_PACKET;
    }
 len += ret;

 // time stamp: closing tag [3]
 if (!gDec->decode_is_closing_tag_number(&data[len],3))
    {
     return BACAPI_ERROR_INVALID_CLOSING_TAG;
    }
 len++;

 // notificationclass ctx [4]
 ret = gDec->decode_context_unsigned(&data[len],4,&notification->notificationClass);
 if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
 len += ret;

 // priority ctx [5]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->priority = (uint8_t)val;


 // event type [6]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->eventType = (BACNET_EVENT_TYPE)val;


 // OPTIONAL: message_text [7]
 gDec->decode_tag_number(&data[len],&tag);
 if (tag == 7)
    {
	 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
     ret = gDec->bacapp_decode_data(&data[len],BACNET_APPLICATION_TAG_CSTRING,len_val,&notification->messageText);
	 if (ret < 0)
		{
		 // nepoznati character set, postavi cursor na len + text_len
		 len += len_val;
		}else len += ret;
    }



 // notify type [8]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->notifyType = (BACNET_NOTIFY_TYPE)val;


 // ack req [9]
 gDec->decode_tag_number(&data[len],&tag);
 if (tag == 9)
    {
     len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
     len += gDec->decode_unsigned(&data[len], len_val, &val);
     notification->ackReq = (bool)val;
    }

 pos = &data[len];

 // from state [10]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->fromState = (BACNET_EVENT_STATE)val;

 // from state [11]
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
 len += gDec->decode_unsigned(&data[len], len_val, &val);
 notification->toState = (BACNET_EVENT_STATE)val;

 pos = &data[len];

 // opening ctx [12]
 // notification params
 if (!gDec->decode_is_opening_tag_number(&data[len],12))
     return BACAPI_ERROR_INVALID_OPENING_TAG;

 len++;

 ret = gDec->decode_tag_number(&data[len], &tag);
 notification->eventParams.paramTag = tag;
 len++;
 switch (tag)
        {
		 case EVENT_CHANGE_OF_BITSTRING:
		 {
          ret = gDec->decode_context_bitstring(&data[len],0,&notification->eventParams.notificationParams.changeOfBitString.bitString);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len+=ret;

          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.changeOfBitString.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_BITSTRING))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

          len++;
          break;
		 }

		 case EVENT_CHANGE_OF_STATE:
		 {
		  uint8_t *tmp = &data[len];

		  // event state
		  ret = gDec->bacapp_decode_context_property_state(&data[len],0,&notification->eventParams.notificationParams.changeOfState.newState);

		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

		  len += ret;

          // status flags
          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.changeOfState.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_STATE))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
		  break;
		 }




		 case EVENT_CHANGE_OF_VALUE:
		 {
		  // start of change of value [0]
		  if (!gDec->decode_is_opening_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_OPENING_TAG;

		  len++;

          gDec->decode_tag_number(&data[len],&tag);
          notification->eventParams.notificationParams.changeOfValue.tag = (CHANGE_OF_VALUE_TYPE)tag;

          switch (notification->eventParams.notificationParams.changeOfValue.tag)
                 {
                  case 0: // bit_string
                   ret = gDec->decode_context_bitstring(&data[len],0,&notification->eventParams.notificationParams.changeOfValue.newValue.changedBits);
                   if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
                   break;

                  case 1: // real value
				   ret = gDec->decode_context_real(&data[len],1,&notification->eventParams.notificationParams.changeOfValue.newValue.changeValue);
				   if (ret < 0 )return BACAPI_ERROR_INVALID_PARAMETER;
                   break;

                  default: return BACAPI_ERROR_INVALID_TAG;
                 }
          // end change of value [0]
          len += ret;
		  if (!gDec->decode_is_closing_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

          len++;

          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.changeOfValue.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_VALUE))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
          break;
		 }


		 case EVENT_COMMAND_FAILURE:
		 return BACAPI_ERROR_UNSUPPORTED_EVENT_PARAMETER;
		 break;

		 case EVENT_FLOATING_LIMIT:
		 {

          // reference_value - real
		  ret = gDec->decode_context_real(&data[len],0,&notification->eventParams.notificationParams.floatingLimit.referenceValue);
		  if (ret < 0)return BACAPI_ERROR_INVALID_PARAMETER;


		  len += ret;

          // status flags
          ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.floatingLimit.statusFlags);
          if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          // setpoint_val - real
		  ret = gDec->decode_context_real(&data[len],2,&notification->eventParams.notificationParams.floatingLimit.setPointValue);
		  if (ret < 0)return BACAPI_ERROR_INVALID_PARAMETER;


		  len += ret;

          // error limit - real
		  ret = gDec->decode_context_real(&data[len],3,&notification->eventParams.notificationParams.floatingLimit.errorLimit);
		  if (ret < 0)return BACAPI_ERROR_INVALID_PARAMETER;


		  len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_FLOATING_LIMIT))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;

		  break;
		 }


		 case EVENT_OUT_OF_RANGE:
		 {

		  ret = gDec->decode_context_real(&data[len],0,&notification->eventParams.notificationParams.outOfRange.exceedingValue);
		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;

		  len += ret;

          notification->eventParams.notificationParams.outOfRange.statusFlags = new BACNET_BIT_STRING;
		  ret = gDec->decode_context_bitstring(&data[len],1,notification->eventParams.notificationParams.outOfRange.statusFlags);
		  if (ret < 0)
		     {
		      delete notification->eventParams.notificationParams.outOfRange.statusFlags;
		      return BACAPI_ERROR_INVALID_PARAMETER;
		     }

		  len += ret;


		  ret = gDec->decode_context_real(&data[len],2,&notification->eventParams.notificationParams.outOfRange.deadband);
		  if (ret < 0)
		     {
		      delete notification->eventParams.notificationParams.outOfRange.statusFlags;
		      return BACAPI_ERROR_INVALID_PARAMETER;
		     }
		  len += ret;

		  ret = gDec->decode_context_real(&data[len],3,&notification->eventParams.notificationParams.outOfRange.exceededLimit);
		  if (ret < 0)
		     {
		      delete notification->eventParams.notificationParams.outOfRange.statusFlags;
		      return BACAPI_ERROR_INVALID_PARAMETER;
		     }

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_OUT_OF_RANGE))
             {
              return BACAPI_ERROR_INVALID_CLOSING_TAG;
             }
          len++;
          break;
		 }

		 case EVENT_COMPLEX_EVENT_TYPE:
		 {
		  return BACAPI_ERROR_UNSUPPORTED_EVENT_PARAMETER;
		  break;
		 }


		 case EVENT_CHANGE_OF_LIFE_SAFETY:
		 {

		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  if (tag != 0) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  notification->eventParams.notificationParams.changeOfLifeSafety.newState = (BACNET_LIFE_SAFETY_STATE)val;

		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  if (tag != 1) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  notification->eventParams.notificationParams.changeOfLifeSafety.newMode = (BACNET_LIFE_SAFETY_MODE)val;


          // statusFlags
          uint8_t *pos = &data[len];
		  ret = gDec->decode_context_bitstring(&data[len],2,&notification->eventParams.notificationParams.changeOfLifeSafety.statusFlags);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  if (tag != 3) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  notification->eventParams.notificationParams.changeOfLifeSafety.operationExpected = (BACNET_LIFE_SAFETY_OPERATION)val;


          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_CHANGE_OF_LIFE_SAFETY))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
		  break;
		 }


		 case EVENT_EXTENDED:
		 {
          // vendorId
          ret = gDec->decode_context_unsigned(&data[len],0,&notification->eventParams.notificationParams.extendedEvent.vendorId);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;
          len += ret;

          // extendedEventType
		  ret = gDec->decode_context_unsigned(&data[len],1,&notification->eventParams.notificationParams.extendedEvent.extendedEventType);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          // TODO: parameters
          // {
              // preskoci parametere za sada
          // }
          return BACAPI_OK;
		  break;
		 }

		 case EVENT_BUFFER_READY:
		 {

		  // start of DEVICE_OBJECT_PROPERTY_REFERENCE opening [0]
		  if (!gDec->decode_is_opening_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_OPENING_TAG;

		  len++;


		  // object id ref: ctx [0]
		  ret = gDec->decode_context_object_id(&data[len],0,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.objectIdentifier.type,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.objectIdentifier.instance);

		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += ret;

		  // property ctx [1]
		  len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
		  len += gDec->decode_unsigned(&data[len], len_val, &val);
		  if (tag != 1)
		     return BACAPI_ERROR_INVALID_PARAMETER;

		  notification->eventParams.notificationParams.bufferReady.bufferProperty.propertyIdentifier = (BACNET_PROPERTY_ID)val;

		  // array index [2] ??
          gDec->decode_tag_number(&data[len], &tag);
          if (tag == 2)  // postoji array index
             {
              len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
              len += gDec->decode_unsigned(&data[len], len_val, &val);
              notification->eventParams.notificationParams.bufferReady.bufferProperty.arrayIndex = val;
             }else notification->eventParams.notificationParams.bufferReady.bufferProperty.arrayIndex = -1;


 		  // device id: ctx [3]
 		  uint8_t *pos = &data[len];

		  ret = gDec->decode_context_object_id(&data[len],3,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.deviceIndentifier.type,
		  &notification->eventParams.notificationParams.bufferReady.bufferProperty.deviceIndentifier.instance);
		  if (ret < 0) return BACAPI_ERROR_INVALID_PARAMETER;
		  len += ret;

          // end of DEVICE_OBJECT_PROPERTY_REFERENCE [0]
		  if (!gDec->decode_is_closing_tag_number(&data[len],0))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;

          // previous notif
          len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
          len += gDec->decode_unsigned(&data[len], len_val, &val);
          notification->eventParams.notificationParams.bufferReady.previousNotification = val;

          // current notif
          len += gDec->decode_tag_number_and_value(&data[len],&tag,&len_val);
          len += gDec->decode_unsigned(&data[len], len_val, &val);
          notification->eventParams.notificationParams.bufferReady.currentNotification= val;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_BUFFER_READY))
             {
              return BACAPI_ERROR_INVALID_CLOSING_TAG;
             }
          len++;
          break;
		 }

		 case EVENT_UNSIGNED_RANGE:
		 {
          // exceedingValue
          ret = gDec->decode_context_unsigned(&data[len],0,&notification->eventParams.notificationParams.unsignedRange.exceedingValue);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;
          len += ret;

          // statusFlags
		  ret = gDec->decode_context_bitstring(&data[len],1,&notification->eventParams.notificationParams.unsignedRange.statusFlags);
		  if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          // exceededLimit
          ret = gDec->decode_context_unsigned(&data[len],2,&notification->eventParams.notificationParams.unsignedRange.exceededLimit);
          if (ret < 0)
		      return BACAPI_ERROR_INVALID_PARAMETER;

          len += ret;

          if (!gDec->decode_is_closing_tag_number(&data[len],EVENT_UNSIGNED_RANGE))
		      return BACAPI_ERROR_INVALID_CLOSING_TAG;

		  len++;
		  break;
		 }


		 default:
		 return BACAPI_ERROR_UNSUPPORTED_EVENT_PARAMETER;
		 break;
        }


 // closing ctx [12]
 // notification params
 if (!gDec->decode_is_closing_tag_number(&data[len],12))
    {
     return BACAPI_ERROR_INVALID_CLOSING_TAG;
    }
 len++;

 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ DECODE WHO IS REQUEST ]=========================================//
// SOURCE: Server
// Ako nadjemo gresku u context tagovima svejedno
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeWhoIsRequest(uint8_t *apdu, BACNET_DEVICE_RANGE *range, uint16_t *ret_len)
{
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0;
 uint32_t val = 0;

 // from
 if (gDec->decode_is_context_tag(&apdu[len], 0))
    {
     len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
     len += gDec->decode_unsigned(&apdu[len], len_val, &val);
     range->from = val;
    }else return BACAPI_ERROR_MALFORMED_PACKET;

 // to
  if (gDec->decode_is_context_tag(&apdu[len], 1))
    {
     len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
     len += gDec->decode_unsigned(&apdu[len], len_val, &val);
     range->to = val;
    }else return BACAPI_ERROR_MALFORMED_PACKET;

 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ DECODE DELETE OBJECT REQUEST ]==================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeDeleteObjectRequest(uint8_t *apdu,BACNET_OBJECT_ID *oid)
{
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0;

 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);

 BACNET_APPLICATION_DATA_VALUE val;
 val.application_tag = (BACNET_APPLICATION_TAG)tag;

 int ret = gDec->bacapp_decode_data(&apdu[len],tag,len_val,&val);
 if (ret < 0)
    {
     return -1;
    }

 oid->type = val.type.Object_Id.type;
 oid->instance = val.type.Object_Id.instance;

 return len;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ DECODE CREATE OBJECT REQUEST ]==================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeCreateObjectRequest(uint8_t *apdu,BACNET_OBJECT_ID *oid)
{
 int len = 0;
 uint32_t val = 0;

 if (!gDec->decode_is_context_tag(&apdu[len], 0))
    {
     return -1;
    }
 len++;

 // create object moze doci ili samo object type ili cijeli oid
 if (gDec->decode_is_context_tag(&apdu[len], 0))
    {
     // dosao je samo object type
     len += gDec->decode_context_unsigned(&apdu[len],0,&val);
     oid->instance = (~0);
     oid->type = (uint16_t)val;
    }else {
           // dosao je object type
           len += gDec->decode_context_object_id(&apdu[len],1,&oid->type,&oid->instance);
          }

 // zatvarajuci tag [0] - preskoci
 len++;

 // TODO:
 // ovdje dolazi lista vrijednosti ali trenutno ne dekodiramo to
 // [1E]
 // Value 1
 // ...
 // [1F]

 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ DECODE READ PROPERTY REQUEST ]==================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeReadPropertyRequest(uint8_t *apdu, int apduLen, BACNET_OBJECT_PROPERTY_REFERENCE *ref)
{
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0;
 uint32_t val = 0;

 if (!gDec->decode_is_context_tag(&apdu[len++], 0))
    {
     return -1;  // nije oid
    }

 len += gDec->decode_object_id(&apdu[len], &ref->oid.type, &ref->oid.instance);

 // ako smo dosli do kraja paketa onda nam fali property parametar
 if (len >= apduLen)
    {
     // BTL 135.1: 13.4.4
     return BACAPI_ERROR_MISSING_REQUIRED_PARAMETER;
    }

 // dekodiraj tag i vrijednost
 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
 if (tag != 1)
    {
     // BTL 135.1: 13.4.3
     return BACAPI_ERROR_INVALID_TAG;
    }

 len += gDec->decode_enumerated(&apdu[len], len_val, &val);
 ref->propertyId = (BACNET_PROPERTY_ID)val;


 if (len >=apduLen)  // nema potrebe nastaviti ako je ovdje kraj
    {
     ref->propertyArrayIndex = BACNET_ARRAY_ALL;
     return len;
    }

 // ovdje bi trebalo provjeriti velicinu da ne izadjemo iz lociranog polja
 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
 if (tag != 2)
    {
     ref->propertyArrayIndex = BACNET_ARRAY_ALL;
     return 0;  // gotov
    }

 // array index
 len += gDec->decode_unsigned(&apdu[len], len_val, &ref->propertyArrayIndex);


 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ DECODE READ PROPERTY MULTIPLE REQUEST ]=========================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeReadPropertyMultipleRequest(uint8_t *apdu, int apduLen, BACNET_OBJECT_PROPERTY_REFERENCE *ref)
{
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0;
 uint32_t val = 0;


 if (!gDec->decode_is_context_tag(&apdu[len++], 0))
    {
     return -1;  // nije oid
    }

 len += gDec->decode_object_id(&apdu[len], &ref->oid.type, &ref->oid.instance);

 if (!gDec->decode_is_opening_tag_number(&apdu[len++],1))
    {
     return -1;
    }

 // prazan?
 if (gDec->decode_is_closing_tag_number(&apdu[len],1))
    {
     return len;
    }



 // list of property references
 uint8_t *pos = &apdu[len];
 /* Tag 1: Property ID */
 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
 if (tag != 0)
    {
     return -1;  // nije property
    }
 len += gDec->decode_enumerated(&apdu[len], len_val, &val);
 ref->propertyId = (BACNET_PROPERTY_ID)val;


 if (gDec->decode_is_closing_tag_number(&apdu[len++],1))
    {
     return len;
    }


 return len;
 //end
}
//_____________________________________________________________________________________\\






//=====================================================================================//
//===================[ DECODE WRITE PROPERTY REQUEST ]=================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeWritePropertyRequest(uint8_t *apdu, int apduLen, BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *prop)
{
 int ret = 0;
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0;
 uint32_t val = 0;

 // WriteProp context  [0]
 if (!gDec->decode_is_context_tag(&apdu[len++], 0))
    {
     return -1;  // nije oid
    }

 // oid
 len += gDec->decode_object_id(&apdu[len], &object->oid.type, &object->oid.instance);


 // PROPERTY tag [1]
 len += gDec->decode_tag_number_and_value(&apdu[len], &tag, &len_val);
 if (tag != 1)
    {
     return -1;  // nije property
    }

 len += gDec->decode_enumerated(&apdu[len], len_val, &val);
 prop->property = (BACNET_PROPERTY_ID)val;


 // ARRAY INDEX [2]
 uint8_t *tmp = &apdu[len];
 ret = gDec->decode_is_context_tag(&apdu[len],2);
 if (ret)
    {
     len += gDec->decode_context_unsigned(&apdu[len],2,&val);
     prop->index = (uint16_t)val;
    }else prop->index = BACNET_ARRAY_ALL;



 bool complex = BACAPI_IsComplexProperty(prop->property);
 if (complex)
    {
     len += DecodeComplexPropertyValue(&apdu[len],prop);
	}else {
	       // dekodiraj vrijednost ovog propertyja
	       int ret_len = 0;
           BACAPI_ERROR ret_err = DecodePrimitiveValue(&apdu[len], apduLen, prop, &ret_len);
           if (ret_err != BACAPI_OK)
              {
               // TODO: return ret_err;
               return -1;
              }
           len += ret_len;
	      }



 return len;
}
//_____________________________________________________________________________________\\


//=====================================================================================//
//===================[ DECODE:I AM ]===================================================//
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeIAmRequest(uint8_t *apdu, BACNET_DEVICE *device, int *ret_len)
{
    int len = 0;
    int apdu_len = 0;   /* total length of the apdu, return value */
    uint16_t object_type = 0;   /* should be a Device Object */
    uint32_t object_instance = 0;
    uint8_t tag_number = 0;
    uint32_t len_value = 0;
    uint32_t decoded_value = 0;

    /* OBJECT ID - object id */
    len = gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
    if (tag_number != BACNET_APPLICATION_TAG_OID)
       {
        // ovdje mora doci Application tag za object id. Paket je krivo kodiran

        return BACAPI_ERROR_MALFORMED_PACKET;
       }

    len += gDec->decode_object_id(&apdu[len], &object_type, &object_instance);
    if (object_type != OBJECT_DEVICE)
       {
        // ovaj tip mora biti OBJECT_DEVICE, paket je krivo kodiran
        return BACAPI_ERROR_MALFORMED_PACKET;
       }


    device->oid.type = (BACNET_OBJECT_TYPE)object_type;
    device->oid.instance = object_instance;

    len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
    if (tag_number != BACNET_APPLICATION_TAG_UINT)
       {
        // ovaj app tag mora biti UINT to je max apdu accepted.
        return BACAPI_ERROR_MALFORMED_PACKET;
       }

    len += gDec->decode_unsigned(&apdu[len], len_value, &decoded_value);
    device->maximumAPDU = (unsigned) decoded_value;

    len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
    if (tag_number != BACNET_APPLICATION_TAG_ENUM)
       {
        return BACAPI_ERROR_MALFORMED_PACKET;
       }

    len += gDec->decode_enumerated(&apdu[len], len_value, &decoded_value);
    device->segmentation = (BACNET_SEGMENTATION) decoded_value;

    ///* Vendor ID - unsigned16 */
    len += gDec->decode_tag_number_and_value(&apdu[len], &tag_number, &len_value);
    if (tag_number != BACNET_APPLICATION_TAG_UINT)
       {
        return BACAPI_ERROR_MALFORMED_PACKET;
       }

    len += gDec->decode_unsigned(&apdu[len], len_value, &decoded_value);
    if (decoded_value > 0xFFFF)
        return BACAPI_ERROR_INVALID_PARAMETER;

    device->vendorId = (uint16_t) decoded_value;

 *ret_len = len;

 return BACAPI_OK;
}
//_____________________________________________________________________________________\\







//
//
//
////=====================================================================================//
////===================[ DECODE: COMPLEX RP ACK ]========================================//
//// TODO: uvjeti koji obj koji prop i dekodiraj u odnosu na to
////=====================================================================================//
//BACAPI_ERROR I_cBACServiceHandler::DecodeReadPropertyComplexAck(uint8_t *data, int dataLen, BACNET_OBJECT_PROPERTY *property,BACNET_OBJECT_ID *oid)
//{
// int len = 0;
// int ret = 0;
// uint16_t objectType = 0;
// uint32_t objectInstance = 0;
// uint8_t tagVal = 0;
// uint32_t lenVal = 0;
// uint32_t propertyVal = 0;
// int arrayIndex = 0;
// uint32_t val = 0;
//
// // decode BACnetObjectID [0]
// ret = gDec->decode_context_object_id(&data[0],0,&objectType, &objectInstance);
// if (ret < 0)
//    {
//     BACAPI_Message_InvalidTag
//     return -1;
//    }
//
// len+=ret;
// oid->type = objectType;
// oid->instance = objectInstance;
//
// // decode BACnetPropertyId [1]
// ret = gDec->decode_context_enumerated(&data[len],1,&propertyVal);
// if (ret < 0)
//    {
//     return -1;
//    }
//
// len += ret;
// property->property = (BACNET_PROPERTY_ID)propertyVal;
//
// // Optional Array Index  [2]
// ret = gDec->decode_context_unsigned(&data[len],2,&val);
// if (ret > 0)
//    {
//     len += ret;
//     property->index = (uint16_t)val;
//    }else property->index = BACNET_ARRAY_ALL;
//
// // Ovdje moramo handlati poseben propertije koji
// //imaju isti property ali razlicitu strukturu
// ret = HandleSpecialProperty(&data[len],dataLen,oid,property,3);
// if (ret < 0)
//    {
//     // ovo nije poseban property, nastavljamo s normalnim dekodiranjem
//
//	 // na ovom mjestu se provjeravaju kompleksni propertiji
//	 // da li je ovaj property complex property?
//	 bool complex = BACAPI_IsComplexProperty(property->property);
//	 // provjeri i dekodiraj complex property
//	 if (complex)
//		{
//		 len += DecodeComplexPropertyValue(&data[len],property);
//		}else {
//			   // dekodiraj vrijednost ovog propertyja
//			   int appDataLen = dataLen - len -1;
//			   int retlen = 0;
//
//			   retlen = DecodePrimitiveValueRP(&data[len], appDataLen, property);
//			   if (retlen < 0)
//				  {
//				   // fuck!
//				   return -1;
//				  }
//			   len += retlen;
//			  }
//    }else len += ret;
//
//
// return len;
//}
////_____________________________________________________________________________________\\
//



//=====================================================================================//
//===================[ DECODE: COMPLEX RP ACK ]========================================//
// TODO: uvjeti koji obj koji prop i dekodiraj u odnosu na to
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeReadPropertyComplexAck(uint8_t *data, int dataLen, BACNET_OBJECT_PROPERTY *property,BACNET_OBJECT_ID *oid, int *ret_len)
{
 int len = 0;
 int ret = 0;
 uint16_t objectType = 0;
 uint32_t objectInstance = 0;
 uint8_t tagVal = 0;
 uint32_t lenVal = 0;
 uint32_t propertyVal = 0;
 int arrayIndex = 0;
 uint32_t val = 0;
 BACAPI_ERROR ret_err = BACAPI_OK;
 

 // decode BACnetObjectID [0]
 ret = gDec->decode_context_object_id(&data[0],0,&objectType, &objectInstance);
 if (ret < 0)
    {
     *ret_len = 0;
     return BACAPI_ERROR_CONTEXT_OBJECT_ID;
    }

 len+=ret;
 oid->type = objectType;
 oid->instance = objectInstance;

 // decode BACnetPropertyId [1]
 ret = gDec->decode_context_enumerated(&data[len],1,&propertyVal);
 if (ret < 0)
    {
     *ret_len = len;
     return BACAPI_ERROR_CONTEXT_ENUMERATED;
    }

 len += ret;
 property->property = (BACNET_PROPERTY_ID)propertyVal;


 if (property->property == PROP_PROPERTY_LIST)
    {
     printf("");
    }


 // Optional Array Index  [2]
 ret = gDec->decode_context_unsigned(&data[len],2,&val);
 if (ret > 0)
    {
     len += ret;
     property->index = (uint16_t)val;
    }else property->index = BACNET_ARRAY_ALL;




 // Ovdje moramo handlati posebne propertije koji
 //imaju isti property ali razlicitu strukturu
 // TODO: BACAPI_IsSpecialProperty();
 bool special = BACAPI_IsSpecialProperty((BACNET_OBJECT_TYPE)oid->type,property->property);
 if (special)
	{
	 int rlen = 0;
	 //ret = DecodeSpecialProperty(&apdu[len],dataLen,&object->oid,property);
	 BACAPI_ERROR ret_err = DecodeSpecialProperty(&data[len],dataLen,oid,property,&rlen);
	 if (ret_err != BACAPI_OK)
		{
		 *ret_len = len;
		 return ret_err;
		}
	 len += rlen;
    } else {
			// ovo nije poseban property, nastavljamo s normalnim dekodiranjem
			// na ovom mjestu se provjeravaju kompleksni propertiji
			bool complex = BACAPI_IsComplexProperty(property->property);
			// provjeri i dekodiraj complex property
			if (complex)
			   {
			    gLog->AppendLog("DecodeReadPropertyComplexAck: Dekodiram Complex Property: (%s - %d)",BACAPI_BACnetPropertyName(property->property),property->property);
				len += DecodeComplexPropertyValue(&data[len],property);
			   }else if (BACAPI_IsPropriateryProperty(property->property))
						{
						 gLog->AppendLog("DecodeReadPropertyComplexAck: Property is Propriatery Property (%d)",property->property);
						 int ret = 0;
						 BACAPI_ERROR ret_err = DecodePropriateryValue(&data[len],dataLen, property, &ret);
						 if (ret_err != BACAPI_OK)
							{
							 *ret_len = len;
							 return ret_err;
							}
						 len += ret;
						}else {
								// dekodiraj vrijednost ovog propertyja
								int retlen = 0;

								// NOTE: dataLen je duzina cijelog apdu djela, ovo treba biti dataLen - len
								gLog->AppendLog("DecodeReadPropertyComplexAck: Dekodiram Primitive Property: (%s - %d)",BACAPI_BACnetPropertyName(property->property),property->property);
								ret_err = DecodePrimitiveValue(&data[len], dataLen, property, &retlen);
								if (ret_err != BACAPI_OK)
								   {
									return ret_err;
								   }

								len += retlen;
							   }
		    }


 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ DECODE: COMPLEX RP MULTIPLE ]===================================//
// NOTE: ova funkcija se koristi samo kod RPM - PROP_ALL za sada.
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeReadPropertyMultipleComplexAck(uint8_t *apdu, int dataLen, BACNET_OBJECT *object, int *ret_len)
{
 int len = 0;
 uint16_t type = 0;
 uint32_t instance = 0;
 uint8_t tagVal = 0;
 uint32_t lenVal = 0;
 uint32_t propertyVal = 0;
 int arrayIndex = 0;
 uint8_t *ptrData = NULL;
 //int ret_len = 0;
 int ret = 0;
 uint32_t val = 0;
 bool complex = false;
 bool propriatery = false;



 // decode BACnetObjectID [0] - context
 ret = gDec->decode_context_object_id(&apdu[0],0,&type, &instance);
 if (ret < 0)
    {
     *ret_len = 0;
     return BACAPI_ERROR_CONTEXT_OBJECT_ID;
    }


 object->oid.instance = instance;
 object->oid.type = type;
 len += ret;

 gLog->AppendLog("DecodeReadPropertyMultipleComplexAck: Object (%s, %d)",BACAPI_BACnetObjectName(object->oid.type),object->oid.instance);

 // ListOfResults  [1]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],1))
    {
     *ret_len = len;
     return BACAPI_ERROR_INVALID_OPENING_TAG;
    }

 len++;


 // za svaki property u rezultatu
 while (gDec->decode_is_closing_tag_number(&apdu[len],1) != true || len > dataLen)
       {
        uint8_t *tmp = &apdu[len];

        // decode BACnetPropertyId [1] - context
        ret = gDec->decode_context_enumerated(&apdu[len],2,&propertyVal);
        if (ret < 0)
           {
            *ret_len = len;
            return BACAPI_ERROR_CONTEXT_ENUMERATED;
           }
        len += ret;

        BACNET_OBJECT_PROPERTY *property = BACAPI_CreateObjectProperty();
        property->property = (BACNET_PROPERTY_ID)propertyVal;
        
        int s = property->vValueList.size();

        // Optional Array Index  [3]
		ret = gDec->decode_context_unsigned(&apdu[len],3,&lenVal);
		if (ret > 0)
		   {
			len += ret;
			property->index = (uint16_t)lenVal;
		   }else property->index = BACNET_ARRAY_ALL;


        gLog->AppendLog("   Decoding Property (%d), %s",property->property,BACAPI_BACnetPropertyName(property->property));

		// Ovdje moramo handlati posebne propertije koji
		//imaju isti property ali razlicitu strukturu
		// TODO: BACAPI_IsSpecialProperty();
		bool special = BACAPI_IsSpecialProperty((BACNET_OBJECT_TYPE)object->oid.type,property->property);
		if (special)
		   {
		    int rlen = 0;
		    BACAPI_ERROR ret_err = DecodeSpecialProperty(&apdu[len],dataLen,&object->oid,property,&rlen);
		    if (ret_err != BACAPI_OK)
		       {
		        *ret_len = len;
		        return ret_err;
		       }
	        len += rlen;
		   }else {
				  // na ovom mjestu se provjeravaju kompleksni propertiji
				  // da li je ovaj property complex property?
				  // provjeri i dekodiraj complex property
				  if (BACAPI_IsComplexProperty(property->property))
					 {
					  len += DecodeComplexPropertyValue(&apdu[len],property);
					  uint8_t *pos = &apdu[len];

					 }else if (BACAPI_IsPropriateryProperty(property->property))
							  {
							   gLog->AppendLog("DecodeReadPropertyMultipleComplexAck: Property is Propriatery Property (%d)",property->property);
							   int ret = 0;
							   BACAPI_ERROR ret_err = DecodePropriateryValue(&apdu[len],dataLen, property, &ret);
							   if (ret_err != BACAPI_OK)
							      {
							       *ret_len = len;
							       return ret_err;
							      }
							   len += ret;
							  }else {
									 // dekodiraj vrijednost ovog propertyja
									 //int appDataLen = dataLen - len -1;

									 int ret = 0;
									 BACAPI_ERROR ret_err = DecodePrimitiveValue(&apdu[len],dataLen,property,&ret);
									 

									 if (ret_err != BACAPI_OK)
										{
										 *ret_len = len;
										 return ret_err;
										}

									 len += ret;
								     tmp = &apdu[len];
									}
				 }

        // ubaci novi property u listu propertyija u parametarskom objektu
        object->vProperties.push_back(property);
       }

 gLog->AppendLog("DecodeReadPropertyMultipleComplexAck: PropNum (%d)",object->vProperties.size());

 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ DECODE PRIMITIVE VALUE ]========================================//
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodePrimitiveValue(uint8_t *apdu, int dataLen, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;
 int data_len = 0;
 int tag_len = 0;
 int cursor = 0;
 uint32_t value = 0;

 // mozemo li uopce procitati property
 if (IsPropertyAccessError(apdu,&property->bac_err,&len))
    {
     property->propertyAccessError = true;
     *ret_len = len;
     return BACAPI_OK;
    }



 // decode opening tag [3E] ili [4E]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 *ret_len = 0;
	 return BACAPI_ERROR_INVALID_OPENING_TAG;
	}

 len++;

 // da li je sljedeci tag zatvarajuci tag?
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
	{
	 *ret_len = 2;
	 return BACAPI_OK;
	}

 // moramo dekodirati sve vrijednosti dok ne naidjemo na zatvarajuci tag ili je velicina veca od dataLen
 while((!gDec->decode_is_closing_tag_number(&apdu[len],4) &&
        !gDec->decode_is_closing_tag_number(&apdu[len],3)) || len > dataLen)
      {
       int ret = 0;

       uint8_t *tmp_ptr = &apdu[len];
       // stvori novu vrijednost i dekodiraj app data
       BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL);
       ret = gDec->bacapp_decode_application_data(&apdu[len],  (uint8_t) dataLen, value);
       if (ret < 0)
          {
           BACAPI_Messagef(0,"Error while trying to decode property application tag.",
                             "Error while decoding property (%s) data value. byte [%d]",
                             BACAPI_BACnetPropertyName(property->property),*ret_len);

           return BACAPI_ERROR_MALFORMED_PACKET;
          }
       else len+=ret;

       // malformed packet. Izbrisi zadnju vrijednost, zadrzi sve ispravno kodirane, zavrsi dekodiranje i vrati error
       if (len > dataLen)
          {
		   BACAPI_DeleteApplicationDataValue(value);
		   *ret_len = len;
           return BACAPI_ERROR_MALFORMED_PACKET;
          }

       gLog->AppendLog("I_cBACServiceHandler::DecodePrimitiveValue: Value (%s)",BACAPI_GetStringValue(value).c_str());
       
       if (property->property == PROP_OBJECT_NAME)
          {
           string s = value->type.character_String.value8;
          }
       property->vValueList.push_back(value);
      }

 // sljedeci tag mora biti zatvarajuci tag
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
	{
     len++;
    }else {
		   *ret_len = len;
		   return BACAPI_ERROR_INVALID_CLOSING_TAG;
          }



 // sve je ispravno.
 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\





BACAPI_ERROR I_cBACServiceHandler::DecodeSpecialProperty(uint8_t *data, int datalen, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *prop, int *ret_len)
{
 int len = 0;
 switch(prop->property)
       {
        case PROP_ACTION:
         if (oid->type == OBJECT_COMMAND)
            {
             BACAPI_ERROR err = DecodeBACnetActionList(data,prop,&len);
             if (err != BACAPI_OK)
                {
                 *ret_len = len;
                 return err;
                }
            }

         if (oid->type == OBJECT_LOOP)
            {
             BACAPI_ERROR err = DecodePrimitiveValue(data,datalen,prop,&len);
             if (err != BACAPI_OK)
                {
                 *ret_len = len;
                 return err;
                }
            }

         break;

       case PROP_PRESENT_VALUE:
        switch (oid->type)
               {
                case OBJECT_GROUP:
                {
                 BACAPI_ERROR err = DecodeReadAccessResult(data,prop,&len);
                 if (err != BACAPI_OK)
                    {
                     *ret_len = len;
                     return err;
                    }
                 break;
                }

                case OBJECT_GLOBAL_GROUP:
                {
                 BACAPI_ERROR err = DecodeBACnetPropertyAccessResult(data,prop,&len);
                 if (err != BACAPI_OK)
                    {
                     *ret_len = len;
                     return err;
                    }
                 break;
                }

                case OBJECT_CHANNEL:
                {
                 BACAPI_ERROR err = DecodeBACnetChannelValue(data,prop, &len);
                 if (err != BACAPI_OK)
                    {
                    *ret_len = len;
                    return err;
                    }
                 break;
                }

                case OBJECT_SCHEDULE:
                 {
                  BACAPI_ERROR err = DecodeScheduleAnyValue(data,prop,&len);
				  if (err != BACAPI_OK)
					 {
					  *ret_len = len;
					  return err;
					 }
				  break;
				 }
				 
				case OBJECT_CREDENTIAL_DATA_INPUT:
				 {
				  uint32_t value;
				  
				  // preskoci otvarajuci tag
				  len++;
				  
				  //dekodiraj vrijednosti
				  len += gDec->decode_context_unsigned(&data[len],0,&value);
				  len += gDec->decode_context_unsigned(&data[len],1,&value);
				  len += gDec->decode_context_unsigned(&data[len],2,&value);
				  
				  break;
				 }
				 
                default: return BACAPI_ERROR_PLACEHOLDER;
               }
        //return len;
       default: BACAPI_ERROR_PLACEHOLDER;
      }

 *ret_len = len;
 return BACAPI_OK;
}


//=====================================================================================//
//===================[ RP DECODE: APPLICATION DATA ]===================================//
// Ne upotrebljava se vise, sada se upotrebljava
// DecodePrimitiveValue();
//=====================================================================================//
//int I_cBACServiceHandler::DecodePrimitiveValueRP(uint8_t *data, int dataLen, BACNET_OBJECT_PROPERTY *property)
//{
// int len = 0;
// int ret = 0;
// int data_len = 0;
// int tag_len = 0;
// int cursor = 0;
// uint32_t value = 0;
//
// // Start of Data opening Tag [3]
// if (gDec->decode_is_opening_tag_number(&data[len], 3))
//    {
//     tag_len++;
//    }else return -1;
// // end?
// if (gDec->decode_is_closing_tag_number(&data[len],3))
//    {
//     // sve je u redu, u ovom propertiju ne postoje vrijednosti
//     tag_len++; // preskoci zatvarajuci tag i vrati duzinu
//     return tag_len;
//    }
//
// cursor = 0;  // pocetak informacija
// uint8_t *data_pos = &data[tag_len];
//
//
// // ovdje dolazi vrijednost ili lista vrijednosti
// while (!gDec->decode_is_closing_tag_number(&data_pos[cursor], 3)) // da li je sljedeci byte zatvarajuci?
//       {
//        BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL);
//
//        ret = gDec->bacapp_decode_application_data(&data_pos[cursor],  (uint8_t) dataLen, value);
//
//        cursor += ret;
//        if (cursor > dataLen)
//           {
//            BACAPI_DeleteObjectProperty(property);
//            // malformed packet. Izbrisi zadnju vrijednost, zavrsi dekodiranje i vrati
//            // malformed  packet errror
//			BACAPI_DeleteApplicationDataValue(value);
//            return BACAPI_ERROR_MALFORMED_PACKET;
//           }
//        property->vValueList.push_back(value);
//       }
//
// cursor++; // preskoci zatvarajuci tag
//
// // priority postoji?
// ret = gDec->decode_context_unsigned(&data_pos[cursor],4,&value);
// if (ret >= 0)
//    {
//     cursor += ret;
//     property->priority = value;
//    }else {
//           property->priority = 0;
//          }
//
//
//
// return cursor;
//}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ DECODE: PRIMITIVE VALUE RPM ]==========================//
// Ne upotrebljava se vise, sada se upotrebljava
// DecodePrimitiveValue();
//=====================================================================================//
//int I_cBACServiceHandler::DecodePrimitiveValueRPM(uint8_t *data, int dataLen, BACNET_OBJECT_PROPERTY *property, uint8_t tag)
//{
// bool is_open_tag;
// int len = 0;
// int ret = 0;
// bool first_value = true;
// bool print_brace = false;
//
//  //Tag 4: value of property
//  //Start of Data opening Tag [4]
// if (gDec->decode_is_opening_tag_number(&data[len], tag))
//    {
//     len++;
//    }else return -1;
//
// // end?
// if (gDec->decode_is_closing_tag_number(&data[len],tag))
//    {
//     // sve je u redu, u ovom propertiju ne postoje vrijednosti
//     len++; // preskoci zatvarajuci tag i vrati duzinu
//     return len;
//    }
//
//
// while (!gDec->decode_is_closing_tag_number(&data[len], tag))
//       {
//        uint8_t *tmp = &data[len];
//        BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL);
//
//        ret = gDec->bacapp_decode_application_data(&data[len],  (uint8_t) dataLen, value);
//        if (ret == 0)
//           {
//            // ako je ovo opening tag, ponovno pokreni funkciju
//            if (gDec->decode_is_opening_tag(&data[len]))
//               {
//                uint8_t new_tag = 0;
//                gDec->decode_tag_number(&data[len],&new_tag);
//                len += DecodePrimitiveValueRPM(&data[len],dataLen,property,new_tag);
//               }else {
//                      // context data octets
//                      uint8_t oct_tag = 0;
//                      uint32_t oct_val = 0;
//                      BACAPI_DeleteApplicationDataValue(value);
//                      value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_OSTRING);
//
//                      uint8_t *tmp_pos = &data[len];
//
//                      len += gDec->decode_tag_number_and_value(&data[len],&oct_tag,&oct_val);
//
//                      for (int c=0;c<oct_val;c++)
//                           value->type.Octet_String.vValue.push_back(data[len+c]);
//
//                      len+=oct_val;
//                      property->vValueList.push_back(value);
//
//                     }
//           }else if (ret < 0)
//					{
//					 delete value;
//					 value = NULL;
//					 // prevrti do [4F] ili do data_len
//					 while(!gDec->decode_is_closing_tag_number(&data[len], 4))
//						  {
//						   len++;
//						   if (len > dataLen)
//							  {
//							   // izbrisi sve stvorene vrijednosti
//							   BACAPI_DeleteObjectPropertyValues(property);
//							   return BACAPI_ERROR_MALFORMED_PACKET;
//							  }
//						  }
//					}else {
//                           // sve je u redu
//                           len += ret;
//						   uint8_t *tmp1 = &data[len];
//						   property->vValueList.push_back(value);
//                          }
//       }
//
// len++; // preskoci zatvarajuci tag
//
//
//
//
// return len;
// return 0;
//}
//_____________________________________________________________________________________\\








//=====================================================================================//
//===================[ DECODE PROPRIATERY VALUE ]======================================//
// pokusaj dekodirati primitivnu vrijednost. Ako nije primitivna vrijednost onda
// prevrti cijeli bytecode do kraja i stavi u octet_string i oznaci property kao complex propriatery property
//====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodePropriateryValue(uint8_t *data, int datalen, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;
 BACAPI_ERROR ret_err = BACAPI_OK;
 BACNET_ERROR bac_err;

 gLog->AppendLog("DecodeProprietaryValue: Dekodiram property: %d",property->property);

 // Da li smo naisli na Property Access Error?
if (IsPropertyAccessError(data,&property->bac_err,&len))
   {
    property->propertyAccessError = true;
    *ret_len = len;
    return BACAPI_OK;
   }

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return BACAPI_ERROR_MALFORMED_PACKET;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

 uint8_t dec_tag = 0;
 gDec->decode_tag_number(&data[len],&dec_tag);

 ret_err = DecodePropriateryPrimitiveValue(&data[len], datalen, property, &ret);
 if (ret_err != BACAPI_OK)
    {
     // ovo nije primitivna vrijednost, stvori oktet string i napuni oktetima sve do zatvarajuceg taga
     BACNET_APPLICATION_DATA_VALUE *val = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_OSTRING);
     while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3)) //  && len < datalen
           {
            val->type.Octet_String.vValue.push_back(data[len]);
            len++;
           }

     char msg[256];
     char desc[256];
     sprintf(msg,"Proprietery property %d could not be decoded and will be presented as an Octet String.",property->property);
     sprintf(desc,"Proprietery property %d included proprietery complex values and could not be decoded by this version of BACnet Lookout.",property->property);

     BACAPI_Message(msg,desc,2);
     property->vValueList.push_back(val);
    }else {
            len += ret;
           }

 uint8_t *pos = &data[len];
 // preskoci zatvarajuci tag
 len++;
 *ret_len = len;

 return BACAPI_OK;
 ///return len;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ DECODE PROPRIATERY PRIMITIVE VALUE ]============================//
//=====================================================================================//
// Ovo je esencijalno ista funkcija kao i DecodePrimitiveValue samo sto se kod propriatery
// propertija opening tagovi gledaju u DecodeProperiateryValue funkciji umjesto ovdje
//BACAPI_ERROR_PROPRIETARY_COMPLEX_VALUE
//=====================================================================================//

BACAPI_ERROR I_cBACServiceHandler::DecodePropriateryPrimitiveValue(uint8_t *apdu, int dataLen, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;
 // moramo dekodirati sve vrijednosti dok ne naidjemo na zatvarajuci tag ili je velicina veca od dataLen
 while((!gDec->decode_is_closing_tag_number(&apdu[len],4) &&
        !gDec->decode_is_closing_tag_number(&apdu[len],3)) || len > dataLen)
      {
       uint8_t *tmp = &apdu[len];
       // hack. JCI NAE na ovom mjestu moze imati 5E i 5F tagove zbog necega.
       // ako naidjes na bilo koji optvarajuci tag na ovom mjestu preskoci do zatvarajuceg
       if (gDec->decode_is_opening_tag(&apdu[len]))
          {
           return BACAPI_ERROR_PROPRIETARY_COMPLEX_VALUE;
          }

       // stvori novu vrijednost i dekodiraj app data
       BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL);

       ret = gDec->bacapp_decode_application_data(&apdu[len],  (uint8_t) dataLen, value);
       if (ret < 0)
           return BACAPI_ERROR_MALFORMED_PACKET;
           else len += ret;



       // malformed packet. Izbrisi zadnju vrijednost, zadrzi sve ispravno kodirane, zavrsi dekodiranje i vrati error
       if (len > dataLen)
          {
		   BACAPI_DeleteApplicationDataValue(value);
		   *ret_len = len;
           return BACAPI_ERROR_MALFORMED_PACKET;
          }
       property->vValueList.push_back(value);
      }
 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\



//
//BACAPI_ERROR I_cBACServiceHandler::DecodePropriateryPrimitiveValue(uint8_t *apdu, int dataLen, BACNET_OBJECT_PROPERTY *property, int *ret_len)
//{
// int len = 0;
// // moramo dekodirati sve vrijednosti dok ne naidjemo na zatvarajuci tag ili je velicina veca od dataLen
// while((!gDec->decode_is_closing_tag_number(&apdu[len],4) &&
//        !gDec->decode_is_closing_tag_number(&apdu[len],3)) || len > dataLen)
//      {
//       uint8_t *tmp = &apdu[len];
//       // hack. JCI NAE na ovom mjestu moze imati 5E i 5F tagove zbog necega.
//       // ako naidjes na bilo koji optvarajuci tag na ovom mjestu preskoci do zatvarajuceg
//       if (gDec->decode_is_opening_tag(&apdu[len]))
//          {
//
//           uint8_t tag_num = apdu[len];
//           tag_num = tag_num >> 4;
//
//           while (!gDec->decode_is_closing_tag_number(&apdu[len++],tag_num)) {}
//
//           uint8_t *tmp = &apdu[len];
//
//           *ret_len = len;
//
//           return BACAPI_OK;
//          }
//
//       // stvori novu vrijednost i dekodiraj app data
//       BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL);
//       len += gDec->bacapp_decode_application_data(&apdu[len],  (uint8_t) dataLen, value);
//
//       // malformed packet. Izbrisi zadnju vrijednost, zadrzi sve ispravno kodirane, zavrsi dekodiranje i vrati error
//       if (len > dataLen)
//          {
//		   BACAPI_DeleteApplicationDataValue(value);
//		   *ret_len = len;
//           return BACAPI_ERROR_MALFORMED_PACKET;
//          }
//       property->vValueList.push_back(value);
//      }
// *ret_len = len;
// return BACAPI_OK;
//}
////_____________________________________________________________________________________\\
//


//=====================================================================================//
//===================[ PROPERTY ACCESS ERROR ]=========================================//
// Neki propertiji kao npr log_buffer kod tren loga ili properiatery propertija
// mogu imati read-access-denied specificiran u obliku [5E] i [5F] tagova.
// To se ovdje rijesava
//=====================================================================================//
bool I_cBACServiceHandler::IsPropertyAccessError(uint8_t *apdu, BACNET_ERROR *bac_err, int *ret_len)
{
 int len = 0;
 if (!gDec->decode_is_opening_tag_number(&apdu[len],5)) return false;

 len++;

 // error class/code
 len += gDec->bacerror_decode_error_class_and_code(&apdu[len],1,&bac_err->errClass,&bac_err->errCode);

 len++; // 5F

 *ret_len = len;
 return true;
}




//
////------:: ERROR TAG
//int I_cBACServiceHandler::DecodeIsPropertyAccessError(uint8_t *data, BACNET_PROPERTY_ID propid)
//{
// int len = 0;
// // Property Access Error?
// if (gDec->decode_is_opening_tag_number(&data[len],5))
//    {
//     len++;
//     BACNET_ERROR bacerror;
//
//     // NOTE: apdu_len = 1 znaci da samo provjeravamo da li postoji, popraviti to
//     len += gDec->bacerror_decode_error_class_and_code(&data[len],1,&bacerror.errClass,&bacerror.errCode);
//
//     //------:: ERROR TAG
//     //// ispali error
//     //BACAPI_Event *event = BACAPI_CreateEvent();
//     //event->type = BACAPI_EVENT_ERROR;
//     //event->error = new BACNET_ERROR;
//     //char tmp_msg[64];
//     //sprintf(tmp_msg,"Property Access Error: %s",BACAPI_BACnetPropertyName(propid));
//     //event->error->err_msg = tmp_msg;
//     //BACAPI_PushEvent(event);
//     //------:: ERROR TAG END
//
//     uint8_t *tmp_pos = &data[len];
//     len++;  // [5F]
//     return len;
//    }
// return 0;
//}




//
//
//int I_cBACServiceHandler::wip_DecodePropriateryByteCode(uint8_t *data, int datalen, uint8_t tag)
//{
// static int len = 0;
//
// while (!gDec->decode_is_closing_tag(&data[len]))
//       {
//        len++;
//        if (gDec->decode_is_opening_tag(&data[len]))
//           {
//            gLog->AppendLog("opening %02X\n",data[len]);
//            wip_DecodePropriateryByteCode(&data[len],datalen,data[len]);
//           }
//       }
// uint8_t *tmp_pos = &data[len];
// gLog->AppendLog("Closing %02X\n",data[len]);
// len++;
//
//
// return len;
//}





//=====================================================================================//
//===================[ DECODE COMPLEX PROPERTY VALUE ]=================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeComplexPropertyValue(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 BACAPI_ERROR berr = BACAPI_OK;
 int len = 0;
 //gLog->AppendLog("I_cBACServiceHandler::DecodeComplexPropertyValue: %s\n", bacnetPropertyName[property->property].name);

 // mozemo li uopce pristupiti vrijednosti?
 if (IsPropertyAccessError(data,&property->bac_err,&len))
    {
     property->propertyAccessError = true;
     //*ret_len = len;
     //return BACAPI_OK;
     return len;
    }

 switch (property->property)
        {
        case PROP_ACCESS_DOORS:
        case PROP_CREDENTIALS_IN_ZONE:
        case PROP_MEMBERS:
        case PROP_CREDENTIALS:
        case PROP_LOG_DEVICE_OBJECT_PROPERTY:
        case PROP_GROUP_MEMBERS:
			  {
			   len = DecodeDeviceObjectPropertyReference(data, property);
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
			   BACAPI_ERROR ret_err = DecodeDateTime(data, property, &len);
			   return len;
			  }

        case PROP_SUBORDINATE_LIST:
        case PROP_DOOR_MEMBERS:
        case PROP_ZONE_MEMBERS:
        case PROP_MEMBER_OF:
        case PROP_ENTRY_POINTS:
        case PROP_EXIT_POINTS:
        case PROP_ACCOMPANIMENT:
        case PROP_BELONGS_TO:
        case PROP_LAST_ACCESS_POINT:
        case PROP_ACCESS_EVENT_CREDENTIAL:
        case PROP_ZONE_FROM:
        case PROP_ZONE_TO:
        case PROP_LAST_CREDENTIAL_ADDED:
        case PROP_LAST_CREDENTIAL_REMOVED:
        case PROP_ENERGY_METER_REF:
		     {
			  len = DecodeDeviceObjectReference(data,property);
			  return len;
			 }


         case PROP_SETPOINT_REFERENCE:
              {
               len = DecodeSetPointObjectPropertyReference(data,property);
               return len;
              }


         case PROP_MANIPULATED_VARIABLE_REFERENCE:
         case PROP_CONTROLLED_VARIABLE_REFERENCE:
         case PROP_INPUT_REFERENCE:
         case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
         case PROP_OBJECT_PROPERTY_REFERENCE:
         case PROP_EVENT_ALGORITHM_INHIBIT_REF:
			  {
			   len = DecodeObjectPropertyReference(data,property);
			   return len;
			  }




         case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
         case PROP_RESTART_NOTIFICATION_RECIPIENTS:
         case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
         case PROP_COVU_RECIPIENTS:
              {
			   len = DecodeRecipient(data, property);
               return len;
              }

         case PROP_SUBSCRIBED_RECIPIENTS:
         {
          int ret_len = 0;
          berr = DecodeEventNotificationSub(data,property, &ret_len);
          return ret_len;
         }

         case PROP_RECIPIENT_LIST:
			  {
			   len = DecodeBACDectination(data,property);
			   return len;
			  }


         case PROP_EFFECTIVE_PERIOD:
              {
               BACAPI_ERROR err_type = DecodeDateRange(data,property, &len);
               return len;
              }


         case PROP_EVENT_TIME_STAMPS:
         case PROP_LAST_RESTORE_TIME:
         case PROP_TIME_OF_DEVICE_RESTART:
         case PROP_ACCESS_EVENT_TIME:
         case PROP_LAST_COMMAND_TIME:
			  {
			   len = DecodeEventTimeStamps(data, property);
			   return len;
			  }

         case PROP_ACTIVE_COV_SUBSCRIPTIONS:
              {
               len = DecodeCOVSubscriptions(data,property);
               return len;
              }
         break;


         case PROP_DATE_LIST:
              {
               len = DecodeCalendarEntry(data,property);
               return len;
              }
         break;

         case PROP_PRIORITY_ARRAY:
			  {
			   len = DecodePriorityList(data,property);
			   return len;
			  }

         case PROP_WEEKLY_SCHEDULE:
              {
               len = DecodeWeeklySchedule(data,property);
               return len;
              }

         case PROP_EXCEPTION_SCHEDULE:
              {
               len = DecodeSpecialEvent(data, property);
               return len;
              }
         break;


         case PROP_LIST_OF_GROUP_MEMBERS:
              {
               len = DecodeReadAccessSpecification(data,property);
               return len;
              }

         case PROP_MANUAL_SLAVE_ADDRESS_BINDING:
         case PROP_DEVICE_ADDRESS_BINDING:
         case PROP_SLAVE_ADDRESS_BINDING:
              {
               len = DecodeDeviceAddressBind(data,property);
               return len;
              }

         case PROP_ACTION:
              {
               len = DecodeBACnetActionList(data,property);
               return len;
              }

         case PROP_ACTION_TEXT:
              {
               len = DecodeActionText(data,property);
               return len;
              }

		 case PROP_ACCEPTED_MODES:
		 case PROP_LIFE_SAFETY_ALARM_VALUES:
		 case PROP_ALARM_VALUES:
		 case PROP_FAULT_VALUES:
		 case PROP_MASKED_ALARM_VALUES:
		      {
		       len = DecodeEnumList(data,property);
		       return len;
		      }

		 case PROP_EVENT_PARAMETERS:
		      {
		       len = DecodeEventParameters(data,property);
		       return len;
		      }

         case PROP_LOG_BUFFER:
              {
               len = DecodeLogBuffer(data,property);
               return len;
              }

         case PROP_SCALE:
              {
               len = DecodeScale(data, property);
               return len;
              }

         case PROP_PRESCALE:
              {
               len = DecodePreScale(data, property);
               return len;
              }

         case PROP_REQUESTED_SHED_LEVEL:
         case PROP_ACTUAL_SHED_LEVEL:
         case PROP_EXPECTED_SHED_LEVEL:
              {
               len = DecodeShedLevel(data,property);
               return len;
              }

         case PROP_ACTIVE_VT_SESSIONS:
              {
               len = DecodeActiveVTSessions(data,property);
               return len;
              }

         case PROP_LOGGING_RECORD:
              {
               len = DecodeLoggingRecord(data,property);
               return len;
              }
         case PROP_FAULT_PARAMETERS:
              {
               len = DecodeFaultParameters(data,property);
               return len;
              }
         case PROP_AUTHENTICATION_POLICY_LIST:
              {
               len = DecodeAuthenticationPolicyList(data,property);
               return len;
              }
         case PROP_NEGATIVE_ACCESS_RULES:
         case PROP_POSITIVE_ACCESS_RULES:
         {
          len = DecodeBACnetAccessRule(data,property);
          return len;
         }

         case PROP_AUTHENTICATION_FACTORS:
         {
          int ret_len = 0;
          berr = DecodeCredentialAuthenticationFactors(data,property,&ret_len);
          len = ret_len;
          return len;
         }

         case PROP_ASSIGNED_ACCESS_RIGHTS:
         {
          int ret_len = 0;
          berr = DecodeAssignedAccessRights(data,property,&ret_len);
          len = ret_len;
          return len;
         }

         case PROP_SUPPORTED_FORMATS:
         {
          len = DecodeAuthenticationFactorFormat(data,property);
          return len;
         }
         
         case PROP_NETWORK_ACCESS_SECURITY_POLICIES:
         {
          int ret_len = 0;
          berr = DecodeNetworkAccessSecurityPolicies(data,property,&ret_len);
          len = ret_len;	         
          return len;
         }

         case PROP_KEY_SETS:
         {
          int ret_len = 0;
          berr = DecodeSecurityKeySets(data,property,&ret_len);
          len = ret_len;	         
          return len;
         }




         case PROP_PORT_FILTER:
         {
          len = DecodePortPermission(data, property);
          return len;
         }

         case PROP_PROCESS_IDENTIFIER_FILTER:
         {
          len = DecodeProcessIdSelection(data, property);
          return len;
         }

         case PROP_LIGHTING_COMMAND:
         {
          len = DecodeLightingCommand(data, property);
          return len;
         }

         case PROP_SCHEDULE_DEFAULT:
         {
          berr = DecodeScheduleAnyValue(data,property,&len);
          return len;
         }

         case PROP_BACNET_IP_GLOBAL_ADDRESS:
         case PROP_FD_BBMD_ADDRESS:
         {
          berr = DecodeBACnetHostNPort(data,property,&len);
          return len;
         }
		 
		 
		 case PROP_BBMD_BROADCAST_DISTRIBUTION_TABLE:
         {
          berr = DecodeBACnetBDTEntry(data,property,&len);
          return len;
         }
		 
		 
		 case PROP_BBMD_FOREIGN_DEVICE_TABLE:
         {
          berr = DecodeBACnetFDTEntry(data,property,&len);
          return len;
         }
		 

		 case PROP_VIRTUAL_MAC_ADDRESS_TABLE:
         {
          berr = DecodeVMACAddressTable(data,property,&len);
          return len;
         }
		  
		 
		 case PROP_ROUTING_TABLE:
		 {
		  berr  = DecodeRoutingTable(data,property,&len);
		  return len;
		 }
		 

		 
         case PROP_COMMAND_TIME_ARRAY:
         {
          berr = DecodeTimeStampArray(data,property,&len);
          return len;
         }
         
         case PROP_VALUE_SOURCE:
         case PROP_VALUE_SOURCE_ARRAY:
         {
          berr = DecodeValueSource(data,property,&len);
          return len;
         }
         

		 
		 default:
		 {
		 
		  //while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true && len < 1476)
    //            {
    //             len++;
    //            }
		 
		  // prevrti do kraja
		  while (gDec->decode_is_closing_tag_number(&data[len],4) == true ||
			     gDec->decode_is_closing_tag_number(&data[len],3) == true || len > 1476)
				 {
				  len++;
				 }		  
		  // preskoci zatvarajuci tag
		  len++;
		  return len;
		 }
 

        }

 return 0;
}
//_____________________________________________________________________________________\\




BACAPI_ERROR I_cBACServiceHandler::DecodeEventNotificationSub(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 
 
  // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return BACAPI_ERROR_INVALID_TAG; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    } 

  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        BACNET_EVENT_NOTIFICATION_SUBSCRIPTION sub;
        len += gDec->decode_notif_sub(&data[len],&sub);
        property->complexValue.vNotificationSubList.push_back(sub);
        len++;
       }

 len++;
 *ret_len = len;
 
 return BACAPI_OK;
}




BACAPI_ERROR I_cBACServiceHandler::DecodeValueSource(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;
 uint32_t value = 0;
 
 
 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return BACAPI_ERROR_INVALID_TAG; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    } 
    
 int t=0;
 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_VALUE_SOURCE *src  = new BACNET_VALUE_SOURCE;
        len += gDec->decode_value_source(&data[len],src);
        property->complexValue.vValueSource.push_back(src);     
        t++;
       }
 
 
 return BACAPI_OK;
}



BACAPI_ERROR I_cBACServiceHandler::DecodeTimeStampArray(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;
 uint32_t value = 0;
 
 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return BACAPI_ERROR_INVALID_TAG; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_TIMESTAMP *new_stamp = new BACNET_TIMESTAMP;
        uint8_t *tmp = &data[len];
        len += gDec->bacapp_decode_timestamp(&data[len],new_stamp,property->property);
        property->complexValue.vTimeStamps.push_back(new_stamp);
       }
 
 
 
 
 return BACAPI_OK;
}



BACAPI_ERROR I_cBACServiceHandler::DecodeRoutingTable(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;
 uint32_t value = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 return BACAPI_ERROR_INVALID_TAG;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 while (gDec->decode_is_closing_tag_number(&apdu[len], 4)!= true && gDec->decode_is_closing_tag_number(&apdu[len], 3) != true)
       {
        BACNET_ROUTER_ENTRY *entry = new BACNET_ROUTER_ENTRY;
        len += gDec->decode_context_unsigned(&apdu[len],0, &value);
        entry->network_number = (uint16_t)value;
        value = 0;
        len += gDec->decode_context_octet_string(&apdu[len],1,&entry->mac_address);
        len += gDec->decode_context_enumerated(&apdu[len],2,&entry->status);
        len += gDec->decode_context_unsigned(&apdu[len],3, &value);
        entry->performance_index = (uint8_t)value;
        value = 0;
        
        property->complexValue.vRouterEntry.push_back(entry);
       }




 len++; // preskoci zatvarajuci tag;
 
 *ret_len = len;
 return BACAPI_OK; 
}


     


BACAPI_ERROR I_cBACServiceHandler::DecodeVMACAddressTable(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;
 uint32_t value = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 return BACAPI_ERROR_MALFORMED_PACKET;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 while (gDec->decode_is_closing_tag_number(&apdu[len], 4)!= true && gDec->decode_is_closing_tag_number(&apdu[len], 3) != true)
       {
        BACNET_VMAC_ENTRY *entry = new BACNET_VMAC_ENTRY;
        len += gDec->decode_context_octet_string(&apdu[len],0,&entry->virtual_mac_address);
        len += gDec->decode_context_octet_string(&apdu[len],1,&entry->native_mac_address);

        
        property->complexValue.vVMACEntry.push_back(entry);
       }




 len++; // preskoci zatvarajuci tag;
 
 *ret_len = len;
 return BACAPI_OK; 
}





BACAPI_ERROR I_cBACServiceHandler::DecodeBACnetFDTEntry(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;
 uint32_t value = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 return BACAPI_ERROR_MALFORMED_PACKET;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 while (gDec->decode_is_closing_tag_number(&apdu[len], 4)!= true && gDec->decode_is_closing_tag_number(&apdu[len], 3) != true)
       {
        BACNET_FDT_ENTRY *entry = new BACNET_FDT_ENTRY;
        
		len += gDec->decode_context_octet_string(&apdu[len],0,&entry->ip_address);
        len += gDec->decode_context_unsigned(&apdu[len],1,&value);
        entry->ttl = (uint16_t)value; 
        value = 0;
        
        len += gDec->decode_context_unsigned(&apdu[len],2,&value);
        entry->rem_ttl = (uint16_t)value;
        value = 0;
        
        property->complexValue.vFDTEntry.push_back(entry);
       }


 len++; // preskoci zatvarajuci tag;
 
 *ret_len = len;
 return BACAPI_OK; 
}






BACAPI_ERROR I_cBACServiceHandler::DecodeBACnetBDTEntry(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;
 uint32_t value = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 return BACAPI_ERROR_MALFORMED_PACKET;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 while (gDec->decode_is_closing_tag_number(&apdu[len], 4)!= true && gDec->decode_is_closing_tag_number(&apdu[len], 3) != true)
       {
        BACNET_BDT_ENTRY *entry = new BACNET_BDT_ENTRY;
        
		if (gDec->decode_is_opening_tag_number(&apdu[len],0))
		   {
			len++;  // opening tag
			len += gDec->decode_bacnet_host_port(&apdu[len],&entry->bbmd_address);

			len += gDec->decode_context_unsigned(&apdu[len],1,&value);
			entry->bbmd_address.port = (uint16_t)value;
			value = 0;

			// sljedeci mora biti zatvarajuci tag
			if (gDec->decode_is_closing_tag_number(&apdu[len],0))
				len++;  //
		   }
		
		len += gDec->decode_context_octet_string(&apdu[len],1,&entry->broadcast_mask);
		property->complexValue.vBDTEntry.push_back(entry);
       }


 len++; // preskoci zatvarajuci tag;
 
 *ret_len = len;
 return BACAPI_OK;
}



BACAPI_ERROR I_cBACServiceHandler::DecodeBACnetHostNPort(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;
 uint32_t value = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 return BACAPI_ERROR_MALFORMED_PACKET;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

 // host address
 BACNET_HOST_NPORT *port = new BACNET_HOST_NPORT;
 memset(port,0,sizeof(BACNET_HOST_NPORT));
 
 if (gDec->decode_is_opening_tag_number(&apdu[len],0))
    {
     len++;  // opening tag
     
     len += gDec->decode_bacnet_host_port(&apdu[len],port);

     // sljedeci mora biti zatvarajuci tag
     if (gDec->decode_is_closing_tag_number(&apdu[len],0))
        {
         len++;  //
        }
    }

 len += gDec->decode_context_unsigned(&apdu[len],1,&value);
 port->port = (uint16_t)value;
     
 property->complexValue.hostnPort = port;
     
 uint8_t *tmp = &apdu[len];
 
 *ret_len = len;
 return BACAPI_OK;
}





//=====================================================================================//
//===================[ COMPLEX: DATE TIME ]============================================//
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeDateTime(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0, ret = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return BACAPI_ERROR_MALFORMED_PACKET;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

 ret = gDec->decode_application_datetime(&data[len],&property->complexValue.dateTime);
 if (ret < 0)
    {
     // povecaj len do zatvarajuceg byta za ovu vrijednost
     while (gDec->decode_is_closing_tag_number(&data[len],4) == true ||
            gDec->decode_is_closing_tag_number(&data[len],3) == true || len > 1476)
           {
            len++;
           }

     // preskoci zatvarajuci tag, postavi return len i vrati error
     len++;
     *ret_len = len;
     return BACAPI_ERROR_MALFORMED_PACKET;
    }else len+=ret;


 // preskoci zatvarajuci tag;
 len++;
 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ COMPLEX: PRIORITY LIST ]========================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodePriorityList(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }



 // dekodiraj sve vrijednosti sve dok ne dodjes do zatvarajuceg taga
 // NOTE: ako nema zatvarajuceg taga ici ce u nedogled
 while (gDec->decode_is_closing_tag_number(&data[len], 4)!= true && gDec->decode_is_closing_tag_number(&data[len], 3) != true)
       {
        // stvori novu vrijednost za ovaj property
        BACNET_APPLICATION_DATA_VALUE *value = new BACNET_APPLICATION_DATA_VALUE;

	    len += gDec->decode_priority_value(&data[len], BACNET_MAX_APDU_IP,value,property->property);

        property->vValueList.push_back(value);
       }
 len++;   // zatvarajuci tag  [4F]
          // TODO: provjeri da li je [4f]
 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ COMPLEX: COV SUBS ]=============================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeCOVSubscriptions(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 uint8_t *pos = &data[len];
 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }

 pos = &data[len];

 // mozda ne postoje upisane vrijednosti, provjeri da li je sljedeci tag zatvarajuci tag
 if (gDec->decode_is_closing_tag_number(&data[len],4) && gDec->decode_is_closing_tag_number(&data[len],3))
    {
     // sve je u redu, u ovom propertiju ne postoje vrijednosti
     len++; // preskoci zatvarajuci tag i vrati duzinu
     return len;
    }

 // dekodiraj sve vrijednosti sve dok ne dodjes do zatvarajuceg taga
 while (!gDec->decode_is_closing_tag_number(&data[len], 4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_COV_SUBSCRIPTION *new_cov = new BACNET_COV_SUBSCRIPTION;
	    len += gDec->decode_cov_subscription(&data[len], BACNET_MAX_APDU_IP,new_cov);
	    property->complexValue.vActiveCOVSubs.push_back(new_cov);
       }

 len++;   // zatvarajuci tag  [4F]/[3F]
 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ COMPLEX: ADDRESS BINDING ]======================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeDeviceAddressBind(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }



 // dekodiraj sve vrijednosti sve dok ne dodjes do zatvarajuceg taga
 while (gDec->decode_is_closing_tag_number(&data[len], 4) != true && gDec->decode_is_closing_tag_number(&data[len], 3) != true)
       {
        int ret = 0;
        BACNET_ADDRESS_BINDING *adr_bind = new BACNET_ADDRESS_BINDING;

        ret = gDec->decode_device_address_binding(&data[len], adr_bind);
        if (ret > 1 && ret < sizeof(BACNET_ADDRESS_BINDING))  // postoji vrijednost
            property->complexValue.vBACnetAddressBindingList.push_back(adr_bind);

        len+=ret;
       }

 uint8_t *ptrData = &data[len];
 len++;   // zatvarajuci tag  [4F];
 return len;
}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ COMPLEX: WEEKLY SCHEDULE ]======================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeWeeklySchedule(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 // pocetak liste address bind
 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }



 int ret = gDec->decode_weekly_schedule(&data[len],BACNET_MAX_APDU_IP,&property->complexValue.weeklySchedule);
 if (ret < 0)
    {
     // TODO: ako je drivo kodiranje
    }
 len += ret;



 len++;  // preskoce zatvarajuci tag

 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ COMPLEX: DATE RANGE ]===========================================//
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeDateRange(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return BACAPI_ERROR_INVALID_OPENING_TAG; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 ret = gDec->decode_daterange(&data[len], &property->complexValue.dateRange);
 if (ret < 0)
    {
     char msg[256];
     char desc[256];
     sprintf(msg,"Error while decoding property: %s",BACAPI_BACnetPropertyName(property->property));
     sprintf(desc,"Error while decoding Date Range Value of property: %s",BACAPI_BACnetPropertyName(property->property));
     BACAPI_Message(msg,desc,1);

     // prevrti do zatvarajuceg taga i nastavi dekodiranje
     while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
            len++;

    }else len += ret;



 len++; // preskoci zatvarajuci tag
 *ret_len = len;

 return BACAPI_OK;
}
//_____________________________________________________________________________________\\






//=====================================================================================//
//===================[ COMPLEX: SPECIAL EVENT ]========================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeSpecialEvent(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }

 uint8_t *tmp = &data[len];

 // mozda ne postoje upisane vrijednosti, provjeri da li je sljedeci tag zatvarajuci tag
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     // sve je u redu, u ovom propertiju ne postoje vrijednosti
     len++; // preskoci zatvarajuci tag i vrati duzinu
     return len;
    }



 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_SPECIAL_EVENT special_event;
        //len += gDec->decode_special_event(&data[len],BACNET_MAX_APDU_IP,&special_event);
        len += tmp_decode_special_event(&data[len],&special_event);

        property->complexValue.vSpecialEventList.push_back(special_event);
       }


  len++;

 return len;
}
//_____________________________________________________________________________________\\





/*
BACnetSpecialEvent::= SEQUENCE {
period  CHOICE {
                calendarEntry [0] - BACnetCalendarEntry,
                calendarReference [1] - BACnetObjectIdentifier
               },
listOfTimeValues [2] - SEQUENCE OF BACnetTimeValue,
eventPriority [3] Unsigned (1..16)
}
*/
int I_cBACServiceHandler::tmp_decode_special_event(uint8_t *data, BACNET_SPECIAL_EVENT *ptr_special)
{
 int len = 0;

 if (gDec->decode_is_context_tag(&data[len], 0) && !gDec->decode_is_closing_tag(&data[len]))   // [0] calendar entry
    {
     ptr_special->TagSpecialEventType = 0;
     len = gDec->decode_context_calendar_entry(&data[len], 1400, 0, &ptr_special->period.calendarEntry);  // TODO: provjeri max_apdu
    }else if (gDec->decode_is_context_tag(&data[len], 1) && !gDec->decode_is_closing_tag(&data[len]))  // [1] object_id
             {
              ptr_special->TagSpecialEventType = 1;
              len = gDec->decode_context_object_id(&data[len], 1,&ptr_special->period.calendarReference.type, &ptr_special->period.calendarReference.instance);
             }


 // [2] list of time values
 if (gDec->decode_is_opening_tag_number(&data[len], 2))
    {
     len++;
     while (!gDec->decode_is_closing_tag_number(&data[len],2))
           {
            BACNET_TIME_VALUE time_value;
            len += tmp_decode_context_time_value(&data[len], &time_value);
            ptr_special->vTimeValueList.push_back(time_value);
           }
    }

 len++;

 if (gDec->decode_is_context_tag(&data[len], 3) && !gDec->decode_is_closing_tag(&data[len])) // [3] eventPriority
    {
     uint32_t priority;
     len += gDec->decode_context_unsigned(&data[len], 3, &priority);

     if (len < 0)return -1;

     ptr_special->eventPriority = (uint8_t) priority;
    }


 return len;
}

int I_cBACServiceHandler::tmp_decode_context_time_value(uint8_t *data, BACNET_TIME_VALUE *time_value)
{
 int len = 0;
 len = gDec->decode_time_and_value(&data[len], 1400 ,time_value);
 return len;
}







//=====================================================================================//
//===================[ COMPLEX: OBJECT PROPERTY REF ]==================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeObjectPropertyReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }

 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_OBJECT_PROPERTY_REFERENCE *ref = new BACNET_OBJECT_PROPERTY_REFERENCE;
        uint8_t *tmp = &data[len];

        int ret = gDec->bacapp_decode_obj_property_ref(&data[len],ref);
        if (ret < 0)
           {
            delete ref;

            // TODO: len nece biti ispravan, trebat ce prevrtit fo 4f taga ovdje
            BACAPI_SetLastError(BACAPI_ERROR_MALFORMED_OBJECT_CODING);
            while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
                  {
                   len++;
                  }

            break;
           }else {
                  len += ret;
                 }

        property->complexValue.vObjectPropertyReference.push_back(ref);
	   }

 uint8_t *tmp = &data[len];
 len++; // preskoci zatvarajuci tag

 return len;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ COMPLEX: BAC DESTINATION ]======================================//
// TODO: izbaciti linkane liste i staviti vector
//=====================================================================================//
int I_cBACServiceHandler::DecodeBACDectination(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 uint8_t *tmp = &data[len];

 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_DESTINATION *dest = new BACNET_DESTINATION;

        len += gDec->decode_destination(&data[len],BACNET_MAX_APDU_IP,dest);
        property->complexValue.vDestinationList.push_back(dest);
	   }
 len++; // preskoci zatvarajuci tag

 return len;
}








//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ COMPLEX: DEVICE OBJECT REFERENCE ]==============================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeDeviceObjectReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }



 // TODO: provjera dobrog kodiranja;
 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_DEVICE_OBJECT_REFERENCE *ref = new BACNET_DEVICE_OBJECT_REFERENCE;
	    len += gDec->bacapp_decode_device_obj_ref(&data[len],ref);
        property->complexValue.vDeviceObjectReference.push_back(ref);
	   }

 len++; // preskoci zatvarajuci tag

 return len;
}
//_____________________________________________________________________________________\\





BACAPI_ERROR I_cBACServiceHandler::DecodeBACnetActionList(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 *ret_len = len;
	 return BACAPI_ERROR_INVALID_OPENING_TAG;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

 while (!gDec->decode_is_closing_tag_number(&apdu[len],4) && !gDec->decode_is_closing_tag_number(&apdu[len],3))
       {
        BACNET_ACTION_LIST *new_list = new BACNET_ACTION_LIST;


        ret = gDec->decode_action_command_list(&apdu[len], new_list);
        if (ret < 0)
           {
            // izbrisi zadnju vrijednost na kojoj se pojavio error.
            BACAPI_DeleteActionCommandList(new_list);
            *ret_len = len;
            return BACAPI_ERROR_MALFORMED_PACKET;
           }

        property->complexValue.vBACnetActionList.push_back(new_list);
				len += ret;
	   }

 uint8_t *tmp_pos = &apdu[len];


 // ako je sljedeci tag zatvarajuci tag vrati OK
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

 *ret_len = len;
 return BACAPI_ERROR_INVALID_CLOSING_TAG;
}

//=====================================================================================//
//===================[ COMPLEX: ACTION LIST ]==========================================//
// deprecated
//=====================================================================================//
int I_cBACServiceHandler::DecodeBACnetActionList(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }

 while (!gDec->decode_is_closing_tag_number(&data[len],4) || !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_ACTION_LIST *new_list = new BACNET_ACTION_LIST;

        len += gDec->decode_action_command_list(&data[len], new_list);

        property->complexValue.vBACnetActionList.push_back(new_list);
	   }

 uint8_t *tmp_pos = &data[len];
 len++; // preskoci closing tag

 return len;
}
//_____________________________________________________________________________________\\






//=====================================================================================//
//===================[ COMPLEX: ACTION TEXT ]==========================================//
// TODO: vector
//=====================================================================================//
int I_cBACServiceHandler::DecodeActionText(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }

 BACNET_ACTION_TEXT *head = NULL;
 property->complexValue.actionText = NULL;


    int tag_len = 0;
    uint8_t tag_number = 0;
    uint32_t len_value_type = 0;

 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        property->complexValue.actionText = new BACNET_ACTION_TEXT;



        //len += gDec->decode_action_command_list(&data[len], property->complexValue.actionCommandList);

		//gDec->decode_character_string(&data[len],

		len +=  gDec->decode_tag_number_and_value(&data[len], &tag_number, &len_value_type);
		len += gDec->bacapp_decode_data(&data[len],tag_number,len_value_type,&property->complexValue.actionText->value);

        property->complexValue.actionText->next = head;
        head = property->complexValue.actionText;
	   }

 len++; // preskoci closing tag
 return len;
}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ COMPLEX: CALENDAR ENTRY ]=======================================//
// calendar
//=====================================================================================//
int I_cBACServiceHandler::DecodeCalendarEntry(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_CALENDAR_ENTRY *new_entry = new BACNET_CALENDAR_ENTRY;

        len += gDec->decode_calendar_entry(&data[len],BACNET_MAX_APDU_IP,new_entry);

        property->complexValue.vCalendarEntryList.push_back(new_entry);
	   }

 // preskoci closing tag [4F]
 len++;

 return len;
}
//_____________________________________________________________________________________\\






//=====================================================================================//
//===================[ COMPLEX: READ ACCESS SPEC ]=====================================//
// group object type
// TODO: vector
//=====================================================================================//
int I_cBACServiceHandler::DecodeReadAccessSpecification(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 BACNET_READ_ACCESS_SPECIFICATION *head = NULL;
 property->complexValue.readAccessSpec = NULL;


 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }



 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        property->complexValue.readAccessSpec = new BACNET_READ_ACCESS_SPECIFICATION;

        // postavi sve property Id e na EMPTY_PROPERTY_REFERENCE_ID
        for (int c=0;c<MAX_LIST_OF_PROPERTY_REFERENCES;c++)
            {
             property->complexValue.readAccessSpec->listOfPropertyReferences[c].propertyIdentifier = EMPTY_PROPERTY_REFERENCE_ID;
            }

        len += gDec->decode_read_access_specification(&data[len],BACNET_MAX_APDU_IP,property->complexValue.readAccessSpec);


        property->complexValue.readAccessSpec->next = head;
        head = property->complexValue.readAccessSpec;
	   }

 // preskoci closing tag [4F]
 len++;


 return len;
}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ COMPLEX: GROUP PRESENT VALUE]===================================//
//=====================================================================================//
//  TODO: dekodiranje vrijednosti
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeReadAccessResult(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;

 // decode opening tag
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 *ret_len = len;
	 return BACAPI_ERROR_INVALID_OPENING_TAG;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 // NOTE ovo je wip funkcija, stvori vrijednost kao octet_string i prebaci podatke u ostr buffer
 BACNET_APPLICATION_DATA_VALUE *val = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_OSTRING);
 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        val->type.Octet_String.vValue.push_back(data[len]);
        len++;
       }

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

uint8_t *tmp1 = &data[len];
len++; // closing tag
return BACAPI_OK;
}



//=====================================================================================//
//===================[ COMPLEX: SET POINT OBJECT PROPERTY REFERENCE ]==================//
// NOTE: Pomocna decode funkcija jer PROP_SETPOINT_REF ima jeda tag viska
//=====================================================================================//
int I_cBACServiceHandler::DecodeSetPointObjectPropertyReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_OBJECT_PROPERTY_REFERENCE *ref = new BACNET_OBJECT_PROPERTY_REFERENCE;
        uint8_t *tmp = &data[len];

        bool ctxSetPointRef = gDec->decode_is_opening_tag_number(&data[len],0);
        if (ctxSetPointRef)
           {
            len++;
	        len += gDec->bacapp_decode_obj_property_ref(&data[len],ref);
            property->complexValue.vObjectPropertyReference.push_back(ref);
           }
        len++;  // closing context tag [0f]
	   }

 len++; // preskoci zatvarajuci tag [4f]

 return len;

}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ PRIMITIVE: BAC ]=================================//

//=====================================================================================//
/*
int I_cBACServiceHandler::DecodeBACnetAction(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;


 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag



 // mozda ne postoje upisane vrijednosti, provjeri da li je sljedeci tag zatvarajuci tag
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     // sve je u redu, u ovom propertiju ne postoje vrijednosti
     len++; // preskoci zatvarajuci tag i vrati duzinu
     return len;
    }

 uint8_t tag = 0;
 uint32_t val = 0;
 uint32_t lenval = 0;
 len += gDec->decode_tag_number_and_value(&data[len],&tag,&lenval);
 len += gDec->decode_enumerated(&data[len],lenval,&val);

 BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue((BACNET_APPLICATION_TAG)tag); // enum u ovom slucaju
 value->type.Enumerated = val;

 // preskoci closing
 len++;


 return len;
}
*/
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ COMPLEX: SCHEDULE PRESENT_VALUE i SCHEDULE_DEFAULT PROPERTY ]===//
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeScheduleAnyValue(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 uint8_t tag = 0;
 uint32_t len_val = 0;
 uint32_t val = 0;
 int len = 0;
 int ret = 0;


 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 *ret_len = len;
	 return BACAPI_ERROR_INVALID_OPENING_TAG;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

 // dekodiraj present value
 ret = gDec->decode_tag_number_and_value(&apdu[len],&tag,&len_val);
 if (ret < 0)
    {
     return BACAPI_ERROR_INVALID_TAG;
    }
 len += ret;

 // stvori vrijednost s ovim tagom
 BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue((BACNET_APPLICATION_TAG)tag);

 switch (tag)
        {
         case BACNET_APPLICATION_TAG_NULL:
         {
          // ova null vrijednost se preskocila automatski u decode_tag_number_and_value() funkciji
          break;
         }
         case BACNET_APPLICATION_TAG_REAL:
          len += gDec->decode_real(&apdu[len],&value->type.Real);
          property->vValueList.push_back(value);
          break;
		 case BACNET_APPLICATION_TAG_BOOL:
		  value->type.Boolean = gDec->decode_boolean(len_val);
		  property->vValueList.push_back(value);
		  len++;
          break;
		 case BACNET_APPLICATION_TAG_UINT:
		  len += gDec->decode_unsigned(&apdu[len],len_val,&value->type.Unsigned_Int);
		  property->vValueList.push_back(value);
		  break;
		 case BACNET_APPLICATION_TAG_SINT:
		  len += gDec->decode_signed(&apdu[len],len_val,&value->type.Signed_Int);
		  property->vValueList.push_back(value);
          break;
		 case BACNET_APPLICATION_TAG_DOUBLE:
		  len += gDec->decode_double(&apdu[len],&value->type.Double);
		  property->vValueList.push_back(value);
          break;
		 case BACNET_APPLICATION_TAG_CSTRING:
		  // TODO: UTF8 / UTF16 ???
		  //len += gDec->decode_standard_character_string(&apdu[len],len_val,&present_value->type.character_String.value8);
		  len += gDec->decode_standard_character_string(&apdu[len],len_val,value->type.character_String.value8);
		  property->vValueList.push_back(value);
          break;
		 case BACNET_APPLICATION_TAG_ENUM:
		  len += gDec->decode_enumerated(&apdu[len],len_val,&value->type.Enumerated);
		  property->vValueList.push_back(value);
          break;

         default:
          // vrijednost nije podrzana, izbrisi sve i vrati -1
          BACAPI_DeleteApplicationDataValue(value);
          *ret_len = len;
          return BACAPI_ERROR_UNSUPPORTED_APPLICATION_TAG;
         }


 // sljedeci tag mora biti zatvarajuci tag
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
	{
     len++;
    }else {
		   *ret_len = len;
		   return BACAPI_ERROR_INVALID_CLOSING_TAG;
          }

 *ret_len = len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ COMPLEX: TIME STAMPS ]==========================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeEventTimeStamps(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;


 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_TIMESTAMP *new_stamp = new BACNET_TIMESTAMP;
        uint8_t *tmp = &data[len];
        len += gDec->bacapp_decode_timestamp(&data[len],new_stamp,property->property);
        property->complexValue.vTimeStamps.push_back(new_stamp);
       }

 // preskoci zatvori tag
 len++;

 return len;

}
//_____________________________________________________________________________________\\






//=====================================================================================//
//===================[ COMPLEX: DEVICE OBJECT PROP REF ]===============================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeDeviceObjectPropertyReference(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;


 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        uint8_t *tmp = &data[len];
        BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *new_reference = new BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE;
        len += gDec->bacapp_decode_device_obj_property_ref(&data[len],new_reference);
        property->complexValue.vDeviceObjectPropertyReference.push_back(new_reference);
       }


 len++;   // [4f] / [3F]

 return len;
}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ COMPLEX: ENUMERATION LISTS ]====================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeEnumList(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 uint8_t tag = 0;
 uint32_t val = 0;
 uint32_t lenval = 0;


 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_ENUM);

		len += gDec->decode_tag_number_and_value(&data[len],&tag,&lenval);
		len += gDec->decode_enumerated(&data[len],lenval,&val);

		value->type.Enumerated = val;
        property->vValueList.push_back(value);
       }

 uint8_t *tmp = &data[len];

 // preskoci closing
 len++;


 return len;
}
//_____________________________________________________________________________________\\






//=====================================================================================//
//===================[ COMPLEX: EVENT PARAMETERS ]=====================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeEventParameters(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 len += gDec->decode_event_parameters(&data[len],&property->complexValue.eventParam);


 len++;

 uint8_t *tmp = &data[len];
 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ COMPLEX: LOG BUFFER ]===========================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeLogBuffer(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int ret = 0;
 int len = 0;
 uint8_t tag = 0;
 uint32_t len_val = 0;
 BACNET_ERROR bac_err;

 // Da li smo naisli na Property Access Error?
 if (IsPropertyAccessError(data,&bac_err,&len))
    {
     property->propertyAccessError = true;
     return len;
    }

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 //
 if (gDec->decode_is_opening_tag_number(&data[len],0))   // [0] timestamp
	{
     len++;
	 BACNET_EVENT_LOG_RECORD *rec = new BACNET_EVENT_LOG_RECORD;
	 ret = gDec->decode_application_date(&data[len],&rec->timeStamp.date);
	 if (ret<0) return ret;

	 len+=ret;

	 ret = gDec->decode_application_date(&data[len],&rec->timeStamp.date);
	 if (ret < 0) return ret;

	 len+=ret;

	 ret = gDec->decode_application_time(&data[len],&rec->timeStamp.time);
	 if (ret < 0) return ret;

	 len++;  // closing tag [0F]

     
	 if (gDec->decode_is_opening_tag_number(&data[len],1))  // // [1] logDatum
		{
		 len++;

		 gDec->decode_tag_number_and_value(&data[len], &tag, &len_val);
         rec->type = tag;

         switch (tag)
		        {
				 case 0: break; // log status - ne hendlamo
				 case 1: // TODO: ConfirmedEventNotification ret?
				  break;
				 case 2: len += gDec->decode_context_real(&data[len],9,&rec->logDatum.time_change); break;
				 default:
				 return -1;
		        }
         len++;  // closing tag [1F]
         property->complexValue.vEventLogRecordList.push_back(rec);
		}
	 len++;  // closing [1]
    }

 len++;

 return len;

}
//_____________________________________________________________________________________\\





//=====================================================================================//
//===================[ COMPLEX: SCALE ]================================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeScale(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }



 len += gDec->decode_scale(&data[len],&property->complexValue.scale);

 uint8_t *tmp = &data[len];

 len++;   // [4F]
 return len;
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//===================[ COMPLEX: PRE SCALE ]============================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodePreScale(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }


 len += gDec->decode_pre_scale(&data[len],&property->complexValue.preScale);

 uint8_t *tmp = &data[len];

 len++;   // [4F]
 return len;
}
//_____________________________________________________________________________________\\



//=====================================================================================//
//===================[ COMPLEX: SHED LEVEL ]===========================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeShedLevel(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;


 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }
 len += gDec->decode_shed_level(&data[len],&property->complexValue.shedLevel);


 len++;

 return len;
}
//_____________________________________________________________________________________\\








//=====================================================================================//
//===================[ COMPLEX: RECIPIENT LIST ]=======================================//
//=====================================================================================//
int I_cBACServiceHandler::DecodeRecipient(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

 // ovo mora biti otvarajuci tag [3] ili [4]
 if (!gDec->decode_is_opening_tag_number(&data[len],4) && !gDec->decode_is_opening_tag_number(&data[len],3))
	{
	 return -1; // bad coding
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&data[len],4) || gDec->decode_is_closing_tag_number(&data[len],3))
    {
     len++; // preskoci zatvarajuci tag
     return len;
    }



 while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
       {
        int ret = 0;
        BACNET_RECIPIENT *new_recipient = new BACNET_RECIPIENT;
        ret = gDec->decode_recipient(&data[len],new_recipient);
        if (ret <= 0)  // moguce je da ovaj properti sadrzi app_tag NULL, u kojem slucaju ce
           {           // gDec->decode_recipient vratiti -1
            //prevrti do kraja i vrati len
            delete new_recipient;
            while (!gDec->decode_is_closing_tag_number(&data[len],4) && !gDec->decode_is_closing_tag_number(&data[len],3))
                  {
                   len++;
                  }
           }else {
                  len += ret;
                  property->complexValue.vRecipientList.push_back(new_recipient);
                 }
       }

 len++; // preskoci zatvarajuci tag
 return len;
}



BACAPI_ERROR I_cBACServiceHandler::DecodeAlarmSummary(uint8_t *data, BACNET_ALARM *alarm, int payloadLen, int *ret_len)
{
 int len = 0;
 uint8_t tagNum = 0;
 uint32_t lenVal = 0;
 uint32_t value = 0;

 while (len < payloadLen)
       {
        BACNET_ALARM_SUMMARY as;

        len += gDec->decode_tag_number_and_value(&data[len],&tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_OID)
           {
            len += gDec->decode_object_id(&data[len],&as.oid.type,&as.oid.instance);
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);    // alarm state
        if (tagNum == BACNET_APPLICATION_TAG_ENUM)
           {
            len += gDec->decode_enumerated(&data[len],lenVal,&value);
            as.alarmState = (BACNET_EVENT_STATE)value;
           }else return BACAPI_ERROR_MALFORMED_PACKET;


        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);    // ack trans
        if (tagNum == BACNET_APPLICATION_TAG_BSTRING)
           {
            len += gDec->decode_bitstring(&data[len],lenVal,&as.bstrTransBits);
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        alarm->vAlarmSummary.push_back(as);
       }

 *ret_len = len;
 return BACAPI_OK;
}



BACAPI_ERROR I_cBACServiceHandler::DecodeEnrollmentSummary(uint8_t *data, BACNET_ENROLLMENT_SUMMARY *summary, int apdu_len, int *ret_len)
{
 int len = 0;
 uint8_t tagNum = 0;
 uint32_t lenVal = 0;
 uint32_t value = 0;

 while (len < apdu_len)
       {
        ENROLLMENT_SUMMARY summ;

        // oid
        len += gDec->decode_tag_number_and_value(&data[len],&tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_OID)
           {
            len += gDec->decode_object_id(&data[len],&summ.oid.type,&summ.oid.instance);
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        // event type
        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_ENUM)
           {
            len += gDec->decode_enumerated(&data[len],lenVal,&value);
            summ.event_type = (BACNET_EVENT_TYPE)value;
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        // event state
        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_ENUM)
           {
            len += gDec->decode_enumerated(&data[len],lenVal,&value);
            summ.event_state = (BACNET_EVENT_STATE)value;
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        // priority
        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_UINT)
           {
            len += gDec->decode_unsigned(&data[len],lenVal,&value);
            summ.priority = (uint16_t)value;
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        // notification class - optional
        int ret = gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_UINT) // postoji notification class
           {
            len += ret;
            len += gDec->decode_unsigned(&data[len],lenVal, &value);
            summ.notification_class = (uint16_t)value;
           }
        uint8_t *tmp = &data[len];
        summary->vEnrollmentSummary.push_back(summ);
       }

 return BACAPI_OK;
}


//==========================================================================================================//
//================================[ DECODE EVENT INFO ]=====================================================//
// TODO:
// bacapp_decode_timestamp Koristi MAX_ASHRAE_PROPERTY_COUNT kao ulazni parametar. Zasto?!!
//==========================================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeEventInformation(uint8_t *data, BACNET_EVENT_INFORMATION *info, int apdu_len, int *ret_len)
{
 int len = 0,ret = 0;
 uint8_t tagNum = 0;
 uint32_t lenVal = 0;
 uint32_t value = 0;

 // opening tag [0E]
 if (gDec->decode_is_opening_tag_number(&data[len],0) == false)
     return BACAPI_ERROR_MALFORMED_PACKET;

 len++;

 while (gDec->decode_is_closing_tag_number(&data[len],0) == false && len < apdu_len)
       {
        EVENT_INFORMATION ei;

        // oid
        ret = gDec->decode_context_object_id(&data[len],0,&ei.oid.type,&ei.oid.instance);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        gLog->AppendLog("DecodeEventInformation: oid %s, %d",BACAPI_BACnetObjectName(ei.oid.type),ei.oid.instance);
        len += ret;

        // event state
        ret = gDec->decode_context_enumerated(&data[len],1,&value);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        ei.eventState = (BACNET_EVENT_STATE)value;
        len += ret;

        // ack trans
        ret = gDec->decode_context_bitstring(&data[len],2,&ei.bstrAckTrans);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        len += ret;

        // time stamp [3E]
        if (gDec->decode_is_opening_tag_number(&data[len],3) == false)
            return BACAPI_ERROR_MALFORMED_PACKET;

        len++;


        BACNET_TIMESTAMP tmp;
        gDec->bacapp_decode_timestamp(&data[len],&tmp,BACNET_PROPERTY_COUNT);

        // ts 1
        ret = gDec->bacapp_decode_timestamp(&data[len],&ei.eventTimeStamps[0],BACNET_PROPERTY_COUNT);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        len+= ret;

        // ts 2
        ret = gDec->bacapp_decode_timestamp(&data[len],&ei.eventTimeStamps[1],BACNET_PROPERTY_COUNT);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        len+= ret;

        // ts 3
        ret = gDec->bacapp_decode_timestamp(&data[len],&ei.eventTimeStamps[2],BACNET_PROPERTY_COUNT);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        len+= ret;

        // time stamp [3F]
        if (gDec->decode_is_closing_tag_number(&data[len],3) == false)
            return BACAPI_ERROR_MALFORMED_PACKET;

        len++;

        // notify type
        ret = gDec->decode_context_enumerated(&data[len],4,&value);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        ei.notifyType = (BACNET_NOTIFY_TYPE)value;
        len += ret;


        // event enable
        ret = gDec->decode_context_bitstring(&data[len],5,&ei.bstrEventEnable);
        if (ret < 0) return BACAPI_ERROR_MALFORMED_PACKET;

        len += ret;

        // priorities [6E]
        if (gDec->decode_is_opening_tag_number(&data[len],6) == false)
            return BACAPI_ERROR_MALFORMED_PACKET;

        len++;

        // prio 1
        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_UINT)
           {
            len += gDec->decode_unsigned(&data[len],lenVal,&value);
            ei.eventPriorities[0] = (uint16_t)value;
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        // prio 2
        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_UINT)
           {
            len += gDec->decode_unsigned(&data[len],lenVal,&value);
            ei.eventPriorities[1] = (uint16_t)value;
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        // prio 3
        len += gDec->decode_tag_number_and_value(&data[len], &tagNum, &lenVal);
        if (tagNum == BACNET_APPLICATION_TAG_UINT)
           {
            len += gDec->decode_unsigned(&data[len],lenVal,&value);
            ei.eventPriorities[2] = (uint16_t)value;
           }else return BACAPI_ERROR_MALFORMED_PACKET;

        // priorities [6F]
        if (gDec->decode_is_closing_tag_number(&data[len],6) == false)
            return BACAPI_ERROR_MALFORMED_PACKET;

        len++;


        info->vEventInformationList.push_back(ei);
       }

 // closing [0F]
 //if (gDec->decode_is_closing_tag_number(&data[len],0) == false)
 //    return BACAPI_ERROR_MALFORMED_PACKET;

 len++;

 uint8_t *pos = &data[len];

 gDec->decode_context_boolean2(&data[len],1,&info->more_events);
 return BACAPI_OK;
}



int I_cBACServiceHandler::DecodeAuthenticationPolicyList(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 uint32_t val;



 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {

        len++;

        BACNET_AUTHENTICATION_POLICY pol;

        if (gDec->decode_is_opening_tag_number(&data[len],0))
           {
            len++;
            BACNET_POLICY policy;
            len++;
            len += gDec->bacapp_decode_device_obj_ref(&data[len],&policy.credential_data_input);
            len += gDec->decode_context_unsigned(&data[len],1,&val);
            len++;
            policy.index = val;
           }


        if (!gDec->decode_is_closing_tag_number(&data[len],0))
           {
            return -1;
           }


        len++;



      len += gDec->decode_context_boolean2(&data[len], 1, &pol.order_enforced);
      len += gDec->decode_context_unsigned(&data[len],2,&val);
      pol.timeout = val;
      uint8_t *pos = &data[len];
      len++;
     }

   //uint8_t *pos = &data[len];
 // preskoci zatvarajuci tag
 len++;

 return len;
}



BACAPI_ERROR I_cBACServiceHandler::DecodeUnconfirmedCOVNotification(uint8_t *data, BACNET_COV_NOTIFICATION *cov, int payloadLen, int *ret_len)
{

 int len = 0;
 int ret = 0;
 uint32_t prop = 0;
 uint32_t sec;
 uint8_t *pos = 0;
 uint32_t value = 0;

 pos = &data[len];

 // processid
 ret = gDec->decode_context_unsigned(&data[len],0,&cov->processId);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;

 // device
 ret =  gDec->decode_context_object_id(&data[len],1,&cov->device.type,&cov->device.instance);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;

 // object
 ret = gDec->decode_context_object_id(&data[len],2,&cov->object.type,&cov->object.instance);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;


 // sec remain
 ret = gDec->decode_context_unsigned(&data[len], 3, &cov->seconds);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;

 if (gDec->decode_is_opening_tag_number(&data[len],4) == false)
    {
     return BACAPI_ERROR_MALFORMED_PACKET;
    }
 len++;

 BACNET_COV_VALUE *head = NULL;

 // preuzmi cijlu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true)
       {


        BACNET_COV_VALUE cov_val;

        // property - context [0]
        ret = gDec->decode_context_enumerated(&data[len], 0,&prop);
        if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

        len += ret;
        cov_val.property = (BACNET_PROPERTY_ID)prop;

        if (gDec->decode_is_context_tag(&data[len],1))// array index
           {
            ret = gDec->decode_context_unsigned(&data[len],1,&value);
            if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET; // array index postoji ali nije proslo dekodiranje

            len += ret;
            // TODO: prio = value
           }

        // value - context [2] - open
        if (gDec->decode_is_opening_tag_number(&data[len],2) == false)
           {
            return BACAPI_ERROR_MALFORMED_PACKET;
           }

        len++;

        cov_val.value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL); // nije vazno koji tag


		len += gDec->bacapp_decode_application_data(&data[len],payloadLen,cov_val.value);

        if (gDec->decode_is_closing_tag_number(&data[len],2) == false)
           {
            BACAPI_DeleteApplicationDataValue(cov_val.value);
            return BACAPI_ERROR_MALFORMED_PACKET;
           }
        len++;


        pos = &data[len];
        // priority?
        if (gDec->decode_is_context_tag(&data[len],3))// priority
           {
            ret = gDec->decode_context_unsigned(&data[len],3,&value);
            if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

            len += ret;
            // TODO: priority = value;
           }

        cov->vValueList.push_back(cov_val);

       }

 len++; // closing tag

 *ret_len = len;
 return BACAPI_OK;
}





BACAPI_ERROR I_cBACServiceHandler::DecodeConfirmedCOVNotification(uint8_t *data, BACNET_COV_NOTIFICATION *cov, int payloadLen, int *ret_len)
{
 int len = 0;
 int ret = 0;
 uint32_t prop = 0;
 uint32_t sec;
 uint8_t *pos = 0;
 uint32_t value = 0;

 pos = &data[len];

 // processid
 ret = gDec->decode_context_unsigned(&data[len],0,&cov->processId);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;

 // device
 ret =  gDec->decode_context_object_id(&data[len],1,&cov->device.type,&cov->device.instance);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;

 // object
 ret = gDec->decode_context_object_id(&data[len],2,&cov->object.type,&cov->object.instance);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;


 // sec remain
 ret = gDec->decode_context_unsigned(&data[len], 3, &cov->seconds);
 if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

 len += ret;

 if (gDec->decode_is_opening_tag_number(&data[len],4) == false)
    {
     return BACAPI_ERROR_MALFORMED_PACKET;
    }
 len++;

 BACNET_COV_VALUE *head = NULL;

 // preuzmi cijlu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true)
       {


        BACNET_COV_VALUE cov_val;

        // property - context [0]
        ret = gDec->decode_context_enumerated(&data[len], 0,&prop);
        if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

        len += ret;
        cov_val.property = (BACNET_PROPERTY_ID)prop;

        if (gDec->decode_is_context_tag(&data[len],1))// array index
           {
            ret = gDec->decode_context_unsigned(&data[len],1,&value);
            if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET; // array index postoji ali nije proslo dekodiranje

            len += ret;
            // TODO: prio = value
           }

        // value - context [2] - open
        if (gDec->decode_is_opening_tag_number(&data[len],2) == false)
           {
            return BACAPI_ERROR_MALFORMED_PACKET;
           }

        len++;

        cov_val.value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL); // nije vazno koji tag


		len += gDec->bacapp_decode_application_data(&data[len],payloadLen,cov_val.value);

        if (gDec->decode_is_closing_tag_number(&data[len],2) == false)
           {
            BACAPI_DeleteApplicationDataValue(cov_val.value);
            return BACAPI_ERROR_MALFORMED_PACKET;
           }
        len++;


        pos = &data[len];
        // priority?
        if (gDec->decode_is_context_tag(&data[len],3))// priority
           {
            ret = gDec->decode_context_unsigned(&data[len],3,&value);
            if (ret <= 0) return BACAPI_ERROR_MALFORMED_PACKET;

            len += ret;
            // TODO: priority = value;
           }

        cov->vValueList.push_back(cov_val);

       }

 len++; // closing tag

 *ret_len = len;
 return BACAPI_OK;
}









// TODO:
// za sada samo prevrti do zatvarajuceg taga
int I_cBACServiceHandler::DecodeActiveVTSessions(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        len++;
       }
 // preskoci zatvarajuci tag
 len++;

 return len;
}


//  TODO: ovo mora biti lista
int I_cBACServiceHandler::DecodeLoggingRecord(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int ret = 0;
 int len = 0;
 BACNET_ACCUMULATOR_RECORD *rec = &property->complexValue.accuRecord;

 if (gDec->decode_is_opening_tag_number(&data[len],4))
    {
     len++;  // preskoci otvarajuci tag

     len++; // pretpostavi da je sve dobro kodirano: TODO: errors (tag [0])
     len += gDec->decode_application_datetime(&data[len],&rec->timeStamp);
     len++; // zatvarajuci tag [0] za datetime

     len += gDec->decode_context_unsigned(&data[len],1,&rec->presentValue);
     len += gDec->decode_context_unsigned(&data[len],2,&rec->accumulatedValue);
     len += gDec->decode_context_enumerated(&data[len],3,&rec->accumulatedValue);
    }
 len++; // preskoci zatvarauci tag
 return len;
}



// TODO:
int I_cBACServiceHandler::DecodeFaultParameters(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        len++;
       }
 // preskoci zatvarajuci tag
 len++;

 return len;
}



int I_cBACServiceHandler::DecodeBACnetAccessRule(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;
 
 if (gDec->decode_is_opening_tag_number(&data[len],4) == true || gDec->decode_is_opening_tag_number(&data[len],3) == true)
     len++; 
     else return 0; // TODO: MALFORMED PACKET
     
     
  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        len++;
        BACNET_ACCESS_RULE rule;

        len += gDec->decode_context_enumerated(&data[len],0,&rule.timeRangeSpecifier);

		if (rule.timeRangeSpecifier == 0)  // postoji time range
		   {
			len += gDec->bacapp_decode_device_obj_property_ref(&data[len],&rule.timeRange);
		   }

      len += gDec->decode_context_enumerated(&data[len],2,&rule.locationSpecifier);
      if (rule.locationSpecifier == 0)
         {
          len += gDec->bacapp_decode_device_obj_ref(&data[len],&rule.location);
         }
      len += gDec->decode_context_boolean2(&data[len],4,&rule.enable);



       }
 // preskoci zatvarajuci tag
 len++;

 return len;
}



/*
BACnetCredentialAuthenticationFactor
 ::= SEQUENCE {
   disable [0] BACnetAccessAuthenticationFactorDisable,
   authentication-factor [1] BACnetAuthenticationFactor
}
*/
BACAPI_ERROR I_cBACServiceHandler::DecodeCredentialAuthenticationFactors(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;
 uint32_t val32 = 0;
 uint8_t *pos = NULL;
 uint8_t tag = 0;

 // Mora postojati otvarajuci tag [3]/[4]
 if (gDec->decode_is_opening_tag_number(&data[len],4) == true || gDec->decode_is_opening_tag_number(&data[len],3) == true)
     len++; 
     else return BACAPI_ERROR_MALFORMED_PACKET;


  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        BACNET_CREDENTIAL_AUTH_FACTOR fact;
        
        // disable [0] BACnetAccessAuthenticationFactorDisable,
        ret = gDec->decode_context_enumerated(&data[len],0,&val32);
	    if (ret == 0 || ret < 0)
		   return BACAPI_ERROR_MALFORMED_PACKET;   
		   
		fact.disable = (BACNET_ACCESS_AUTH_FACTOR_DISABLE)val32;
		len += ret;
		
		pos = &data[len];
		

        //authentication-factor [1] BACnetAuthenticationFactor
		if (gDec->decode_is_opening_tag_number(&data[len],1))
		   {
		    len++; // preskoci otvarajuci tag [1]
		    
		    pos = &data[len];
		    
			// factor type [0]
			ret = gDec->decode_context_enumerated(&data[len],0,&val32);
			if (ret == 0 || ret < 0)
			   return BACAPI_ERROR_MALFORMED_PACKET;   
			
			fact.auth_factor.factor_type = (BACNET_AUTHENTICATION_FACTOR_TYPE)val32;
            len += ret;
            
            pos = &data[len];
            
            // format class [1]
            ret = gDec->decode_context_unsigned(&data[len],1,&val32);
			if (ret == 0 || ret < 0)
			   return BACAPI_ERROR_MALFORMED_PACKET;   
		    
            fact.auth_factor.format_class = (uint16_t)val32;
            len += ret;
            
            BACNET_OCTET_STRING ostr;
            ret = gDec->decode_context_octet_string(&data[len],2,&fact.auth_factor.ostr);
			if (ret == 0 || ret < 0)
			   return BACAPI_ERROR_MALFORMED_PACKET;   
			   
			len += ret;
			
			len++;  // preskoci zatvarajuci tag [1]
		   }
		   
		property->complexValue.vCredentialAuthFactorList.push_back(fact);
       }
 
 pos = &data[len];
 len++; // preskoci zatvarajuci tag [3] [4]
 *ret_len = len;
 return BACAPI_OK;
}


BACAPI_ERROR I_cBACServiceHandler::DecodeAssignedAccessRights(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;
 uint8_t *pos = NULL;

 // Mora postojati otvarajuci tag [3]/[4]
 if (gDec->decode_is_opening_tag_number(&data[len],4) == true || gDec->decode_is_opening_tag_number(&data[len],3) == true)
     len++; 
     else return BACAPI_ERROR_MALFORMED_PACKET;
     
     
  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        uint8_t tag = 0;
        uint32_t val32 = 0;
        pos = &data[len];
        BACNET_ASSIGNED_ACCESS_RIGHTS ar;
        
        // [0] 
        // dev.oid
        // obj.oid
        // [/0]
        // bool
        
        // BACnetDeviceObjectReference
        if (gDec->decode_is_opening_tag_number(&data[len],0))
           {
            len++;
            //gDec->bacapp_decode_device_obj_ref(
           
           
			ret = gDec->decode_context_object_id(&data[len],0,&ar.ref.deviceIndentifier.type,&ar.ref.deviceIndentifier.instance);
			if (ret == 0 || ret < 0)
			   return BACAPI_ERROR_MALFORMED_PACKET;
               
			len += ret;
            
			ret = gDec->decode_context_object_id(&data[len],1,&ar.ref.objectIdentifier.type,&ar.ref.objectIdentifier.instance);
			if (ret == 0 || ret < 0)
			   return BACAPI_ERROR_MALFORMED_PACKET;
            
            
			len += ret;   
			
			uint8_t *pos = &data[len];
	        len++;  // preskoci zatvarajuci tag [0]
	       }
	     
	     // boolean   
	     len += gDec->decode_context_boolean2(&data[len],1,&ar.enable);
	     property->complexValue.vAssignedAccessRights.push_back(ar);
	     printf("");
		}
      

 pos = &data[len];
 len++;  // preskoci zatvarajuci tag [3]/[4]
 *ret_len = len;
 pos = &data[len];
 return BACAPI_OK;
}


BACAPI_ERROR I_cBACServiceHandler::DecodeNetworkAccessSecurityPolicies(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;
 uint32_t val32;
 uint8_t *pos = NULL; 
 

 // Mora postojati otvarajuci tag [3]/[4]
 if (gDec->decode_is_opening_tag_number(&data[len],4) == true || gDec->decode_is_opening_tag_number(&data[len],3) == true)
     len++; 
     else return BACAPI_ERROR_INVALID_TAG; 
 
 
  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        BACNET_NETWORK_SECURITY_POLICY policy;
 
        uint8_t tag = 0;
        uint32_t val32 = 0;
        pos = &data[len];
        
        // port_id [0]
        ret = gDec->decode_context_unsigned(&data[len],0,&val32);
        if (ret <= 0)
           return BACAPI_ERROR_INVALID_TAG;
       
        policy.port_id = (uint8_t)val32;
        len += ret;
         
        ret = gDec->decode_context_enumerated(&data[len],1,&val32);
        if (ret <= 0)
           return BACAPI_ERROR_INVALID_TAG;
           
        
        policy.sec_level = (BACNET_SECURITY_POLICY)val32;
        len += ret;
        
        property->complexValue.vSecurityPolicyList.push_back(policy);
       }
        
 pos = &data[len];
 len++;
 return BACAPI_OK;
}


BACAPI_ERROR I_cBACServiceHandler::DecodeSecurityKeySets(uint8_t *data, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;
 int ret = 0;
 uint32_t val32;
 uint8_t *pos = NULL;

 // Mora postojati otvarajuci tag [3]/[4]
 if (gDec->decode_is_opening_tag_number(&data[len],4) == true || gDec->decode_is_opening_tag_number(&data[len],3) == true)
     len++; 
     else return BACAPI_ERROR_INVALID_TAG;
     
     
  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        BACNET_SECURITY_KEY_SET set;
 
        uint8_t tag = 0;
        uint32_t val32 = 0;
        pos = &data[len];
        
        // key-revision [0]
        ret = gDec->decode_context_unsigned(&data[len],0,&val32);
        if (ret <= 0)
           return BACAPI_ERROR_INVALID_TAG;
       
        set.key_revision = (uint8_t)val32;
        len += ret;
        
        
        // activation time [1]
        if (gDec->decode_is_opening_tag_number(&data[len],1))
           { 
            len++;
            BACNET_DATE_TIME datetime;
            
            ret = gDec->decode_application_datetime(&data[len],&set.activation_time);
            if (ret <= 0)
              return BACAPI_ERROR_MALFORMED_PACKET;
            
            len += ret;
            
            pos = &data[len];
            if (!gDec->decode_is_closing_tag_number(&data[len],1))
               return BACAPI_ERROR_INVALID_TAG;

            len++; // zatvarajuci tag [1]               
           }else return BACAPI_ERROR_INVALID_TAG;
              
        // expiration time [2]
        if (gDec->decode_is_opening_tag_number(&data[len],2))
           { 
            len++;
            BACNET_DATE_TIME datetime;
            
            ret = gDec->decode_application_datetime(&data[len],&set.expiration_time);
            if (ret <= 0)
              return BACAPI_ERROR_MALFORMED_PACKET;
            
            len += ret;
            
            pos = &data[len];
            if (!gDec->decode_is_closing_tag_number(&data[len],2))
               return BACAPI_ERROR_INVALID_TAG;

            len++; // zatvarajuci tag [1]               
           }else return BACAPI_ERROR_INVALID_TAG;               
                     
         // SEQUENCE of BACnetKeyIndentifier [3]
        if (gDec->decode_is_opening_tag_number(&data[len],3))
           { 
            len++;
            
            while (!gDec->decode_is_closing_tag_number(&data[len],3))
                  {
                   BACNET_KEY_IDENTIFIER key;
                   // algorithm [0]
                   ret = gDec->decode_context_unsigned(&data[len],0,&val32);
                   if (ret <= 0)
                      return BACAPI_ERROR_MALFORMED_PACKET;
                  
                   key.algorithm = (uint8_t)val32;
                   len += ret;
                   
                   // key-id [1]
                   ret = gDec->decode_context_unsigned(&data[len],1,&val32);
                   if (ret <= 0)
                      return BACAPI_ERROR_MALFORMED_PACKET;
                   
                   key.key_id = (uint8_t)val32;
                   len += ret;
                   
                   pos = &data[len];
                   
                   set.key_id.push_back(key);
                  }
                  
            len++;   // while [3f] moramo preskociti
            
            pos = &data[len];
            property->complexValue.vSecurityKeySet.push_back(set);
            
           }         
        
       } 
 
 return BACAPI_OK;
}



int I_cBACServiceHandler::DecodeAuthenticationFactorFormat(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        len++;
       }

 len++;

 return len;
}





//int I_cBACServiceHandler::DecodeSecurityKeySet(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
//{
// int len = 0;
//
//  // prevrti cijelu listu rezultata
// while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
//       {
//        len++;
//       }
//
// len++;
//
// return len;
//}






int I_cBACServiceHandler::DecodePortPermission(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        len++;
       }

 len++;

 return len;
}



int I_cBACServiceHandler::DecodeProcessIdSelection(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        len++;
       }

 len++;

 return len;
}





int I_cBACServiceHandler::DecodeLightingCommand(uint8_t *data, BACNET_OBJECT_PROPERTY *property)
{
 int len = 0;

  // prevrti cijelu listu rezultata
 while (gDec->decode_is_closing_tag_number(&data[len],4) != true && gDec->decode_is_closing_tag_number(&data[len],3) != true)
       {
        len++;
       }

 len++;

 return len;
}



//=====================================================================================//
//===================[ DECODE BACNET PROPERTY ACCESS RESULT ]==========================//
// NOTE provjeriti: special property
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeBACnetPropertyAccessResult(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;

 // decode opening tag
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 *ret_len = len;
	 return BACAPI_ERROR_INVALID_OPENING_TAG;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 // NOTE ovo je wip funkcija, stvori vrijednost kao octet_string i prebaci podatke u ostr buffer
 BACNET_APPLICATION_DATA_VALUE *val = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_OSTRING);
 while (!gDec->decode_is_closing_tag_number(&apdu[len],4) && !gDec->decode_is_closing_tag_number(&apdu[len],3))
       {
        val->type.Octet_String.vValue.push_back(apdu[len]);
        len++;
       }

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

len++; // closing tag
return BACAPI_OK;
}
//_____________________________________________________________________________________//





//=====================================================================================//
//===================[ DECODE CHANNEL VALUE ]==========================================//
//=====================================================================================//
BACAPI_ERROR I_cBACServiceHandler::DecodeBACnetChannelValue(uint8_t *apdu, BACNET_OBJECT_PROPERTY *property, int *ret_len)
{
 int len = 0;

 // decode opening tag
 if (!gDec->decode_is_opening_tag_number(&apdu[len],4) && !gDec->decode_is_opening_tag_number(&apdu[len],3))
	{
	 *ret_len = len;
	 return BACAPI_ERROR_INVALID_OPENING_TAG;
	}
 len++;   // preskoci opening tag

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }


 // NOTE ovo je wip funkcija, stvori vrijednost kao octet_string i prebaci podatke u ostr buffer
 BACNET_APPLICATION_DATA_VALUE *val = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_OSTRING);
 while (!gDec->decode_is_closing_tag_number(&apdu[len],4) && !gDec->decode_is_closing_tag_number(&apdu[len],3))
       {
        val->type.Octet_String.vValue.push_back(apdu[len]);
        len++;
       }

 // ako je sljedeci tag zatvarajuci tag, vrati se
 if (gDec->decode_is_closing_tag_number(&apdu[len],4) || gDec->decode_is_closing_tag_number(&apdu[len],3))
    {
     len++; // preskoci zatvarajuci tag
     *ret_len = len;
     return BACAPI_OK;
    }

len++; // closing tag
return BACAPI_OK;
}
//_____________________________________________________________________________________//





