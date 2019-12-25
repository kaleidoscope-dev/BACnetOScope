#include "I_cBACNet.h"
#include "I_cBACdec.h"
#include "I_cBACLog.h"

//#include "I_cSettings.h"
//#include "I_cBTLTestClientIP.h"
//#include "I_cBBMD.h"

I_cBACnet::I_cBACnet()
{

 bacClient_ = NULL;
 clientIP_ = NULL;

 encoderNPDU_ = NULL;
 encoderAPDU_ = NULL ;
 decoderNPDU_ = NULL ;
 decoder_ = NULL;
 networkHandler_ = NULL;

 //btlClient = NULL;
 
}


I_cBACnet::~I_cBACnet()
{
/*
 if (clientIP_) delete clientIP_;
 if (client802_) delete client802_;
 if (clientMSTP_) delete clientMSTP_;
 if (bacClient_) delete bacClient_;


 if (gDec) delete gDec;
 if (encoderNPDU_) delete encoderNPDU_;
 if (encoderAPDU_) delete encoderAPDU_;
 if (decoderNPDU_) delete decoderNPDU_;
 if (decoderAPDU_) delete decoderAPDU_;
 if (decoder_) delete decoder_;



#ifdef BACAPI_TESTING_ENABLED
 if (btlClient) delete btlClient;
#endif

 if (networkHandler_) delete networkHandler_;

 */
}

void I_cBACnet::Init(bool routing_enable)
{

 gDec = new I_cBACdec();
 encoderNPDU_ = new I_cNPDUEncoder();
 encoderAPDU_ = new I_cAPDUEncoder();
 decoderNPDU_ = new I_cNPDUDecoder();
 decoderAPDU_ = new I_cAPDUDecoder();
 decoderAPDU_->Init(routing_enable);  // create serviceHandler
 networkHandler_ = new I_cNetworkHandler();

 decoder_ = new I_cDecoder();
 decoder_->Init(decoderAPDU_, decoderNPDU_,networkHandler_);
 

}



I_cBACClientQt *I_cBACnet::CreateQtIPClient(unsigned short port, bool wire)
{
 BACAPI_ERROR err;
 clientQt_ = new I_cBACClientQt();
 err = clientQt_->Init(port,encoderNPDU_, encoderAPDU_, decoderNPDU_, decoderAPDU_,decoder_,wire);
 if (err != BACAPI_OK)
    {
		 return NULL;
		}
 return clientQt_;
}


/*
I_cNPDUEncoder *I_cBACnet::GetNPDUEncoder(void)
{
 return encoderNPDU_;
}


I_cAPDUDecoder *I_cBACnet::GetAPDUDecoder(void)
{
 return decoderAPDU_;
}



BACAPI_ERROR I_cBACnet::InitInterface(bool ip, bool eth, bool mstp)
{
 gLog->AppendLog("I_cBACnet::InitInterface:");
 BACAPI_ERROR err = BACAPI_OK;

 if (ip)  // BACnetIP
    {
     err = CreateIPClient();
     if (err)
        {
         gLog->AppendLog("I_cBACnet::InitInterface: Error Creating BACnet/IP Interface, %d",err);
         clientIP_ = NULL;
        }
    }

 if (eth) // BACnet Ethernet
    {
     err = CreateEthClient();
     if (err)
        {
         gLog->AppendLog("I_cBACnet::InitInterface: Error Creating Ethernet Interface, %d",err);
         client802_ = NULL;
        }
    }

// TODO: ovo nesmije biti ovdje
#ifndef BRIGHTCORE_BACNET_LOOKOUT_ESSENTIAL
 if (mstp)
    {
     err = CreateMSTPClient();
     if (err)
        {
         gLog->AppendLog("I_cBACnet::InitInterface: Error Creating MSTP Interface, %d",err);
         clientMSTP_ = NULL;
        }
    }
#endif


 if (clientIP_ == NULL && client802_ == NULL && clientMSTP_ == NULL)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }



 return BACAPI_OK;
}




BACAPI_ERROR I_cBACnet::CreateBACnetClient(void)
{
 gLog->AppendLog("I_cBACnet::CreateBACnetClient:");
 if (clientIP_ == NULL && client802_ == NULL && clientMSTP_ == NULL)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 bacClient_ = new I_cBACClient();
 gLog->AppendLog("I_cBACnet::CreateBACnetClient: Initializing Client");
 bacClient_->Init(client802_,clientIP_,clientMSTP_);

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACnet::CreateIPClient(void)
{
 BACAPI_ERROR err;
 clientIP_ = new I_cBACClientIP();
 if (!gSettings)
    {
     gLog->AppendLog("I_cBACnet::CreateIPClient: Unknwn settings");
    }
 uint16_t port = gSettings->GetBACPort();
 gLog->AppendLog("I_cBACnet::CreateIPClient: PORT = %d",port);
 err = clientIP_->Init(port,encoderNPDU_,encoderAPDU_,decoderNPDU_, decoderAPDU_,decoder_);


 if (err != BACAPI_OK)
    {
     gLog->AppendLog("I_cBACnet::CreateIPClient: Error init clientIP: %d",err);
     delete clientIP_;
     clientIP_ = NULL;
     return err;
    }

gLog->AppendLog("I_cBACnet::CreateIPClient: Starting IP thread.");
#ifdef BACAPI_BUILD_WIN32
     clientIP_->Start();
#else
     clientIP_->activate();
#endif



#ifdef BACAPI_BTL_TESTING_ENABLED   // privremeni tag - izmjeniti ovaj ifdef na if PRO?
 btlClient = new  I_cBTLTestClientIP();
 btlClient->Init(clientIP_->GetAce(),encoderNPDU_,encoderAPDU_,decoderNPDU_, decoderAPDU_,decoder_);
#endif


 return err;
}



BACAPI_ERROR I_cBACnet::CreateEthClient(void)
{
 gLog->AppendLog("I_cBACnet::CreateEthClient:");
 client802_ = new I_cBACClient802();
 int err = client802_->Init(encoderNPDU_,encoderAPDU_,decoderNPDU_, decoderAPDU_,decoder_);
 if (err < 0)
    {
     return BACAPI_ERROR_INTERFACE_ETH;
    }

#ifdef BACAPI_BUILD_WIN32
// client802_->SetId("client802");
 client802_->Start();
#endif


return BACAPI_OK;
}



BACAPI_ERROR I_cBACnet::CreateMSTPClient(void)
{
#ifndef BRIGHTCORE_BACNET_LOOKOUT_ESSENTIAL
 gLog->AppendLog("I_cBACnet::CreateMSTPClient:");
 clientMSTP_ = new I_cBACClientMSTP();
 int err = clientMSTP_->Init(encoderAPDU_, encoderNPDU_,decoder_);
 if (err < 0)
    {
     delete clientMSTP_;
     clientMSTP_ = NULL;
     return BACAPI_ERROR_INTERFACE_MSTP;
    }
gLog->AppendLog("I_cBACnet::CreateMSTPClient: Created MSTP Client");

#endif
 return BACAPI_OK;
}




I_cBACClient *I_cBACnet::GetClient(void)
{
 return bacClient_;
}



I_cDecoder *I_cBACnet::GetDecoder(void)
{
 return decoder_;
}





*/