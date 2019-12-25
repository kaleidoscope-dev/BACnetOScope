
#pragma once
#ifdef BACAPI_BUILD_WIN32
// lookout sere bez ovog ovdje a device sere ako je definiran
//#include <stdafx.h>
#endif
#include "bacnet.h"
#include "I_cBACClient.h"
#include "I_cNPDUEncoder.h"
#include "I_cAPDUEncoder.h"
#include "I_cAPDUDecoder.h"
#include "I_cNetworkHandler.h"
#include "I_cBACClientQt.h"

class I_cBACnet
{
 public:
        I_cBACnet();
       ~I_cBACnet();
			 
        void Init(bool routing_enable);
        
       
        //BACAPI_ERROR InitInterface(bool ip, bool eth, bool mstp);
				I_cBACClientQt *CreateQtIPClient(unsigned short port, bool wire);

        /*
        BACAPI_ERROR CreateBACnetClient(void);
        
        I_cBACClient *GetClient(void);
        I_cNPDUEncoder *GetNPDUEncoder(void);
        I_cDecoder *GetDecoder(void);
        I_cAPDUDecoder *GetAPDUDecoder(void);


 private:
         BACAPI_ERROR CreateIPClient(void);
         BACAPI_ERROR CreateEthClient(void);
         BACAPI_ERROR CreateMSTPClient(void);

				 */
 private:

         I_cBACClient *bacClient_;
         I_cBACClientIP *clientIP_;
				 I_cBACClientQt *clientQt_;
				 I_cNPDUEncoder *encoderNPDU_;
         I_cAPDUEncoder *encoderAPDU_;
         I_cNPDUDecoder *decoderNPDU_;
         I_cAPDUDecoder *decoderAPDU_;
         I_cNetworkHandler *networkHandler_;
         I_cDecoder *decoder_;
				 
				 
};

extern I_cBACnet *gBACnet;
