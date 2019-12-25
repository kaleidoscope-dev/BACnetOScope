#include "I_cAPDUDecoder.h"
#include "I_cBACdec.h"
#include "bacevent.h"
#include "bacapi.h"
#include "I_cBACLog.h"
#include <string.h>

I_cAPDUDecoder::I_cAPDUDecoder()
{
}



I_cAPDUDecoder::~I_cAPDUDecoder()
{
 delete serviceHandler_;
}



void I_cAPDUDecoder::Init(bool routing_enable)
{
 serviceHandler_ = new I_cBACServiceHandler();

 // BACAPI koristi dvije vrste event poruka. Ako je definiran BACAPI_ROUTING_ENABLE
 // switch, APDU dio paketa se nece dekodirati. Kreirat ce se novi event
 // sa byte streamom koji je dosao. Ovo se koristi kada nemoramo znati APDU dio paketa
 routingEnabled_ = routing_enable;  //

 segmented_sequence_ = 0;
}



void I_cAPDUDecoder::HandleAPDU(uint8_t *apdu, BACNET_NPDU_DATA *npdu, int apdulen, uint8_t bvlc_len)
{
 if (routingEnabled_)
    {
	 BACAPI_Event *event = BACAPI_CreateEvent();
	 event->packet = BACAPI_CreatePacket(apdu,npdu,apdulen, bvlc_len);
	 event->type = BACAPI_EVENT_ROUTE_PACKET;

	 gLog->AppendLog("I_cAPDUDecoder::Stvaram paket dlink: %d",npdu->dataLink);

	 BACAPI_PushEvent(event);

	 // ako koristimo API kao router a zelimo svejedno koristiti application layer (npr. router ima device-objekt)
	 // onda se definira ovaj define u projektu.
	 // NOTE: za sada ostaviti ovako dok se ne odjele api i stack

//#ifdef BACAPI_ROUTING_HANDLE_APDU_ENABLE
     //DecodeAPDU(apdu, npdu, apdulen,0);
//#else

     // NOTE: ovdje vise nikada nebi smjeli doc. Vidi gore NOTE.
	 // vise netrebamo prosljedjeni npdu i apdu. ovdje ih sigurno mozemo isbrisati
	 BACAPI_DeleteNPDUData(npdu);
	 delete []apdu;
//#endif
    }else {
	       DecodeAPDU(apdu, npdu, apdulen, bvlc_len);
	      }
}



