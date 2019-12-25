#include "I_cNPDUDecoder.h"
#include "I_cBACdec.h"
#include <bitset>
#include <iostream>
#include "bacapi.h"
//#include "I_cBBMD.h"
//#include <I_cGlobal.h>

I_cNPDUDecoder::I_cNPDUDecoder()
{
}


I_cNPDUDecoder::~I_cNPDUDecoder()
{
}




//=====================================================================================//
//================[ DECODE BVLC ]======================================================//
//=====================================================================================//
int I_cNPDUDecoder::DecodeBVLC(uint8_t *packet, BACNET_BVLC *bvlc, uint16_t *bvlc_result, BACNET_ADDRESS *source)
{
 //gLog->AppendLog("NPDUDecoder::DecodeBVLC: Decoding BVLC");
 uint8_t protocolType = 0;
 uint8_t function = 0;
 uint16_t size = 0;
 uint16_t result = 0;
 int len = 0;

 len += gDec->decode_unsigned8(&packet[len],&protocolType);
 len += gDec->decode_unsigned8(&packet[len],&function);
 len += gDec->decode_unsigned16(&packet[len],&size);

 switch (function)
        {
         // ovo je bvlc paket bez npdu djela
		 case BVLC_RESULT:
		 len += gDec->decode_unsigned16(&packet[len],&result);
         HandleBVLCResult(source,result);
         //char desc[256];
         //sprintf(desc,"BVLC Service Result: Success. Address: %s",BACAPI_GetAddressStringIP(source).c_str());
         //BACAPI_Message(desc,desc,3);
         *bvlc_result = result;
		 break;

		 case BVLC_WRITE_BROADCAST_DISTRIBUTION_TABLE:
		 {
		  // Upon receipt of a BVLL Write-Broadcast-Distribution-Table message,
		  // a BBMD shall attempt to create or replace its BDT,
          // depending on whether or not a BDT has previously existed.
          BACNET_BVLC_BDT tmp;
          DecodeBVLC_BDTable(&tmp,&packet[len], size-sizeof(BACNET_BVLC));
		  //gBBMD->WriteBroadcastDistributionTable(source, &tmp);
		  break;
		 }


		 case BVLC_READ_BROADCAST_DISTRIBUTION_TABLE:
		  // Netko je zatrazio moju BBMD tablicu, kodiraj i posalji
		  //gBBMD->ReadBroadcastDistributionTable(source);
          break;


		 case BVLC_READ_BROADCAST_DISTRIBUTION_TABLE_ACK:
		 {
		   // dobio sam tudju BBMD tablicu, dekodiraj i polaji event dalje na API
		  BACNET_BVLC_BDT *table= new BACNET_BVLC_BDT;
		  DecodeBVLC_BDTable(table,&packet[len], size-sizeof(BACNET_BVLC));

		  table->source = BACAPI_CreateBACnetAddress(source);
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_BVLC_READ_BDT_ACK;
		  event->bdtTable = table;
		  BACAPI_PushEvent(event);
		  *bvlc_result = 0;
		  break;
		 }

		 case BVLC_REGISTER_FOREIGN_DEVICE:
		 {
		  // Netko je zatrazio da registrira FD na moju listu
		  BACNET_BVLC_FDT_ENTRY *fd = DecodeBVLC_RegisterFD(&packet[len],size);
		  if (fd)
		     {
		      memcpy(fd->address,source->adr,4);
		      gDec->decode_unsigned16(&source->adr[4],&fd->port);
		      //gBBMD->RegisterForeignDevice(fd);
		     }
		  break;
		 }


		 case BVLC_READ_FOREIGN_DEVICE_TABLE:
		 {
		  // Netko je zatrazio popis mojih FD na listi
		  //gBBMD->ReadForeignDeviceTable(source);
		  break;
		 }


		 case BVLC_READ_FOREIGN_DEVICE_TABLE_ACK:
		 {
		  // Dobio sam tudju FD listu, dekodiraj i posalji event na API
		  BACNET_BVLC_FDT *table= new BACNET_BVLC_FDT;
		  DecodeBVLC_FDTable(table,&packet[len], size-sizeof(BACNET_BVLC));

		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_BVLC_READ_FDT_ACK;
		  event->fdtTable = table;
		  BACAPI_PushEvent(event);
		  *bvlc_result = 0;
		  break;
		 }


		 case BVLC_DELETE_FOREIGN_DEVICE_TABLE_ENTRY:
		  //gBBMD->DeleteFDTEntry(source,packet,size);
		 break;

         // ovi ostali sadrzavaju NPDU



		 case BVLC_FORWARDED_NPDU:
		  //gBBMD->ReceivedForwardNPDUMessage(source,packet,size);
		  *bvlc_result = BVLC_RESULT_CONTAINS_NPDU;
		  len=10;

		 break;
		 case BVLC_DISTRIBUTE_BROADCAST_TO_NETWORK:
		  break;

		 case BVLC_ORIGINAL_UNICAST_NPDU:
		  *bvlc_result = BVLC_RESULT_CONTAINS_NPDU;
		  break;

		 case BVLC_ORIGINAL_BROADCAST_NPDU:
		  // Upon receipt of a BVLL Original-Broadcast-NPDU message,
		  // a BBMD shall construct a BVLL Forwarded-NPDU message and
          // send it to each IP subnet in its BDT with the exception of its own.
#ifdef BACAPI_USES_BBMD
          //gBBMD->ReceivedOriginalBroadcastNPDU(source, packet,size);
#endif

		 *bvlc_result = BVLC_RESULT_CONTAINS_NPDU;
		 break;
        }



 bvlc->bacnet_vlc_type = protocolType;
 bvlc->bacnet_vlc_function = function;
 bvlc->bacnet_vlc_len = size;

 return len;
}
//_____________________________________________________________________________________\\



