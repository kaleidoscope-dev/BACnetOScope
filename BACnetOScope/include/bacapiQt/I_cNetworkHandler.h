// dekodira i handla network layer pakete (router)
#pragma once
#include "bacapi.h"



class I_cNetworkHandler
{
 public:
		I_cNetworkHandler();
	   ~I_cNetworkHandler();
	   
 public:
        int HandleNetworkLayerMessage(BACNET_NPDU_DATA *npduData, uint8_t *packet, uint32_t packet_len);
        
 private:
         int DecodeDestinationNNList(BACNET_ROUTER *router, uint8_t *packet, uint32_t packet_len);
        
    	   
};