void I_cAPDUDecoder::DecodeAPDU(uint8_t *apdu, BACNET_NPDU_DATA *npdu, int apdulen, int bvlc_len)
{
 int len = 0;
 BACNET_PDU_TYPE pduType;

 len += DecodePDUType(&apdu[len], &pduType);


 switch (pduType)
        {
         // UNCONFIRMED
         case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
         {
          int data_len = 0;
          BACNET_UNCONFIRMED_SERVICE service;
          len += DecodeUnconfService(&apdu[len], &service);
          data_len = apdulen-len;
          uint8_t *apdu_data = NULL;

		  serviceHandler_->Handle_Unconfirmed(&apdu[len],npdu,service,data_len, bvlc_len);
		  delete []apdu;   // alokacija iz I_cDecoder::HandleDataPacket()
		  BACAPI_DeleteNPDUData(npdu);  // alokacija iz I_cDecoder::HandleDataPacket()
          break;
         }


         //CONFIRMED
         case PDU_TYPE_CONFIRMED_SERVICE_REQUEST:
         {
          int data_len = 0;
          //gLog->AppendLog("I_cAPDUDecoder::DecodeAndExec: PDU_TYPE_CONFIRMED_SERVICE_REQUEST:\n");
          BACNET_SERVICE_CHOICE_CONFIRMED service;
          len += DecodeConfService(&apdu[len],&service);
          data_len = apdulen-len;

          if (data_len > 0)
             {
			  serviceHandler_->Handle_Confirmed(&apdu[len],npdu,service.serviceChoice, data_len, service.invoke_id);
			 }else {
			        // nepoznati service request
			        gLog->AppendLog("APDUDecoder::DecodeAPDU: PDU_TYPE_CONFIRMED_SERVICE_REQUEST: DataLen for Conf.Req is 0 ?");
			       }

	      delete []apdu;
	      BACAPI_DeleteNPDUData(npdu);  // ne treba nam vise osim u I-AM PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST
          break;
         }

         // SIMPLE ACK
         case PDU_TYPE_SIMPLE_ACK:
         {
          BACAPI_Event *event = BACAPI_CreateEvent();

          event->type = BACAPI_EVENT_SIMPLE_ACK;
          event->simpleAck = new BACNET_SIMPLE_ACK;
          event->invokeId = apdu[len];
          event->simpleAck->serviceChoice = apdu[len+1];

          BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_Message_SimpleACK(event->simpleAck->serviceChoice,adr);

          BACAPI_PushEvent(event);
          BACAPI_DeleteNPDUData(npdu);  // ne treba nam vise osim u I-AM PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST
          delete []apdu;
          break;
         }

         // COMPLEX ACK
         case PDU_TYPE_COMPLEX_ACK:
         {
          uint8_t *ptr_general_apdu_data = NULL;
          static int seg_data_len = 0;
          int data_len = 0;
          int data_pos = 0;
          BACNET_SERVICE_CHOICE_CONFIRMED service;


          data_pos += DecodeComplexACK(apdu,&service);
          data_len = apdulen-data_pos;

          // prebaci cijelu DATA dio paketa u novi buffer
          uint8_t *apdu_data = NULL;
          try {
               apdu_data = new uint8_t[data_len];
              }catch (std::bad_alloc& ba)
                     {
                      gLog->AppendLog("APDUDecoder::DecodeAPDU: PDU_TYPE_COMPLEX_ACK: caught bad_alloc. [%s]",ba.what());
                      delete []apdu;
                      BACAPI_DeleteNPDUData(npdu);
                      return;
                     }

          memcpy(apdu_data,&apdu[data_pos],data_len);
          ptr_general_apdu_data = apdu_data;


          if (IsSegmentedPacket(&service)) // segmentirana poruka?
             {
              // ispali event za nastavak
              gLog->AppendLog("DecodeAPDU: Segmented Packet: seg_data_len = %d, data_len = %d",seg_data_len,data_len);
              gLog->IncInd(3);
              seg_data_len += HandleSegmentedPacket(apdu_data,data_len,&service);
              gLog->DecInd(3);
              delete []apdu_data; // ne treba nam vise
              BACAPI_DeleteNPDUData(npdu); // ne treba za ovu poruku
              return;
             }else if (IsSegmentedEnd(&service)) // kraj poruke?
                      {
                       gLog->IncInd(3);
                       ptr_general_apdu_data = HandleSegmentedEnd(apdu_data, data_len, &service);
                       gLog->DecInd(3);
                       seg_data_len += data_len; // zbroji segmentiranu velicinu poruke
                       data_len = seg_data_len;  // finalna velicina koja ulazi u Handle_ComplexACK

                       //BACAPI_PrintOctetData("c:\\data.bin",ptr_general_apdu_data,data_len);
                      }

          if (ptr_general_apdu_data)
             {
			  serviceHandler_->Handle_ComplexACK(ptr_general_apdu_data,npdu,service.serviceChoice, data_len, service.invoke_id);
			  BACAPI_DeleteNPDUData(npdu);
			  seg_data_len = 0;  // resetiraj static int
			 }
          //delete []apdu;
          break;
         }

         // ERROR
         case PDU_TYPE_ERROR:
         {
          int ret_len = 0;
          uint8_t invokeId = 0;
          BACNET_ERROR err;
          uint8_t service = 0;
          BACAPI_ERROR ret_err = DecodeError(&apdu[len],apdulen,&err, &invokeId, &service, &ret_len);
          if (ret_err != BACAPI_OK)
             {
              BACAPI_MESSAGE *msg = new BACAPI_MESSAGE;
              msg->pdu_type = PDU_TYPE_ERROR;
              msg->type = ret_err;
              msg->service = service;
              msg->severity = BACAPI_SEVERITY_ERROR;
              BACAPI_Message_Error(msg);

              gLog->AppendLog("I_cAPDUDecoder::DecodeAndExec: PDU_TYPE_ERROR: Failed decoding BACerror: (%s)",BACAPI_GetErrorString(ret_err));

              BACAPI_DeleteNPDUData(npdu);
              delete []apdu;
              
              return;
             }

          gLog->AppendLog("I_cAPDUDecoder::DecodeAndExec: PDU_TYPE_ERROR: (%s), (%s)", BACAPI_BACnetErrorClassName(err.errClass),BACAPI_BACnetErrorCodeName(err.errCode));

          BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_ERROR;
          event->pdu_type = PDU_TYPE_ERROR;
          event->error.errClass = err.errClass;
          event->error.errCode = err.errCode;
          event->invokeId = invokeId;
          event->address = adr;

          BACAPI_PushEvent(event);
          delete []apdu;
          BACAPI_DeleteNPDUData(npdu);
          break;
         }

         case PDU_TYPE_REJECT:
         {
          gLog->AppendLog("I_cAPDUDecoder::DecodeAndExec: PDU_TYPE_REJECT:\n");
          uint8_t invokeId = 0;
          BACNET_REJECT_CODE rejCode;
          DecodeReject(&apdu[len-1],apdulen, &rejCode, &invokeId);

          BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_Message_BACnetReject(rejCode,invokeId,adr);

          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_REJECT;
          event->rejCode = rejCode;
          event->invokeId = invokeId;


          BACAPI_PushEvent(event);
          BACAPI_DeleteNPDUData(npdu);  // ne treba nam vise osim u I-AM PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST
          break;
         }

         case PDU_TYPE_ABORT:
         {
          gLog->AppendLog("I_cAPDUDecoder::DecodeAndExec: PDU_TYPE_ABORT:\n");
          uint8_t invokeId = 0;
          BACNET_ABORT abort;
          int ret = DecodeAbort(&apdu[len-1],apdulen,&abort);
          if (ret < 0)
             {
              // TODO: error decoding abort pdu
              return;
             }

          BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(npdu->remote);
          BACAPI_Message_BACnetAbort(abort,adr);


          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_ABORT;
          event->abort = abort;
          event->abort.invoke_id = invokeId;

          event->invokeId = invokeId;
          BACAPI_PushEvent(event);

          BACAPI_DeleteNPDUData(npdu);  // ne treba nam vise osim u I-AM PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST
          break;
         }

         default:
         {
          gLog->AppendLog("I_cAPDUDecoder::DecodeAndExec: BACAPI_EVENT_ERROR_UNKNOWN_PDU_TYPE:\n");
          // nepoznati apdu
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_ERROR_UNKNOWN_PDU_TYPE;
          BACAPI_PushEvent(event);
          BACAPI_DeleteNPDUData(npdu);
          break;
         }
        }
}



