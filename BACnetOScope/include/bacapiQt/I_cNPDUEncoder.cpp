#include "I_cNPDUEncoder.h"
#include "I_cBACdec.h"
#include <string.h>
#if defined BACAPI_BUILD_ARM9 || defined BACAPI_BUILD_LINUX
#include <arpa/inet.h>
#endif

#ifdef BACAPI_BUILD_WIN32
//#include <winsock2.h>  // htons
#endif



I_cNPDUEncoder::I_cNPDUEncoder()
{
}


I_cNPDUEncoder::~I_cNPDUEncoder()
{
}






//======================================================================================//
//========================[ ENCODE NPDU DATA ]==========================================//
// NOTE: funkcija se mora zvati set npdu data ili tako nesto
//======================================================================================//
void I_cNPDUEncoder::SetNPDUData(BACNET_NPDU_DATA *npduData, bool reply, int bacnetMessagePriority, int hopcount, bool net_message)
{

 npduData->isExpectingReply = reply;
 npduData->protocolVersionNumber = BACNET_PROTOCOL_VERSION;
 npduData->npduMessageType = NETWORK_MESSAGE_INVALID;        // nije router
 npduData->priority = bacnetMessagePriority;
 npduData->hopCount = hopcount;
 npduData->network_layer_message = net_message;   // samo APDU


}
//______________________________________________________________________________________\\










//======================================================================================//
//========================[ ENCODE NPDU ]===============================================//
// destNPDUData - destination NPDU, kodira se prema podacima iz srcNPDUData ako postoji
// srcNPDUData - source NPDU. Ako postoji onda se destNPDU->dest->net/len kodira prema ovom
//======================================================================================//
int I_cNPDUEncoder::EncodeNPDU(uint8_t *npdu, BACNET_NPDU_DATA *npduData, BACNET_ADDRESS *dest, BACNET_ADDRESS *src)
{
// gData->IncInd();
 int len = 0;



 npdu[len++] = npduData->protocolVersionNumber;
 npdu[len++] = 0;   // postavi kontrolni oktet na 0
 uint8_t *controlOctet = &npdu[1];



 if (npduData->network_layer_message)
    {
     *controlOctet = *controlOctet | SET_BIT(7);
    }
 //*controlOctet = *controlOctet | SET_BIT(6);  // rezerviran - biti ce 0

  /*
  Bit 5: Destination specifier where:
  0 = DNET, DLEN, DADR, and Hop Count absent
  1 = DNET, DLEN, and Hop Count present
  DLEN = 0 denotes broadcast MAC DADR and DADR field is absent
  DLEN > 0 specifies length of DADR field
  */
  if (dest)
     {
      //gLog->AppendLog("EncodeNPDU: dest_len %d, dest_net %d",dest->len,dest->net);
      if (dest->len || dest->networkNumber)
         {
          *controlOctet = *controlOctet | SET_BIT(5);

          len += gDec->encode_unsigned16(&npdu[len],dest->networkNumber);
          npdu[len++] = dest->len;

          if (dest->len > 0)  // nije broadcast, kodiraj adresu
             {
              for (int c=0;c<dest->len;c++)
                  {
                   npdu[len++] = dest->adr[c];
                  }
             }
         }
     }else {
            //gLog->AppendLog("EncodeNPDU: dest = NULL");
           }

  // *controlOctet = *controlOctet | SET_BIT(4);  // rezerviran - biti ce 0


  /*
  Bit 3: Source specifier where:
  0 =  SNET, SLEN, and SADR absent
  1 =  SNET, SLEN, and SADR present
  SLEN = 0 Invalid
  SLEN > 0 specifies length of SADR field
  */
  if (src)
     {
//      gLog->AppendLog("EncodeNPDU: src_len %d, src_net %d",src->len,src->networkNumber);
      if (src->len || src->networkNumber)
         {
          *controlOctet = *controlOctet | SET_BIT(3);

          // postavi source
          len += gDec->encode_unsigned16(&npdu[len],src->networkNumber);
          npdu[len++] = src->len;

          if (src->len > 0)  // nije broadcast, kodiraj adresu
             {
              for (int c=0;c<src->len;c++)
                  {
                   npdu[len++] = src->adr[c];
                  }
             }

         }
     }else {
            //gLog->AppendLog("EncodeNPDU: src = NULL");
           }


  // Bit 2: The value of this bit corresponds to the data_expecting_reply
  // parameter in the N-UNITDATA primitives.
  if (npduData->isExpectingReply)
     {
      *controlOctet = *controlOctet | SET_BIT(2);
     }

   npdu[1] |= (npduData->priority & 0x03);
 //pNPDU[len++] = 0;       // broadcast

 if (dest)
    {
     if (dest->len || dest->networkNumber)
        {
         npdu[len++] = npduData->hopCount;
        }
    }


 //gData->DecInd();
 return len;   //vracam broj zauzetih bytova u NPDUu

}
//______________________________________________________________________________________\\