BACNET_BVLC_FDT_ENTRY *I_cNPDUDecoder::DecodeBVLC_RegisterFD(uint8_t *packet, int size)
{
 BACNET_BVLC_FDT_ENTRY *fd = new BACNET_BVLC_FDT_ENTRY;
 gDec->decode_unsigned16(packet,&fd->ttl);
 fd->time_remaining = fd->ttl;
 return fd;
}



void I_cNPDUDecoder::HandleBVLCResult(BACNET_ADDRESS *source, uint16_t result)
{

 switch (result)
		{
		 case BVLC_RESULT_SUCCESS:
		 {
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_BVLC_RESULT_SUCCESS;
		  BACAPI_PushEvent(event);
		  
		  BACAPI_Message("Success","Success",BACAPI_SEVERITY_MESSAGE);
		  break;
		 }

		 case BVLC_RESULT_WRITE_BDT_NAK:
		 break;


		 case BVLC_RESULT_READ_BDT_NAK:    // BDT - NAK
		 {
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_BVLC_READ_BDT_NAK;
		  BACAPI_PushEvent(event);

		  BACAPI_Message_BVLC_Read_BDT_NAK(source);


		  break;
		 }

		 case BVLC_RESULT_READ_FOREIGN_DEVICE_TABLE_NAK:   // FDT - NAK
		 {
          BACAPI_Message_BVLC_Read_FDT_NAK(source);
		  break;
		 }



         // register FD
		 case BVLC_RESULT_REGISTER_FOREIGN_DEVICE_NAK:
		 {
		  // posalji negative ACK
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_BVLC_REGISTER_FD_NAK;
		  BACAPI_PushEvent(event);
		  
		  BACAPI_Message_BVLC_Register_FD_NAK(source);
		  break;
		 }


		 case BVLC_RESULT_DELETE_FOREIGN_DEVICE_TABLE_ENTRY_NAK:
		 {
		  // posalji negative ACK
		  BACAPI_Event *event = BACAPI_CreateEvent();
		  event->type = BACAPI_EVENT_BVLC_DELETE_FD_NAK;
		  BACAPI_PushEvent(event);
		  
		  //BACAPI_Message("MSG","",0);
		  break;
		 }

		 case BVLC_RESULT_DISTRIBUTE_BROADCAST_TO_NETWORK_NAK:
		 break;

		 default: break;
		}
}