//=====================================================================================//
//================[ DECODE SERVICE REQUEST ]===========================================//
//=====================================================================================//
int I_cAPDUDecoder::DecodePDUType(uint8_t *packet, BACNET_PDU_TYPE *pduType)
{
 uint8_t pdutype = 0;
 int len = 0;
 len = gDec->decode_unsigned8(&packet[0], &pdutype);

 pdutype = pdutype & 0xF0;

 *pduType = (BACNET_PDU_TYPE)pdutype;


 return len;
}
//_____________________________________________________________________________________//





int I_cAPDUDecoder::DecodeUnconfService(uint8_t *packetBuffer, BACNET_UNCONFIRMED_SERVICE *service)
{
 uint8_t serviceChoice = 0;
 int len = gDec->decode_unsigned8(&packetBuffer[0], &serviceChoice);
 *service = (BACNET_UNCONFIRMED_SERVICE)serviceChoice;
 return len;
}






int I_cAPDUDecoder::DecodeConfService(uint8_t *packetBuffer, BACNET_SERVICE_CHOICE_CONFIRMED *serviceData)
{
 uint16_t len = 0;
 uint8_t serviceChoice = 0;

 serviceData->segmented_message = (packetBuffer[len] & SET_BIT(3)) ? true : false;
 serviceData->more_follows = (packetBuffer[len] & SET_BIT(2)) ? true : false;
 serviceData->segmented_response_accepted = (packetBuffer[len] & SET_BIT(1)) ? true : false;

 len++;

 // NOTE: Popraviti ovo ovdje. Ako je ovo Comfirmed REQ onda dolaze ove dvije linije
 //       Ako nije onda ih nema
 serviceData->invoke_id = packetBuffer[len++];


 if (serviceData->segmented_message)
    {
     // ne podrzavamo segmentirane poruke sada.
     serviceData->sequence_number = packetBuffer[len++];
     serviceData->proposed_window_number = packetBuffer[len++];
    }


 len += gDec->decode_unsigned8(&packetBuffer[len], &serviceChoice);

 serviceData->max_segs = gDec->decode_max_segs(packetBuffer[1]);
 serviceData->max_resp = gDec->decode_max_apdu(packetBuffer[1]);

 serviceData->serviceChoice = (BACNET_CONFIRMED_SERVICE)serviceChoice;
 return len;
}