//======================================================================================//
//========================[ ENCODE BVLC WRITE TABLE ]===================================//
//======================================================================================//
int I_cNPDUEncoder::EncodeBVLC_WriteBDT(uint8_t *buffer, BACNET_BVLC_BDT *table)
{
 int len = 0;
 uint8_t table_buffer[1024];
 int table_len = 0;
 uint16_t fin_len = 0;
 BACNET_BVLC bvll;
 bvll.bacnet_vlc_type = BACNET_BVLC_TYPE_BACNET_IP;
 bvll.bacnet_vlc_function = BVLC_WRITE_BROADCAST_DISTRIBUTION_TABLE;

 buffer[len++] = bvll.bacnet_vlc_type;
 buffer[len++] = bvll.bacnet_vlc_function;


 int entry_count = table->vTableEntryList.size();
 for (int c=0;c<entry_count;c++)
     {
      BACNET_BVLC_BDT_ENTRY *entry = &table->vTableEntryList[c];
      table_buffer[table_len++] = entry->address[0];
      table_buffer[table_len++] = entry->address[1];
      table_buffer[table_len++] = entry->address[2];
      table_buffer[table_len++] = entry->address[3];

      table_len += gDec->encode_unsigned16(&table_buffer[table_len],entry->port);

      table_buffer[table_len++] = entry->mask[0];
      table_buffer[table_len++] = entry->mask[1];
      table_buffer[table_len++] = entry->mask[2];
      table_buffer[table_len++] = entry->mask[3];

     }

 uint16_t bvlc_len = len + 2 + table_len; // +2 jer se racuna i ovaj dolje len
 len += gDec->encode_unsigned16(&buffer[len],bvlc_len);
 memcpy(&buffer[len],table_buffer,table_len);


 //len = htons(len);    // host to network short conv.
 //bvll.bacnet_vlc_len = len;



 //memcpy(buffer,&bvll,sizeof(BACNET_BVLC));

 return bvlc_len;
}
//______________________________________________________________________________________\\



//======================================================================================//
//========================[ ENCODE BVLC ]===============================================//
// TODO: htons na android port
//======================================================================================//
int I_cNPDUEncoder::EncodeBVLC(uint8_t *buffer, uint16_t len, int transmission)
{
 int blen = 0;
 BACNET_BVLC bvll;
 bvll.bacnet_vlc_type = BACNET_BVLC_TYPE_BACNET_IP;
 bvll.bacnet_vlc_function = transmission;


 buffer[blen++] = bvll.bacnet_vlc_type;
 buffer[blen++] = bvll.bacnet_vlc_function;

 // Nesto nije u redu ovdje

 //len = htons(len/* + sizeof(BACNET_BVLL)*/);    // host to network short conv.
 //len = len; // ptrovjeri htons android port
 
 bvll.bacnet_vlc_len = len;
 blen += gDec->encode_unsigned16(&buffer[blen], bvll.bacnet_vlc_len);



 //memcpy(buffer,&bvll,sizeof(BACNET_BVLC));

 return sizeof(BACNET_BVLC);
}
//______________________________________________________________________________________\\



int I_cNPDUEncoder::EncodeBVLC_RegisterFD(uint8_t *buffer, uint16_t ttl)
{
 int len =0;
        buffer[0] = BACNET_BVLC_TYPE_BACNET_IP;
        buffer[1] = BVLC_REGISTER_FOREIGN_DEVICE;

        /* The 2-octet BVLC Length field is the length, in octets,
           of the entire BVLL message, including the two octets of the
           length field itself, most significant octet first. */
        gDec->encode_unsigned16(&buffer[2], 6);
        gDec->encode_unsigned16(&buffer[4], ttl);
        len = 6;

 return len;
}



int I_cNPDUEncoder::EncodeBVLC_ReadFDT(uint8_t *buffer)
{
 int len = 0;

        buffer[0] = BACNET_BVLC_TYPE_BACNET_IP;
        buffer[1] = BVLC_READ_FOREIGN_DEVICE_TABLE;
        /* The 2-octet BVLC Length field is the length, in octets,
           of the entire BVLL message, including the two octets of the
           length field itself, most significant octet first. */
        gDec->encode_unsigned16(&buffer[2], 4);
        len = 4;
 return len;
}


int I_cNPDUEncoder::EncodeBVLC_DeleteFDTEntry(uint8_t *buffer, uint32_t address, uint16_t port)
{
 int len = 0;


        buffer[0] = BACNET_BVLC_TYPE_BACNET_IP;
        buffer[1] = BVLC_DELETE_FOREIGN_DEVICE_TABLE_ENTRY;

        gDec->encode_unsigned16(&buffer[2], 10);
        /* FDT Entry */
        gDec->encode_unsigned32(&buffer[4], address);
        gDec->encode_unsigned16(&buffer[8], port);
        len = 10;



 return len;
}