// n*10 okteta, ip[4], port[2], mask[4]
int I_cNPDUDecoder::DecodeBVLC_BDTable(BACNET_BVLC_BDT *table, uint8_t *packet, int size)
{
 int len = 0;

 // ovdje dolazi vrijednost ili lista vrijednosti
 while (len < size)
       {
        BACNET_BVLC_BDT_ENTRY entry;

        entry.address[0] = packet[len++];
        entry.address[1] = packet[len++];
        entry.address[2] = packet[len++];
        entry.address[3] = packet[len++];

        len += gDec->decode_unsigned16(&packet[len],&entry.port);

        entry.mask[0] = packet[len++];
        entry.mask[1] = packet[len++];
        entry.mask[2] = packet[len++];
        entry.mask[3] = packet[len++];

        table->vTableEntryList.push_back(entry);
       }


 return len;
}





int I_cNPDUDecoder::DecodeBVLC_FDTable(BACNET_BVLC_FDT *table, uint8_t *packet, int size)
{
 int len = 0;


 // ovdje dolazi vrijednost ili lista vrijednosti
 while (len < size)
       {
        BACNET_BVLC_FDT_ENTRY *tableEntry = new BACNET_BVLC_FDT_ENTRY;

        tableEntry->address[0] = packet[len++];
        tableEntry->address[1] = packet[len++];
        tableEntry->address[2] = packet[len++];
        tableEntry->address[3] = packet[len++];

        len += gDec->decode_unsigned16(&packet[len],&tableEntry->port);
        len += gDec->decode_unsigned16(&packet[len],&tableEntry->ttl);
        len += gDec->decode_unsigned16(&packet[len],&tableEntry->time_remaining);

        table->vTableEntryList.push_back(tableEntry);
       }


 return len;
}





//=====================================================================================//
//================[ DECODE LLC ]=======================================================//
// kod etherneta
//=====================================================================================//
int I_cNPDUDecoder::DecodeLLC(uint8_t *packet, BACNET_LLC *llc)
{
 //gLog->AppendLog("NPDUDecoder::DecodeBVLC: Decoding BVLC");
 uint8_t protocolType = 0;
 uint8_t function = 0;
 uint8_t controlfield = 0;

 gDec->decode_unsigned8(&packet[0],&protocolType);
 gDec->decode_unsigned8(&packet[1],&function);
 gDec->decode_unsigned8(&packet[2],&controlfield);


 // kada se dekodira ETH802 frame ovaj dio se zove Logical Link Control
 // 1. 0x82 (DSAP)
 // 2. 0x82 (SSAP)
 // 3. command


 llc->bacnet_llc_type = protocolType;  //  (DSAP)
 llc->bacnet_llc_function = function;  //  (SSAP)
 llc->bacnet_llc_control_field = controlfield;           //  command


 return sizeof(BACNET_LLC);
}
//_____________________________________________________________________________________\\