//=====================================================================================//
//===================[ DECODE: COMPLEX ACK ]===========================================//
//=====================================================================================//
int I_cAPDUDecoder::DecodeComplexACK(uint8_t *packetBuffer, BACNET_SERVICE_CHOICE_CONFIRMED *serviceData)
{
 uint16_t len = 0;
 uint8_t serviceChoice = 0;


 serviceData->segmented_message = (packetBuffer[len] & SET_BIT(3)) ? true : false;
 serviceData->more_follows = (packetBuffer[len] & SET_BIT(2)) ? true : false;
 serviceData->segmented_response_accepted = (packetBuffer[len] & SET_BIT(1)) ? true : false;

 len++;  // gotov s PDU flags, nastavi na sljedeci byte

 // NOTE: Popraviti ovo ovdje. Ako je ovo Comfirmed REQ onda dolaze ove dvije linije
 //       Ako nije onda ih nema
 serviceData->invoke_id = packetBuffer[len++];
 //gLog->AppendLog("I_cAPDUDecoder::DecodeComplexACK: Invoke id: %d",serviceData->invoke_id);
 //gLog->AppendLog("I_cAPDUDecoder::DecodeComplexACK: segmented_message: %d",serviceData->segmented_message);
 //gLog->AppendLog("I_cAPDUDecoder::DecodeComplexACK: more_follows: %d",serviceData->more_follows);
 //gLog->AppendLog("I_cAPDUDecoder::DecodeComplexACK: segmented_response_accepted: %d",serviceData->segmented_response_accepted);

 if (serviceData->segmented_message)
    {
     // ne podrzavamo segmentirane poruke sada.
     serviceData->sequence_number = packetBuffer[len++];
     serviceData->proposed_window_number = packetBuffer[len++];
    }



 len += gDec->decode_unsigned8(&packetBuffer[len], &serviceChoice);


 serviceData->max_segs = gDec->decode_max_segs(packetBuffer[0]);
 serviceData->max_resp = gDec->decode_max_apdu(packetBuffer[0]);

 serviceData->serviceChoice = (BACNET_CONFIRMED_SERVICE)serviceChoice;
 return len;
}
//_____________________________________________________________________________________//




//=====================================================================================//
//===================[ DECODE: ERROR ]=================================================//
//=====================================================================================//
BACAPI_ERROR I_cAPDUDecoder::DecodeError(uint8_t *packet , int apdu_len, BACNET_ERROR *error,uint8_t * invoke_id, uint8_t *service, int *ret_len)
{
 int len = 0;

 *invoke_id = packet[len++];
 
 //printf("DECODE ERR: invoke: %d\n",*invoke_id);
 *service = (BACNET_CONFIRMED_SERVICE) packet[len++];

 if (*service == SERVICE_CONFIRMED_CREATE_OBJECT)
    {
     int ret = gDec->bacerror_create_object_decode_error_class_and_code(&packet[len], apdu_len - 2, &error->errClass, &error->errCode);
     if (ret < 0)
        {
         return BACAPI_ERROR_MALFORMED_PACKET;
        }
     len += ret;
    }else {
           len = gDec->bacerror_decode_error_class_and_code(&packet[len], apdu_len - 2, &error->errClass, &error->errCode);
          }




 *ret_len += len;
 return BACAPI_OK;
}
//_____________________________________________________________________________________//






