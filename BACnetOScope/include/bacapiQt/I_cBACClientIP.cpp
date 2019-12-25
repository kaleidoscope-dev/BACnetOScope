#include "I_cBACClientIP.h"
#include "I_cDecoder.h"
#include "I_cBACLog.h"
//#include "I_cBBMD.h"
#include "I_cSettings.h"
#include "I_cBACdec.h"




I_cBACClientIP::I_cBACClientIP()
{
 port_ = 0;
 encoderNPDU_ = NULL;
 encoderAPDU_ = NULL;
 decoder_ = NULL;
}


I_cBACClientIP::~I_cBACClientIP()
{
 //delete ace_;
}






BACAPI_ERROR I_cBACClientIP::Init(u_short port, I_cNPDUEncoder *npdu, I_cAPDUEncoder *apdu, I_cNPDUDecoder *decnpdu, I_cAPDUDecoder *decapdu, I_cDecoder *dec)
{
/*
 gLog->AppendLog("I_cBACClientIP::Init: port = %d",port);
 BACAPI_ERROR err;
 ace_ = new I_cAce();
 port_ = port;
 err = ace_->Init(port_);
 if (err != BACAPI_OK)
    {
     return err;
    }

 encoderAPDU_ = apdu;
 encoderNPDU_ = npdu;
 decoderNPDU_ = decnpdu;
 decoderAPDU_ = decapdu;
 decoder_ = dec;

#ifdef BACAPI_USES_BBMD
 gBBMD->Init(ace_,gSettings->GetBBMDEnable());
#endif

 gLog->AppendLog("I_cBACClientIP::Init: return BACAPI_OK");
 */
 return BACAPI_OK;
}


