#include "I_cNetworkHandler.h"
#include "I_cBACdec.h"


I_cNetworkHandler::I_cNetworkHandler()
{
}


I_cNetworkHandler::~I_cNetworkHandler()
{
}



int I_cNetworkHandler::HandleNetworkLayerMessage(BACNET_NPDU_DATA *npduData, uint8_t *packet, uint32_t packet_len)
{

 int ret = 0;

 switch (npduData->npduMessageType)
        {
		 case NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK:
		 {
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_WHO_IS_ROUTER_TO_NETWORK;
          event->address = BACAPI_CreateBACnetAddress(npduData->remote);
          event->srcDataLink = npduData->dataLink;
          
          // provjeri da li smo dobili DNET koji se trazi za ovaj WHO-IS-RTN
          // ako smo dobili postavi address NN na trazeni
          if (packet_len > 0)
             {
              gDec->decode_unsigned16(packet,&event->address->networkNumber);
             }else event->address->networkNumber = 0xFFFF;
          
          BACAPI_PushEvent(event);
          BACAPI_Message_Received_Who_Is_Router(npduData->remote);
		  break;
         }

		 case NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK:
		 {
		  BACNET_ROUTER *router = new BACNET_ROUTER;
		  router->data_link = npduData->dataLink;
		  //router->device = NULL;
		  router->router_address = BACAPI_CreateBACnetAddress(npduData->remote);
		  DecodeDestinationNNList(router,packet,packet_len);
          BACAPI_Event *event = BACAPI_CreateEvent();
          event->type = BACAPI_EVENT_I_AM_ROUTER_TO_NETWORK;
          event->router = router;
          BACAPI_PushEvent(event);

          BACAPI_Message_Received_I_AM_Router(router);

		  break;
		 }

		 case NETWORK_MESSAGE_REJECT_MESSAGE_TO_NETWORK:

		 case NETWORK_MESSAGE_I_COULD_BE_ROUTER_TO_NETWORK:
		 case NETWORK_MESSAGE_ROUTER_BUSY_TO_NETWORK:
		 case NETWORK_MESSAGE_ROUTER_AVAILABLE_TO_NETWORK:
		 case NETWORK_MESSAGE_INIT_RT_TABLE:
		 case NETWORK_MESSAGE_INIT_RT_TABLE_ACK:
		 case NETWORK_MESSAGE_ESTABLISH_CONNECTION_TO_NETWORK:
		 case NETWORK_MESSAGE_DISCONNECT_CONNECTION_TO_NETWORK:
		  break;

		 default: break;
        }

 return 0;
}





int I_cNetworkHandler::DecodeDestinationNNList(BACNET_ROUTER *router, uint8_t *packet, uint32_t packet_len)
{
 int len = 0;
 uint16_t val = 0;
 while (len < packet_len)
       {
        len += gDec->decode_unsigned16(&packet[len],&val);
        router->vDestinationNetworkList.push_back(val);
       }

 return len;
}