//=====================================================================================//
//===================[ DECODE: REJECT ]================================================//
// TODO: Da li reject dolazi s service?
//=====================================================================================//
int I_cAPDUDecoder::DecodeReject(uint8_t *packet , int apdu_len, BACNET_REJECT_CODE *rejCode ,uint8_t * invoke_id)
{
 int len = 0;

 if (packet[len++] != PDU_TYPE_REJECT)
    {
     return -1;
    }else {
           *invoke_id = packet[len++];
           //service = (BACNET_CONFIRMED_SERVICE) packet[len++];
           *rejCode = (BACNET_REJECT_CODE)packet[len++];  // reject code

          }


    return len;
}
//_____________________________________________________________________________________//






//=====================================================================================//
//===================[ DECODE: ABORT ]=================================================//
//=====================================================================================//
int I_cAPDUDecoder::DecodeAbort(uint8_t *packet , int apdu_len, BACNET_ABORT *abort)
{
 int len = 0;

 if ((packet[0] & 0xF0) != PDU_TYPE_ABORT)
    {
     return -1;
    }

 if (packet[0] & 1)
    {
     abort->server = true;
    }else {
           abort->server = false;
          }

 len++;
 abort->invoke_id = packet[len++];
 abort->reason = (BACNET_ABORT_REASON)packet[len++];



  return len;
}
//_____________________________________________________________________________________//





bool I_cAPDUDecoder::IsSegmentedPacket(BACNET_SERVICE_CHOICE_CONFIRMED *service)
{
 if (service->segmented_message && service->more_follows)
    {
     return true;
    }
 return false;
}


bool I_cAPDUDecoder::IsSegmentedEnd(BACNET_SERVICE_CHOICE_CONFIRMED *service)
{
 if (service->segmented_message && !service->more_follows)
    {
     return true;
    }
 return false;
}



bool I_cAPDUDecoder::SegmentOutOfSequence(int seq_num)
{
 // ako je seq_num > 0  a lista segmenata je prazna
 int count = vSegmentedPacket.size();

 if (count == 0 && seq_num == 0)
     return false;  // ok


 if (count > 0)
    {
     int num = vSegmentedPacket.size()-1;  // daj mi zadnji pristigli paket
     BACAPI_SEGMENT *seg = &vSegmentedPacket[num];

     // dobiveni seq_num uvije mora biti za jedan veci od proslog
     if (seg->sequence_number == seq_num-1)
         return false;  // sve OK
    }

 // else
 return true;
}