/*

BACAPI_ERROR I_cBACClientIP::WhoIs(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_ADDRESS *adr, bool params)
{
 int len = 4;   // NPDU kodiranje pocinje poslje BVLC-a
 uint8_t buffer[BACNET_MAX_APDU_IP] = {0};
 BACNET_NPDU_DATA npduData = {0};


 encoderNPDU_->SetNPDUData(&npduData,false,BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr, NULL);
 len += encoderAPDU_->WhoIsAPDU(&buffer[len], devInstanceFrom, devInstanceTo, params);
 encoderNPDU_->EncodeBVLC(&buffer[0],len,BVLC_ORIGINAL_BROADCAST_NPDU);   // prva 4 byta su BVLC
 //encoderNPDU_->EncodeBVLC(&buffer[0],len,BVLC_ORIGINAL_UNICAST_NPDU);   // prva 4 byta su BVLC

#ifdef BACAPI_USES_BBMD
 if (gBBMD->Enabled())
    {
     BACNET_BVLC_BDT *bdt = gBBMD->GetBBMDTable();
     size_t count = bdt->vTableEntryList.size();
     for (size_t c=0;c<count;c++)
         {
          BACNET_BVLC_BDT_ENTRY *entry = &bdt->vTableEntryList[c];
          BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(entry->address,entry->port,0);

		  int size = ace_->Send(buffer,len,adr);
		  if (size == 0 || size < 0)
			 {
			  BACAPI_Message("Error while sending Who-Is to BDT Entry.","Error while sending Who-Is to BDT Entry",BACAPI_SEVERITY_ERROR);
			 }
         }
    }
#endif

 int size = ace_->Send(buffer,len,adr);
 gLog->AppendLog("I_cBACClientIP::WhoIs: Sending WhoIs, retsize %d",size);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClientIP::WhoHas(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_OBJECT_ID *oid, BACNET_ADDRESS *adr)
{
 int len = 4;   // NPDU kodiranje pocinje poslje BVLC-a
 uint8_t buffer[BACNET_MAX_APDU_IP] = {0};
 BACNET_NPDU_DATA npduData = {0};


 encoderNPDU_->SetNPDUData(&npduData,false,BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr, NULL);
 len += encoderAPDU_->EncodeWhoHas(&buffer[len], devInstanceFrom, devInstanceTo, oid);
 encoderNPDU_->EncodeBVLC(&buffer[0],len,BVLC_ORIGINAL_BROADCAST_NPDU);   // prva 4 byta su BVLC


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::ReadProperty(BACNET_OBJECT_ID *oid, BACNET_PROPERTY_ID prop, int arrayIndex, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,npdu_dest,npdu_src);
 len += encoderAPDU_->ReadPropAPDU(&buffer[len],oid, prop, arrayIndex, invokeId,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);



 //BACNET_ADDRESS *sendToAddr = BACAPI_CreateBACnetAddress(ipDest,47808,65535);   // TODO: ne hardkodirani port!!

 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     //delete sendToAddr;
     return BACAPI_ERROR_CLIENT_SEND_FAILED;

    }

 //delete sendToAddr;
 return BACAPI_OK;

}


BACAPI_ERROR I_cBACClientIP::ReadRange(BACNET_READ_RANGE *range, uint8_t invokeId,  BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);
 len += encoderAPDU_->EncodeReadRange(&buffer[len],invokeId,range,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::AtomicReadFileRec(BACNET_OBJECT_ID *oid, uint8_t invoke_id,uint16_t rec_start_pos, uint32_t rec_count, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);
 len += encoderAPDU_->EncodeAtomicReadFileRec(&buffer[len],oid,invoke_id,rec_start_pos, rec_count,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClientIP::AtomicReadFileOct(BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t file_start_pos, uint32_t octet_count, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);
 len += encoderAPDU_->EncodeAtomicReadFileOct(&buffer[len],oid,invoke_id,file_start_pos, octet_count,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;

}


BACAPI_ERROR I_cBACClientIP::AtomicWriteFileRec(BACNET_OBJECT_ID *oid, uint8_t invoke_id, BACNET_FILE *file_data, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);
 len += encoderAPDU_->EncodeAtomicWriteFileRec(&buffer[len],oid,invoke_id,file_data,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}

BACAPI_ERROR I_cBACClientIP::AtomicWriteFileOct(BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t file_start_pos, uint32_t octet_count, BACNET_FILE *file_data, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);
 len += encoderAPDU_->EncodeAtomicWriteFileOct(&buffer[len],oid,invoke_id,file_start_pos, octet_count, file_data,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClientIP::AtomicWriteAck(uint8_t invoke_id, uint8_t access, uint16_t file_start_pos, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);
 len += encoderAPDU_->EncodeAtomicWriteAck(&buffer[len],invoke_id, access, file_start_pos,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


// NOTE:
// ReadPropertyALL() je posebna implementacija ReadPropertyMultiple s specijalni PROP_ALL parametrom.
// Cita sve propertije u specificiranom objektu.
// Target uredjaj mora podrzavati ReadPropertyMultiple i ako ima puno propertija, segmentaciju
BACAPI_ERROR I_cBACClientIP::ReadPropertyALL(BACNET_OBJECT_ID *oid, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,npdu_dest,npdu_src);  // NOTE: provjeriti ovaj zadnji NULL
 len += encoderAPDU_->ReadPropALLAPDU(&buffer[len],oid, invokeId,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;

}






BACAPI_ERROR I_cBACClientIP::WriteProperty(BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *prop,int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,npdu_dest,npdu_src);
 len += encoderAPDU_->EncodeWritePropAPDU(&buffer[len],oid, prop,invokeId,BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);



 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;

}




BACAPI_ERROR I_cBACClientIP::GetAlarmSummary(int invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);  // NOTE: provjeriti ovaj zadnji NULL
 len += encoderAPDU_->EncodeGetAllarmSummaryAPDU(invokeId, &buffer[len],BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);



 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::GetEnrollmentSummary(BACAPI_ENROLLMENT_SUMMARY *summary, int invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);  // NOTE: provjeriti ovaj zadnji NULL
 len += encoderAPDU_->EncodeEnrollmentSummaryAPDU(&buffer[len], summary, invokeId, BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);



 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::GetEventInformation(int invokeId, BACNET_OBJECT_ID *oid, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


 encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);  // NOTE: provjeriti ovaj zadnji NULL
 len += encoderAPDU_->EncodeGetEventInformationAPDU(&buffer[len], oid, invokeId, BACNET_MAX_APDU_IP);
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);



 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::ReadBDT(BACNET_ADDRESS *dest)
{
 int len = 0;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 //encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 //len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,dest,NULL);  // NOTE: provjeriti ovaj zadnji NULL


 //for (int c=0;c<6;c++)
 //    {
 //     buffer[c] = dest->adr[c];
 //    }
 //len = 6;
 len += encoderNPDU_->EncodeBVLC(&buffer[len],len,BVLC_READ_BROADCAST_DISTRIBUTION_TABLE);   // prva 4 byta su BVLC


 buffer[len-1] = 0x04;

 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::WriteBDT(BACNET_ADDRESS *dest, BACNET_BVLC_BDT *table)
{
 int len = 0;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


 len += encoderNPDU_->EncodeBVLC_WriteBDT(&buffer[len],table);   // prva 4 byta su BVLC


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::RegisterFD(BACNET_ADDRESS *dest,uint16_t ttl)
{
 int len = 0;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };

 len += encoderNPDU_->EncodeBVLC_RegisterFD(&buffer[len],ttl);   // prva 4 byta su BVLC


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;


}


BACAPI_ERROR I_cBACClientIP::ReadFDT(BACNET_ADDRESS *dest)
{
 int len = 0;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };

 len += encoderNPDU_->EncodeBVLC_ReadFDT(&buffer[len]);   // prva 4 byta su BVLC


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;


}



BACAPI_ERROR I_cBACClientIP::DeleteFDTEntry(BACNET_ADDRESS *dest,uint32_t address, uint16_t port)
{
 int len = 0;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };

 len += encoderNPDU_->EncodeBVLC_DeleteFDTEntry(&buffer[len],address,port);   // prva 4 byta su BVLC


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




//======================================================================================//
//===========================[ SUBSCRIBE COV PROPERTY]==================================//
//======================================================================================//
BACAPI_ERROR I_cBACClientIP::SubscribeCOVProperty(BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *net,  BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeSubscribeCOVPropertyAPDU(&buffer[len],cov,invokeId,BACNET_MAX_APDU_IP);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;


}
//______________________________________________________________________________________//





//======================================================================================//
//===========================[ SUBSCRIBE COV  ]=========================================//
//======================================================================================//
BACAPI_ERROR I_cBACClientIP::SubscribeCOV(BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *net,  BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeSubscribeCOVAPDU(&buffer[len],cov,invokeId,BACNET_MAX_APDU_IP);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;


}
//______________________________________________________________________________________//




BACAPI_ERROR I_cBACClientIP::SegmentACK(BACNET_SEGMENT_ACK *ack, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeSegmentACK(&buffer[len],ack);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;


}





BACAPI_ERROR I_cBACClientIP::CreateObject(BACNET_OBJECT_ID *oid, uint8_t invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeCreateObject(&buffer[len],oid->type,oid->instance,invokeId,BACNET_MAX_APDU_IP);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;


}





BACAPI_ERROR I_cBACClientIP::DeleteObject(BACNET_OBJECT_ID *oid, uint8_t invokeId, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeDeleteObject(&buffer[len],oid->type,oid->instance,invokeId,BACNET_MAX_APDU_IP);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;


}



BACAPI_ERROR I_cBACClientIP::TimeSync(BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeTimeSync(&buffer[len],dateTime);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;

}



BACAPI_ERROR I_cBACClientIP::TimeSyncUTC(BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, true, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,net,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeTimeSyncUTC(&buffer[len],dateTime);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;

}


BACAPI_ERROR I_cBACClientIP::Iam(BACNET_DEVICE *device, BACNET_ADDRESS *adr, BACNET_ADDRESS *src)
{
 gLog->AppendLog("I_cBACClientIP::Iam:...");
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


 encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);


 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr,src);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.



 len += encoderAPDU_->EncodeIAm(&buffer[len],device);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);

 ////gLog->AppendLog("BACClientIP::Iam: Transmitting!\n");
 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     gLog->AppendLog("FAIL\n");
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 gLog->AppendLog("SUCCESS\n");
 return BACAPI_OK;

}


BACAPI_ERROR I_cBACClientIP::IHave(BACNET_OBJECT_ID device, BACNET_OBJECT_ID object, string name, BACNET_ADDRESS *adr)
{
 gLog->AppendLog("I_cBACClientIP::IHave:\n");

 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };

 encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.

 len += encoderAPDU_->EncodeIHave(&buffer[len],device,object,name);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }



 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::ReadProperty_ACK(BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *property, int arrayIndex, BACNET_ADDRESS *adr, int invokeId)
{
 int len = 4;
 int ret = 0;

 // +100 za svaki slucaj ako enkodiranje predje granicu da se ne corrupta buffer
 uint8_t buffer[BACNET_MAX_APDU_IP + 100] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.

 ret = encoderAPDU_->EncodeReadPropertyACK(&buffer[len], object, property, arrayIndex, invokeId);
 if (ret < 0)
    {
     return (BACAPI_ERROR)ret;
    }
 len += ret;

 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::ReadRange_ACK(BACNET_OBJECT_ID *oid , BACNET_OBJECT_PROPERTY *property,  BACNET_READ_RANGE *range, uint8_t invokeId,  BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 int len = 4;
 int ret = 0;

 // +100 za svaki slucaj ako enkodiranje predje granicu da se ne corrupta buffer
 uint8_t buffer[BACNET_MAX_APDU_IP + 100] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.

 // Postoje 2 razlicita PROP_LOG_BUFFER u odnosu na objekt
 if (oid->type == OBJECT_TREND_LOG)
    {
     ret = encoderAPDU_->EncodeReadRangeACK(&buffer[len], oid, property, range, invokeId);
    }else {
           ret = encoderAPDU_->EncodeReadRangeMultiACK(&buffer[len], oid, property, range, invokeId);
          }


 if (ret < 0)
    {
     return (BACAPI_ERROR)ret;
    }
 len += ret;

 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::AtomicReadFile_ACK(BACNET_FILE *bac_file, uint8_t invoke, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeAtomicReadFileACK(&buffer[len],invoke, bac_file);

        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::ReadPropertyMultiple_ALL_ACK(BACNET_OBJECT *object, BACNET_ADDRESS *adr, int invokeId)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeReadPropertyMultipleALLACK(&buffer[len], object, invokeId);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}








BACAPI_ERROR I_cBACClientIP::Simple_ACK(BACNET_CONFIRMED_SERVICE service, int invokeId, BACNET_ADDRESS *adr)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeSimpleACK(&buffer[len], service, invokeId);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClientIP::AcknowledgeAlarm(BACNET_ACK_ALARM ack_alarm, int invokeId, BACNET_ADDRESS *dest)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeAckAlarm(&buffer[len], ack_alarm, invokeId);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::Error_ACK(BACNET_ERROR bacerr,int invokeId, BACNET_CONFIRMED_SERVICE service, BACNET_ADDRESS *adr)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeErrorACK(&buffer[len], bacerr,service,invokeId);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::Reject_ACK(BACNET_REJECT_CODE code,int invokeId, BACNET_ADDRESS *adr)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeRejectACK(&buffer[len], code,invokeId);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClientIP::Abort_ACK(BACNET_ABORT abortcode, BACNET_ADDRESS *adr)
{
 int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeAbortACK(&buffer[len], abortcode.reason,abortcode.invoke_id);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}

uint32_t I_cBACClientIP::GetBroadcastAddress(void)
{
 uint32_t ret = ace_->GetBroadcastAddress();
 return ret;
}



BACAPI_ERROR I_cBACClientIP::UnconfirmedEventNotification(BACNET_ADDRESS *adr, BACNET_EVENT_NOTIFICATION *eventNotification)
{

int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeUnconfirmedEventNotification(&buffer[len],eventNotification);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}





BACAPI_ERROR I_cBACClientIP::DeviceCommunicationControl(BACNET_ADDRESS *adr, uint16_t duration, uint32_t enable, string password)
{

int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeDeviceCC(&buffer[len],duration,enable,password);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClientIP::ReinitDevice(BACNET_ADDRESS *adr, uint32_t state, string password)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,adr,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 len += encoderAPDU_->EncodeReinitDevice(&buffer[len],state,password);
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);


 int size = ace_->Send(buffer,len,adr);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}





BACAPI_ERROR I_cBACClientIP::WhoIs_RouterToNetwork(BACNET_ADDRESS *dest)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255,true);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 buffer[len++] = NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK;
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_BROADCAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::WhoIs_RouterToNetwork(uint16_t dnet, BACNET_ADDRESS *src, BACNET_ADDRESS *dest)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255,true);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,src);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 buffer[len++] = NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK;
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_BROADCAST_NPDU);


 len += gDec->encode_unsigned16(&buffer[len],dnet);

 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClientIP::WhoIs_RouterToNetwork(BACNET_ADDRESS *src, BACNET_ADDRESS *dest)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255,true);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,src);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 buffer[len++] = NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK;
        encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_BROADCAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}

BACAPI_ERROR I_cBACClientIP::I_Am_RouterToNetwork(vector<uint16_t> dnet_list, BACNET_ADDRESS *dest)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255,true);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 buffer[len++] = NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK;

 for (uint32_t c = 0;c < dnet_list.size();c++)
     {
      len += gDec->encode_unsigned16(&buffer[len],dnet_list[c]);
     }

 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_BROADCAST_NPDU);


 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClientIP::Init_RoutingTable(BACNET_ADDRESS *dest)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255,true);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 buffer[len++] = NETWORK_MESSAGE_INIT_RT_TABLE;

 buffer[len++] = 0;
 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);



 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClientIP::InitRoutingTableACK(vector<BACAPI_ROUTER_PORT*>vPortList, BACNET_ADDRESS *dest)
{
int len = 4;
 uint8_t buffer[BACNET_MAX_APDU_IP] = { 0 };
 BACNET_NPDU_DATA npduData = { 0 };


        encoderNPDU_->SetNPDUData(&npduData, false, BACNET_MESSAGE_PRIORITY_NORMAL,255,true);
 len += encoderNPDU_->EncodeNPDU(&buffer[len],&npduData,NULL,NULL);   // prva 4 byta je duzina BVLCa. NPDU pocinje na 4 bytu.
 buffer[len++] = NETWORK_MESSAGE_INIT_RT_TABLE_ACK;

 buffer[len++] = (uint8_t)vPortList.size();

 for (size_t c=0;c<vPortList.size();c++)
     {
      BACAPI_ROUTER_PORT *port = vPortList[c];

      len += gDec->encode_unsigned16(&buffer[len],port->dnet);  // dnet
      buffer[len++] = c+1;  // port id
      buffer[len++] = 0;  // port info len / port info
     }

 encoderNPDU_->EncodeBVLC(buffer,len,BVLC_ORIGINAL_UNICAST_NPDU);



 int size = ace_->Send(buffer,len,dest);

 if (size == 0 || size < 0)
    {
     return BACAPI_ERROR_CLIENT_SEND_FAILED;
    }

 return BACAPI_OK;
}



#ifdef BACAPI_BUILD_WIN32
DWORD I_cBACClientIP::Run(LPVOID)
#else
int I_cBACClientIP::svc(void)
#endif
{
 ACE_Time_Value t(1,0);
 int ierr = 0;
 uint8_t recvBuffer[MAX_PDU];
 ACE_INET_Addr remote_addr;

 while(1)
 {
  // cekaj odgovor
  ssize_t recvCount = ace_->GetDeviceSocket()->recv(recvBuffer,MAX_PDU,remote_addr,NULL,&t);
  if (recvCount < 0)
     {
      continue;
     }else {

            // obradi poruku. Nemoj slusati poruke koji sam ja poslao
            uint32_t local_adr = ace_->GetLocalAddressInt();
            uint32_t remote_adr = remote_addr.get_ip_address();

            unsigned short prtNum = remote_addr.get_port_number();
            if (local_adr != remote_adr)  // poruka nije od ovog racunala
               {
                uint8_t *packet = NULL;
                try {
                     packet = new uint8_t[recvCount];
                    }catch (std::bad_alloc& ba)
                           {
                            gLog->AppendLog("BACClientIP::Run: caught bad_alloc. [%s]",ba.what());
                            packet = NULL;
                           }

                if (packet)
                   {
					memcpy(packet,recvBuffer,recvCount);
					BACNET_ADDRESS *src_adr = BACAPI_CreateBACnetAddress(remote_adr,prtNum,0);
					BACNET_ADDRESS *dest_adr = BACAPI_CreateBACnetAddress(local_adr,prtNum,0);

#ifdef BACAPI_ENABLE_WIRESHARK_BACKUP
					if (wireshark_)
					   {
					    wireshark_->WritePDUBufferIP(packet,src_adr->adr,dest_adr->adr,recvCount);
					   }
#endif


					decoder_->HandleDataPacket(packet, *src_adr, recvCount, BACNET_DATALINK_BACIP);

					delete dest_adr;
					delete src_adr;
				   }
               }
           }
 }

 return 0;
}




*/