//=====================================================================================//
//================[ DECODE NPDU ]======================================================//
// RET: Duzina NPDU-a u bytovima
//=====================================================================================//
int I_cNPDUDecoder::DecodeNPDU(uint8_t *npdu, BACNET_NPDU_DATA *npduData)
{
 int len = 0;        /* return value - number of octets loaded in this function */
 npduData->dadr = NULL;
 npduData->sadr = NULL;
 npduData->dnet = 0;
 npduData->snet = 0;

 npduData->protocolVersionNumber = npdu[0];

 // BIT[7]
 // TRUE: ovo je network layer poruka. Message Type postoji u NPDU-u
 // FALSE: Nije network poruka, Message Type ne postoji i poruka sadrzi APDU
 if ((npdu[1] & BIT7))
       {
        npduData->network_layer_message = true;
       }else npduData->network_layer_message = false;

 // Bit [6]: Reserved. Shall be zero.
 // [0]


 // BIT[5]
 // FALSE: DNET,DLEN,DADR i HopCount ne postoje
 // TRUE: DNET,DLEN i HopCount postoje
 //       - Ako je DLEN == 0, ovo je broadcast i MAC DADR i DADR ne postoje
 //       - Ako je DLEN > 0, DLEN oznacava velicinu DADR polja
 if ((npdu[1] & BIT5))
    {
     npduData->dadr = BACAPI_CreateBACnetAddress();
    }else npduData->dadr = NULL;

 // Bit 4: Reserved. Shall be zero.
 // [0]

 // BIT[3]
 // FALSE: SNET,SLEN,SARD ne postoje
 // TRUE: SNET,SLEN,SARD postoje
 //       - SLEN == 0, je neispravno kodiranje
 //       - SLEN > 0, SLEN oznacava velicinu SADR polja
 if ((npdu[1] & BIT3))
    {
     npduData->sadr = BACAPI_CreateBACnetAddress();
    }else npduData->sadr = NULL;


 // BIT[2] expecting reply
 if ((npdu[1] & BIT2))
    {
     npduData->isExpectingReply = true;
    }else npduData->isExpectingReply = false;

 // Bits 1,0: Network priority
 npduData->priority = (BACNET_MESSAGE_PRIORITY) (npdu[1] & 0x03);


 // Dekodiraj ostatak paketa s obzirom na stanje gore specificiranih bitova
 len = 2;

 // DESTINATION
 if (npduData->dadr) // -TRUE: DNET,DLEN i HopCount postoje
    {
     // DNET i DLEN
     len += gDec->decode_unsigned16(&npdu[len], &npduData->dadr->networkNumber); // DNET
     npduData->dadr->len = npdu[len++];    // DLEN
     npduData->dnet = npduData->dadr->networkNumber;

     if (npduData->dadr->len == 0)  // broadcast, ne treba nam dard->len
        {
         
         //delete dadr;
         //BACAPI_DeleteNPDUData(npduData);
         //return -1;
        }

     for (int i = 0; i < npduData->dadr->len; i++)
          npduData->dadr->adr[i] = npdu[len++];

    }

 // SOURCE
 if (npduData->sadr)
    {
     // SNET i SLEN
     len += gDec->decode_unsigned16(&npdu[len], &npduData->sadr->networkNumber); // Source Network Address
     npduData->sadr->len = npdu[len++];
     npduData->snet = npduData->sadr->networkNumber;
     
     // spresent == true ali sard->len == 0 se nesmije dogoditi
     if (npduData->sadr->len == 0)
        {
         delete npduData->sadr;
         npduData->sadr = NULL;
         BACAPI_Message("Source Address Length is 0 when decoding NPDU","Source Address Length is 0 when decoding NPDU. This indicated an error with the NPDU",BACAPI_SEVERITY_ERROR);
        }

     if (npduData->sadr)
        {
		 for (int i = 0; i < npduData->sadr->len; i++)
			  npduData->sadr->adr[i] = npdu[len++];
	    }
    }


  /* The Hop Count field shall be present only if the message is */
  /* destined for a remote network, i.e., if DNET is present. */
  /* This is a one-octet field that is initialized to a value of 0xff. */
  if (npduData->dadr)
     {
      if (npduData->dadr->networkNumber) // nn nije 0
         {
          npduData->hopCount = npdu[len++];
         }
     } else npduData->hopCount = 0;


  /* Indicates that the NSDU conveys a network layer message. */
  /* Message Type field is present. */
  if (npduData->network_layer_message)
	 {
	  npduData->npduMessageType = (BACNET_NPDU_MESSAGE_TYPE) npdu[len++];

	  /* Message Type field contains a value in the range 0x80 - 0xFF, */
	  /* then a Vendor ID field shall be present */
	  if (npduData->npduMessageType >= 0x80)
		 {
		  len += gDec->decode_unsigned16(&npdu[len], &npduData->vendor_id);
		 }
     } else {
		     /* FIXME: another value for this? */
		     npduData->npduMessageType = NETWORK_MESSAGE_INVALID;
		    }


 return len;
}
//_____________________________________________________________________________________\\