int I_cAPDUDecoder::HandleSegmentedPacket(uint8_t *apdu, int apdulen, BACNET_SERVICE_CHOICE_CONFIRMED *service)
{
 // provjeri da li ovaj segment postoji na listi
 int seg_cnt = (int)vSegmentedPacket.size();
 gLog->AppendLog("HandleSegmentedPacket: Primio segment. Dobiveni sequence: %d, Trenutni broj segmenata: %d",service->sequence_number,seg_cnt);
 gLog->AppendLog("HandleSegmentedPacket: Provjeravam da li je dobiveni seqment out of sequence");
 if (SegmentOutOfSequence(service->sequence_number))
    {
     gLog->IncInd(3);
     gLog->AppendLog("HandleSegmentedPacket: Dobiveni segment je out of sequence: Trenutna lista segmenata:");
     gLog->IncInd(3);
	 for (int c=0;c<seg_cnt;c++)
		 {
		  BACAPI_SEGMENT *seg = &vSegmentedPacket[c];
		  gLog->AppendLog("vSegmentedPacket[%d].seq_num: %d",c,vSegmentedPacket[c].sequence_number);
		 }
     gLog->DecInd(3);
     gLog->DecInd(3);



	 BACNET_SEGMENT_ACK *segACK = new BACNET_SEGMENT_ACK;

	 segACK->pduType = PDU_TYPE_SEGMENT_ACK;
	 segACK->invokeId = service->invoke_id;
	 segACK->negativeACK = true;
	 segACK->server = false;
	 segACK->reserved = 0;
	 segACK->sequenceNum = service->sequence_number;
	 segACK->windowSize = service->proposed_window_number;

	 BACAPI_Event *event = BACAPI_CreateEvent();

	 event->type = BACAPI_EVENT_SEGMENT_ACK;
	 event->segmentAck = segACK;
	 BACAPI_PushEvent(event);
	 //gData->segmentedFlag_ = false;
	 //vSegmentedPacket.clear();
     return 0;
    }
 BACNET_SEGMENT_ACK *segACK = new BACNET_SEGMENT_ACK;

 segACK->pduType = PDU_TYPE_SEGMENT_ACK;
 segACK->invokeId = service->invoke_id;
 segACK->negativeACK = false;
 segACK->server = false;
 segACK->reserved = 0;
 segACK->sequenceNum = service->sequence_number;
 segACK->windowSize = service->proposed_window_number;

 // Prvi messagte fragment uvijek mora biti potvrdjen
 if (segmented_sequence_ == 0)
    {
	 BACAPI_Event *event = BACAPI_CreateEvent();

	 event->type = BACAPI_EVENT_SEGMENT_ACK;
	 event->segmentAck = segACK;

	 BACAPI_PushEvent(event);
	 segmented_sequence_++;
	}

 // ako je sequence number == window_size, uvijek potvrdi
 if (segACK->sequenceNum >= service->proposed_window_number)
    {
	 BACAPI_Event *event = BACAPI_CreateEvent();

	 event->type = BACAPI_EVENT_SEGMENT_ACK;
	 event->segmentAck = segACK;

	 BACAPI_PushEvent(event);
    }


 BACAPI_SEGMENT segment;
 segment.data = new uint8_t[apdulen];
 segment.datalen = apdulen;
 segment.sequence_number = service->sequence_number;
 segment.window_size = service->proposed_window_number;
 memcpy(segment.data,apdu,apdulen);

 vSegmentedPacket.push_back(segment);
 return apdulen;
}


