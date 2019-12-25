

/*
  NOTE: 
  I_cDecoder koristi check od APDUDecoder objekta za provjeru da li se API koristi kao router da zaobidje 
  provjere sepcificne za API. npr: networkHandler objekt je specifican samo za lookout. routrinEnable
  varijabla od APDUDekodera se koristi za taj check. To je hack i moramo to prepraviti.
  
  TODO: razdvojiti API i Stack da se mogu koristiti odvojeno.
*/


#pragma once
#include "I_cAPDUDecoder.h"
#include "I_cNPDUDecoder.h"
#include "I_cNetworkHandler.h"


class I_cDecoder
{
 public: 
	    I_cDecoder();
	   ~I_cDecoder();
	   
	    void Init(I_cAPDUDecoder *ptrDec, I_cNPDUDecoder *ptrNpdu, I_cNetworkHandler *ptrNetHandler);
	   
	    void HandleDataPacket(uint8_t *packet, BACNET_ADDRESS sourceAddress, size_t packetSize, BACNET_DATALINK dataLink);
	    
 private:
  	     I_cAPDUDecoder *decoderAPDU_;
  	     I_cNPDUDecoder *decoderNPDU_;
  	     I_cNetworkHandler *networkHandler_;
  	     
  	     
  	     int DecodeNPDUDataBACIP(uint8_t *packet, BACNET_NPDU_DATA *npdu, BACNET_BVLC *bvlc, BACNET_ADDRESS *source);
  	     int DecodeNPDUDataETH802(uint8_t *packet, BACNET_NPDU_DATA *npdu, BACNET_LLC *llc, BACNET_ADDRESS *source);   
  	     
  	     void ProcessBVLC(BACNET_BVLC *bvlc);
};



