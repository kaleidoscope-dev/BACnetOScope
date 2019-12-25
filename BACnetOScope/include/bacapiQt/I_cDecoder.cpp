#include "I_cDecoder.h"
#include "I_cBACLog.h"
#include "bacapi.h"
#include "I_cWiresharkIO.h"
#include <string.h>

 I_cWiresharkIO wio;

I_cDecoder::I_cDecoder()
{
 networkHandler_ = NULL;
}




I_cDecoder::~I_cDecoder()
{
}





void I_cDecoder::Init(I_cAPDUDecoder *ptrApdu, I_cNPDUDecoder *ptrNpdu,I_cNetworkHandler *ptrNetHandler)
{
 decoderAPDU_ = ptrApdu;
 decoderNPDU_ = ptrNpdu;
 networkHandler_ = ptrNetHandler;

}





void I_cDecoder::HandleDataPacket(uint8_t *packet, BACNET_ADDRESS sourceAddress, size_t packetSize, BACNET_DATALINK dataLink)
{
 uint8_t *buffer = NULL;
 int curLen = 0;
 int len = 0;


 BACNET_NPDU_DATA *npdu = BACAPI_CreateNPDUData(0,0,&sourceAddress,dataLink);   // stvori novi NPDU
 BACNET_BVLC bvlc = {0};
 BACNET_LLC llc;
 
 switch (dataLink)
        {
         case BACNET_DATALINK_BACIP:
         {
          int npdu_len = 0;
          int apdu_len = 0;
          uint16_t bvlc_result = 0;

          npdu_len = DecodeNPDUDataBACIP(packet,npdu,&bvlc,npdu->remote);
          if (npdu_len == 0)
             {
              // ova poruka ne sadrzava NPDU niti APDU dio.
              BACAPI_DeleteNPDUData(npdu);
              break;
             }
          apdu_len = packetSize - npdu_len;

          // ako je routing_enabled onda zanemari ovaj check
          // upotrebljuje se samo Ako se koristi Lookout. 
          // TODO: razdvojiti API i Stack
          if (decoderAPDU_->routingEnabled_ == false)
             {
			  if (npdu->network_layer_message)
				 {
				  //int apdu_len = packetSize - npdu_len;
				  networkHandler_->HandleNetworkLayerMessage(npdu,&packet[npdu_len],apdu_len);
				  break;
				 }
			 }

          // moramo stvoriti novi buffer za APDU dekoder jer neznamo tko trenutno koristi dekoder
          // router MAIN ili mi od ovdje
          
          uint8_t *apdu = NULL;
          apdu = new uint8_t[apdu_len];
          memcpy(apdu,&packet[npdu_len],apdu_len);
          
          // TODO: provjeriti ovo s razlicitim dolaznim NN
          if (npdu->dadr)
             {
              npdu->remote->networkNumber = npdu->dadr->networkNumber;
             }


          decoderAPDU_->HandleAPDU(apdu,npdu,apdu_len, bvlc.bacnet_vlc_len);
          
          // ovo je paket stvoren od ip clijenta. Ovdje ga sigurno mozemo izbrisati
          delete []packet;  
          break;
         }

         case BACNET_DATALINK_ETH802:
         {
          int npdu_len = 0;

          npdu_len = DecodeNPDUDataETH802(packet, npdu, &llc,npdu->remote);
          if (npdu_len == 0)
             {
              BACAPI_DeleteNPDUData(npdu);
              break;
             }

          int apdu_len = packetSize - npdu_len;
          uint8_t *apdu = NULL;

          try {
               apdu = new uint8_t[apdu_len];
              }catch (std::bad_alloc& ba)
                     {
                      gLog->AppendLog("Decoder::HandleDataPacket: caught bad_alloc. [%s]",ba.what());
                      delete []packet;
                      BACAPI_DeleteNPDUData(npdu);
                      return;
                     }

          memcpy(apdu,&packet[npdu_len],apdu_len);
          delete []packet;

          decoderAPDU_->HandleAPDU(apdu,npdu,apdu_len,bvlc.bacnet_vlc_len);   
          //decoderAPDU_->DecodeAPDU(apdu,npdu,apdu_len);
          break;
         }

        case BACNET_DATALINK_MSTP:
        {
        
         int npdu_len = decoderNPDU_->DecodeNPDU(packet,npdu);
         if (npdu_len == 0)
            {
             // stvoreno u MasterStateMachine
             delete []packet;
             // stvoreno ovdje
             BACAPI_DeleteNPDUData(npdu);
             break;
            }


         int apdu_len = packetSize - npdu_len;

         
         uint8_t *apdu = NULL;
         apdu = new uint8_t[apdu_len];
         memcpy(apdu,&packet[npdu_len],apdu_len);

         npdu->dataLink = BACNET_DATALINK_MSTP;

         //if (npdu->dadr)  // complex_ack nema dadr, ali I-AM ima
             //sourceAddress->networkNumber = npdu->dadr->networkNumber;


         decoderAPDU_->HandleAPDU(apdu,npdu,apdu_len, 0);
          
         delete []packet;
         //BACAPI_DeleteNPDUData(npdu);
         break;
        }
        default: break;
       }





}


void I_cDecoder::ProcessBVLC(BACNET_BVLC *bvlc)
{

}



int I_cDecoder::DecodeNPDUDataBACIP(uint8_t *packet, BACNET_NPDU_DATA *npdu, BACNET_BVLC *bvlc, BACNET_ADDRESS *source)
{
 uint16_t bvlc_result = 0;
 int len  = 0;
 len =  decoderNPDU_->DecodeBVLC(packet, bvlc, &bvlc_result, source);

 // ovdje provjeri da li je ovo samo BVLC paket
 if (bvlc_result != BVLC_RESULT_CONTAINS_NPDU)
    {
     // pusti npdu na miru
     return 0;
    }

 len += decoderNPDU_->DecodeNPDU(&packet[len],npdu);
 return len;
}






int I_cDecoder::DecodeNPDUDataETH802(uint8_t *packet, BACNET_NPDU_DATA *npdu, BACNET_LLC *llc, BACNET_ADDRESS *source)
{
 int len  = 0;


 npdu->dadr = 0;
 npdu->sadr = 0;


 len =  decoderNPDU_->DecodeLLC(packet, llc);
 len += decoderNPDU_->DecodeNPDU(&packet[len],npdu);

 return len;
}

