uint8_t *I_cAPDUDecoder::HandleSegmentedEnd(uint8_t *apdu, int apdulen, BACNET_SERVICE_CHOICE_CONFIRMED *service)
{
 gLog->AppendLog("HandleSegmentedEnd: Dobio sam zadnji segment.");
 int seg_cnt = (int)vSegmentedPacket.size();
 gLog->AppendLog("HandleSegmentedEnd: Provjeravam da li je dobiveni seqment out of sequence");
 if (SegmentOutOfSequence(service->sequence_number))
    {
     gLog->IncInd(3);
     gLog->AppendLog("HandleSegmentedEnd: Dobiveni segment (%d) je out of sequence: Trenutna lista segmenata:",service->sequence_number);
     gLog->IncInd(3);
	 for (int c=0;c<seg_cnt;c++)
		 {
		  BACAPI_SEGMENT *seg = &vSegmentedPacket[c];
		  gLog->AppendLog("vSegmentedPacket[%d].seq_num: %d",c,vSegmentedPacket[c].sequence_number);
		 }
     gLog->DecInd(3);
     gLog->DecInd(3);

     vSegmentedPacket.clear();  // isprazni segmented buffer
     segmented_sequence_ = 0;  // reset


	 BACNET_SEGMENT_ACK *segACK = new BACNET_SEGMENT_ACK;

	 segACK->pduType = PDU_TYPE_SEGMENT_ACK;
	 segACK->invokeId = service->invoke_id;
	 segACK->negativeACK = true;
	 segACK->server = false;
	 segACK->reserved = 0;
	 segACK->sequenceNum = service->sequence_number;
	 segACK->windowSize = service->proposed_window_number;

	 BACAPI_Event *event = BACAPI_CreateEvent();

     BACAPI_segmentedFlag = false;
     ClearSegmentBuffer();

	 event->type = BACAPI_EVENT_SEGMENT_ACK;
	 event->segmentAck = segACK;
	 BACAPI_PushEvent(event);
     return 0;
    }

 BACAPI_segmentedFlag = true;
 BACNET_SEGMENT_ACK *segACK = new BACNET_SEGMENT_ACK;

 segACK->pduType = PDU_TYPE_SEGMENT_ACK;
 segACK->invokeId = service->invoke_id;
 segACK->negativeACK = false;
 segACK->server = false;
 segACK->reserved = 0;
 segACK->sequenceNum = service->sequence_number;
 segACK->windowSize = service->proposed_window_number;

 BACAPI_Event *event = BACAPI_CreateEvent();

 event->type = BACAPI_EVENT_SEGMENT_ACK;
 event->segmentAck = segACK;

 // ubaci zadnji paket na listu
 BACAPI_SEGMENT segment;
 segment.data = new uint8_t[apdulen];
 segment.datalen = apdulen;
 segment.sequence_number = service->sequence_number;
 memcpy(segment.data,apdu,apdulen);
 vSegmentedPacket.push_back(segment);


 // sortiraj dobivene pakete
 SortSegmentedPackets();


 // izracunaj velicinu svih paketa
 int fin_len = 0;
 seg_cnt = (int)vSegmentedPacket.size();
 for (int c=0;c<seg_cnt;c++)
     {
      BACAPI_SEGMENT *segment = &vSegmentedPacket[c];
      gLog->AppendLog("I_cAPDUDecoder::HandleSegmentedEnd: vSegmentedPacket.seg = %d",segment->sequence_number);

      fin_len += (int)segment->datalen;
     }

 // stvorimo novi buffer za cijelu poruku i kopiraj sve segmente u buffer
 uint8_t *buffer = new uint8_t[fin_len];
 int len = 0;
 for (int c=0;c<seg_cnt;c++)
	 {
	  BACAPI_SEGMENT *segment = &vSegmentedPacket[c];
	  memcpy(&buffer[len],segment->data,segment->datalen);
	  len+=segment->datalen;
	 }

 // zavrsili smo s segmentacijom, pocisti sve
 for (int c=0;c<seg_cnt;c++)
     {
      BACAPI_SEGMENT *segment = &vSegmentedPacket[c];
      delete []segment->data;
     }


 // potvrdi zadnji dobiveni segment
 BACAPI_PushEvent(event);
 this->segmented_sequence_ = 0;  // reset
 BACAPI_segmentedFlag = false;

 vSegmentedPacket.clear();  // isprazni segmented buffer
 int size = vSegmentedPacket.size();
 gLog->AppendLog("HandleSegmentedEnd: Kraj segmentirane poruke. vSegmentedPacket: %d",size);


 return buffer;
}


void I_cAPDUDecoder::ClearSegmentBuffer(void)
{
 vSegmentedPacket.clear();
 segmented_sequence_ = 0;  // reset
}


bool myfunction (BACAPI_SEGMENT s1, BACAPI_SEGMENT s2)
{
 return (s1.sequence_number<s2.sequence_number);
}

void I_cAPDUDecoder::SortSegmentedPackets(void)
{

 int size = vSegmentedPacket.size();
 std::sort(vSegmentedPacket.begin(),vSegmentedPacket.end(),myfunction);

 for (int c=0;c<vSegmentedPacket.size();c++)
     {
      //printf("s = %d\n",vSegmentedPacket[c].sequence_number);
     }


}


// NOTE: promjeniti u NAK
void I_cAPDUDecoder::DecodeAndExecBVLC_NAK(uint8_t *packet, BACNET_BVLC *bvlc, uint16_t bvlc_result)
{
 switch (bvlc_result)
        {
		 case BVLC_RESULT_SUCCESS:
		 {

		  break;
		 }
		 // decode
		 case BVLC_RESULT_WRITE_BDT_NAK:
		 case BVLC_RESULT_READ_BDT_NAK:
		 {
		  // posalji negative ACK
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_BVLC_READ_BDT_NAK;
          BACAPI_PushEvent(event);
          break;
         }


		 case BVLC_RESULT_REGISTER_FOREIGN_DEVICE_NAK:
		  break;
		 case BVLC_RESULT_READ_FOREIGN_DEVICE_TABLE_NAK:
		 case BVLC_RESULT_DELETE_FOREIGN_DEVICE_TABLE_ENTRY_NAK:
		 case BVLC_RESULT_DISTRIBUTE_BROADCAST_TO_NETWORK_NAK:
		 break;
        }

}






