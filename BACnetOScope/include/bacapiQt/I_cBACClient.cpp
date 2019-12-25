#include "I_cBACClient.h"
#include "I_cBACLog.h"



I_cBACClient::I_cBACClient()
{
 clientIP_ = NULL;
}



I_cBACClient::~I_cBACClient()
{

}



// NE ZVATI DIREKTNO, OVO SE POSTAVLJA U  I_cBACnet
void I_cBACClient::Init(I_cBACClientIP *clientIP)
{
 clientIP_ = clientIP;
}


/*
void I_cBACClient::ClearSegmentBuffer(void)
{
 if (clientIP_)
    {
     clientIP_->GetAPDUDec()->ClearSegmentBuffer();
    }
}

// DEFINICIJE SERVISA
BACAPI_ERROR I_cBACClient::WhoIs(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_ADDRESS *adr, bool params)
{

 if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
    {
     clientIP_->WhoIs(devInstanceFrom, devInstanceTo, adr, params);
    }

#ifdef BACAPI_BUILD_WIN32
 if (client802_ != NULL)
    {
     client802_->WhoIs(devInstanceFrom, devInstanceTo, adr, params);
    }
#endif

 if (clientMSTP_ != NULL)
    {
     clientMSTP_->WhoIs(devInstanceFrom, devInstanceTo, adr, params);
    }

 return BACAPI_OK;
}





BACAPI_ERROR I_cBACClient::WhoHas(uint32_t devInstanceFrom, uint32_t devInstanceTo, BACNET_OBJECT_ID *oid, BACNET_ADDRESS *adr)
{
 if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
    {
     clientIP_->WhoHas(devInstanceFrom, devInstanceTo, oid, adr);
    }

#ifdef BACAPI_BUILD_WIN32
 if (client802_ != NULL)
    {
     // TODO:
    }

 if (clientMSTP_ != NULL)
    {
     // TODO:
    }
#endif

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClient::ReadProperty(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, BACNET_PROPERTY_ID prop, int arrayIndex, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
ClearSegmentBuffer();
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->ReadProperty(oid,prop, arrayIndex, invokeId,npdu_src,npdu_dest,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->ReadProperty(oid,prop, arrayIndex, invokeId,npdu_src,npdu_dest, dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->ReadProperty(oid,prop,arrayIndex,invokeId,npdu_src,npdu_dest,dest);
            }
#endif
  default: break;
 }

 return BACAPI_OK;
}


// NOTE:
// ReadPropertyALL() je posebna implementacija ReadPropertyMultiple s specijalni PROP_ALL parametrom.
// Cita sve propertije u specificiranom objektu.
// Target uredjaj mora podrzavati ReadPropertyMultiple i ako ima puno propertija, segmentaciju
// 
// @net je source adr.
BACAPI_ERROR I_cBACClient::ReadPropertyALL(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid,  int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 ClearSegmentBuffer();
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->ReadPropertyALL(oid,invokeId,npdu_src,npdu_dest,dest);  
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->ReadPropertyALL(oid,invokeId,npdu_src,npdu_dest,dest);  
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->ReadPropertyALL(oid,invokeId,npdu_src,npdu_dest,dest);  
            }
#endif
  default: break;
 }



 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::ReadRange(BACNET_DATALINK datalink, BACNET_READ_RANGE *range, uint8_t invokeId,  BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 ClearSegmentBuffer();
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
	   if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
	      {
	   	   return clientIP_->ReadRange(range,invokeId,npdu_src,dest);
		  }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
	   if (client802_ != NULL)
	   	  {
		   return client802_->ReadRange(range,invokeId,npdu_src,dest);
		  }
  break;
  case BACNET_DATALINK_MSTP:
       if (clientMSTP_ != NULL)
          {
           return clientMSTP_->ReadRange(range,invokeId,npdu_src,npdu_dest,dest);
          }
#endif
  default: break;
 }

 return BACAPI_OK;
}


// TODO: Eth802 i MSTP
BACAPI_ERROR I_cBACClient::ReadRange_ACK(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *property, BACNET_READ_RANGE *range, uint8_t invoke,BACNET_ADDRESS *net, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->ReadRange_ACK(oid,property,range,invoke,net,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
  break;
  case BACNET_DATALINK_MSTP:
#endif
  default: break;
 }

 return BACAPI_OK;
}








BACAPI_ERROR I_cBACClient::WriteProperty(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, BACNET_OBJECT_PROPERTY *prop, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->WriteProperty(oid,prop,invokeId,npdu_src,npdu_dest,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->WriteProperty(oid,prop,invokeId,npdu_src,dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->WriteProperty(oid,prop,invokeId,npdu_src,npdu_dest,dest);
            }
#endif
  default: break;
 }



 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::ReadBDT(BACNET_ADDRESS *dest)
{
 BACAPI_ERROR err = BACAPI_OK;

 if (clientIP_)
 err = clientIP_->ReadBDT(dest);

 return err;
}




BACAPI_ERROR I_cBACClient::RegisterFD(BACNET_ADDRESS *dest,uint16_t ttl)
{
 BACAPI_ERROR err = BACAPI_OK;

 if (clientIP_)
 err = clientIP_->RegisterFD(dest,ttl);

 return err;
}


BACAPI_ERROR I_cBACClient::ReadFDT(BACNET_ADDRESS *dest)
{
 BACAPI_ERROR err = BACAPI_OK;

 if (clientIP_)
 err = clientIP_->ReadFDT(dest);

 return err;
}


BACAPI_ERROR I_cBACClient::DeleteFDTEntry(BACNET_ADDRESS *dest,uint32_t address, uint16_t port)
{
 BACAPI_ERROR err = BACAPI_OK;

 if (clientIP_)
 clientIP_->DeleteFDTEntry(dest,address,port);

 return err;
}






BACAPI_ERROR I_cBACClient::WriteBDT(BACNET_ADDRESS *dest, BACNET_BVLC_BDT *table)
{
 BACAPI_ERROR err = BACAPI_OK;
  if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
    {
     err = clientIP_->WriteBDT(dest, table);
    }

 return err;
}




BACAPI_ERROR I_cBACClient::GetAlarmSummary(BACNET_DATALINK datalink, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 ClearSegmentBuffer();
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->GetAlarmSummary(invokeId, npdu_src, dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->GetAlarmSummary(invokeId, npdu_src, dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->GetAlarmSummary(invokeId, npdu_src, npdu_dest, dest);
            }
#endif
  default: break;
 }

 return BACAPI_OK;
}





BACAPI_ERROR I_cBACClient::GetEnrollmentSummary(BACNET_DATALINK datalink, BACAPI_ENROLLMENT_SUMMARY *summary, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 ClearSegmentBuffer();
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->GetEnrollmentSummary(summary,invokeId, npdu_src, dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->GetEnrollmentSummary(summary,invokeId, npdu_src, dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->GetEnrollmentSummary(summary,invokeId, npdu_src, npdu_dest,dest);
            }
#endif
  default: break;
 }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::GetEventInformation(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 ClearSegmentBuffer();
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->GetEventInformation(invokeId, oid, npdu_src, dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->GetEventInformation(invokeId, oid, npdu_src, dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->GetEventInformation(invokeId, oid, npdu_src, npdu_dest, dest);
            }
#endif
  default: break;
 }

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClient::SubscribeCOVProperty(BACNET_DATALINK datalink, BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->SubscribeCOVProperty(cov, invokeId, npdu_src, dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->SubscribeCOVProperty(cov, invokeId, npdu_src, dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->SubscribeCOVProperty(cov, invokeId, npdu_src, npdu_dest, dest);
            }
#endif
  default: break;
 }



 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClient::SubscribeCOV(BACNET_DATALINK datalink, BACAPI_SUBSCRIBE_COV *cov, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->SubscribeCOV(cov, invokeId, npdu_src, dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->SubscribeCOV(cov, invokeId, npdu_src, dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->SubscribeCOV(cov, invokeId, npdu_src, npdu_dest, dest);
            }
#endif
  default: break;
 }



 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClient::SegmentACK(BACNET_DATALINK datalink, BACNET_SEGMENT_ACK *ack, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->SegmentACK(ack, npdu_src, dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->SegmentACK(ack, npdu_src, dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->SegmentACK(ack, npdu_src, npdu_dest, dest);
            }
#endif
  default: break;
 }


 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::CreateObject(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->CreateObject(oid,invokeId,npdu_src,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->CreateObject(oid,invokeId,npdu_src,dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->CreateObject(oid,invokeId,npdu_src,npdu_dest, dest);
            }
#endif
  default: break;
 }


 return BACAPI_OK;

}



BACAPI_ERROR I_cBACClient::DeleteObject(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invokeId,BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->DeleteObject(oid,invokeId,npdu_src,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->DeleteObject(oid,invokeId,npdu_src,dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->DeleteObject(oid,invokeId,npdu_src,npdu_dest,dest);
            }
#endif
  default: break;
 }



 return BACAPI_OK;

}



BACAPI_ERROR I_cBACClient::TimeSync(BACNET_DATALINK datalink, BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
		    clientIP_->TimeSync(dateTime,npdu_src,dest);
		   }
#ifdef BACAPI_BUILD_WIN32
		 if (client802_ != NULL)
			{
			 client802_->TimeSync(dateTime,npdu_src,dest);
			}
		 if (clientMSTP_ != NULL)
			{
			 clientMSTP_->TimeSync(dateTime,npdu_src,npdu_dest, dest);
			}
#endif


 return BACAPI_OK;
}





BACAPI_ERROR I_cBACClient::TimeSyncUTC(BACNET_DATALINK datalink, BACNET_DATE_TIME *dateTime, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->TimeSyncUTC(dateTime,npdu_src,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->TimeSyncUTC(dateTime,npdu_src,dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->TimeSyncUTC(dateTime,npdu_src,npdu_dest,dest);
            }
#endif
  default: break;
 }



 return BACAPI_OK;
}




I_cBACClient802 *I_cBACClient::GetClient802(void)
{
 return client802_;
}


I_cBACClientIP *I_cBACClient::GetClientIP(void)
{
 if (clientIP_)
     return clientIP_;
    else {
          gLog->AppendLog("I_cBACClient::GetClientIP: client is NULL");
         }

 return NULL;
}


I_cBACClientMSTP *I_cBACClient::GetClientMSTP(void)
{
 return clientMSTP_;
}



BACAPI_ERROR I_cBACClient::IAm(BACNET_DEVICE *device,BACNET_ADDRESS *dest, BACNET_ADDRESS *src)
{
 gLog->AppendLog("I_cBACClient::IAm");
 if (!dest || !device)
      return BACAPI_ERROR_INVALID_PARAMETER;


 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
    {
     clientIP_->Iam(device,dest,src);
    }

#ifdef BACAPI_BUILD_WIN32
 if (client802_ != NULL)
    {
     client802_->Iam(device,dest,src);
    }
#endif


 if (clientMSTP_ != NULL)
    {
     clientMSTP_->Iam(device,dest);
    }


 return BACAPI_OK;
}



// TODO: adrese
BACAPI_ERROR I_cBACClient::IHave(BACNET_OBJECT_ID device, BACNET_OBJECT_ID object, string name)
{
 uint32_t adr32 = clientIP_->GetBroadcastAddress();
 BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(adr32, BACNET_BACNET_IP_PORT,65535);  // kod ip adrese

 clientIP_->IHave(device,object,name,adr);

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::ReadProperty_ACK(BACNET_DATALINK datalink, BACNET_OBJECT *object, BACNET_OBJECT_PROPERTY *property,int arrayIndex,  int invokeId, BACNET_ADDRESS *dest)
{
 BACAPI_ERROR err;




 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
             clientIP_->ReadProperty_ACK(object, property,arrayIndex, dest, invokeId);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->ReadProperty_ACK(object, property,arrayIndex, dest, invokeId);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             //clientMSTP_->Simple_ACK(service,invokeId,dest);
            }
#endif
 }


 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::ReadPropertyMultiple_ALL_ACK(BACNET_DATALINK datalink, BACNET_OBJECT *object, int invokeId, BACNET_ADDRESS *dest)
{






 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
             clientIP_->ReadPropertyMultiple_ALL_ACK(object, dest, invokeId);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->ReadPropertyMultiple_ALL_ACK(object, dest, invokeId);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             //clientMSTP_->Simple_ACK(service,invokeId,dest);
            }
#endif
 }


 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::Simple_ACK(BACNET_DATALINK datalink, BACNET_CONFIRMED_SERVICE service, int invokeId, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->Simple_ACK(service,invokeId,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 client802_->Simple_ACK(service,invokeId,dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->Simple_ACK(service,invokeId,npdu_src,npdu_dest,dest);
            }
#endif
  default: break;
 }



 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::AcknowledgeAlarm(BACNET_DATALINK datalink, BACNET_ACK_ALARM ack_alarm, int invokeId, BACNET_ADDRESS *dest)
{
 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			clientIP_->AcknowledgeAlarm(ack_alarm,invokeId,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 //client802_->Simple_ACK(service,invokeId,dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             //clientMSTP_->Simple_ACK(service,invokeId,dest);
            }
#endif
  default: break;
 }



 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::AtomicReadFileRec(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id,uint16_t rec_start_pos, uint32_t rec_count, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 ClearSegmentBuffer();
 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }


 switch(datalink)
 {
  case BACNET_DATALINK_BACIP:
		if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
		   {
			return clientIP_->AtomicReadFileRec(oid,invoke_id, rec_start_pos,rec_count,dest);
		   }
  break;

#ifdef BACAPI_BUILD_WIN32
  case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
			 return client802_->AtomicReadFileRec(oid, invoke_id,rec_start_pos,rec_count,dest);
			}
  break;
  case BACNET_DATALINK_MSTP:
         if (clientMSTP_ != NULL)
            {
             clientMSTP_->AtomicReadFileRec(oid, invoke_id, rec_start_pos,rec_count,npdu_src,npdu_dest,dest);
            }
#endif
  default: break;
 }

 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}

BACAPI_ERROR I_cBACClient::AtomicReadFileOct(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id, uint16_t file_start_pos, uint32_t octet_count, BACNET_ADDRESS *dest)
{
 ClearSegmentBuffer();
 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 switch (datalink)
        {
         case BACNET_DATALINK_BACIP:
		 if (clientIP_ != NULL)
			{
			 return clientIP_->AtomicReadFileOct(oid, invoke_id,file_start_pos,octet_count, dest);
			}
	     case BACNET_DATALINK_ETH802:
		 if (client802_ != NULL)
			{
#ifdef BACAPI_BUILD_WIN32
			 return client802_->AtomicReadFileOct(oid,invoke_id,file_start_pos,octet_count, dest);
#endif
         break;
			}
		 case BACNET_DATALINK_MSTP:
//		 if (clientMSTP_ != NULL)
//			{
//			 return clientMSTP_->AtomicReadFileOct(oid, invoke_id, file_start_pos,octet_count,dest);
//			}
         break;
        }

 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}



BACAPI_ERROR I_cBACClient::AtomicWriteFileRec(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id,BACNET_FILE *file_data, BACNET_ADDRESS *dest)
{
 if (!clientIP_ && !client802_ && !clientMSTP_)
	{
	 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
	}

 switch (datalink)
        {
         case BACNET_DATALINK_BACIP:
		 if (clientIP_ != NULL)
			{
			 return clientIP_->AtomicWriteFileRec(oid, invoke_id, file_data,dest);
			}
		 case BACNET_DATALINK_ETH802:
#ifdef BACAPI_BUILD_WIN32
		 if (client802_ != NULL)
			{
			 return client802_->AtomicWriteFileRec(oid, invoke_id, file_data,dest);
			}
#endif
         break;
		 case BACNET_DATALINK_MSTP:
//		 if (clientMSTP_ != NULL)
//			{
//			 return clientMSTP_->AtomicWriteFileRec(oid, invoke_id, file_data,dest);
//			}
         break;
	    }
 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}

BACAPI_ERROR I_cBACClient::AtomicWriteFileOct(BACNET_DATALINK datalink, BACNET_OBJECT_ID *oid, uint8_t invoke_id,uint16_t file_start_pos, uint32_t octet_count, BACNET_FILE *file_data, BACNET_ADDRESS *dest)
{
 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 switch (datalink)
        {
         case BACNET_DATALINK_BACIP:
		 if (clientIP_ != NULL)
			{
			 return clientIP_->AtomicWriteFileOct(oid, invoke_id, file_start_pos, octet_count, file_data,dest);
			}
	     case BACNET_DATALINK_ETH802:
#ifdef BACAPI_BUILD_WIN32
		 if (client802_ != NULL)
			{
			 return client802_->AtomicWriteFileOct(oid, invoke_id, file_start_pos, octet_count, file_data,dest);
			}
#endif
         break;
	     case BACNET_DATALINK_MSTP:
//		 if (clientMSTP_ != NULL)
//			{
//			 return clientMSTP_->AtomicWriteFileOct(oid, invoke_id, file_start_pos, octet_count, file_data,dest);
//			}
         break;
        }
 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}


BACAPI_ERROR I_cBACClient::AtomicWriteAck(BACNET_DATALINK datalink, uint8_t invoke_id, uint8_t access, uint16_t file_start_pos, BACNET_ADDRESS *dest)
{
 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 switch (datalink)
        {
         case BACNET_DATALINK_BACIP:
		 if (clientIP_ != NULL)
			{
			 return clientIP_->AtomicWriteAck(invoke_id,access,file_start_pos,dest);
			}
	     case BACNET_DATALINK_ETH802:
#ifdef BACAPI_BUILD_WIN32
		 if (client802_ != NULL)
			{
			 //return client802_->AtomicWriteFileOct(oid, invoke_id, file_start_pos, octet_count, file_data,dest);
			}

#endif
          break;
	     case BACNET_DATALINK_MSTP:
//		 if (clientMSTP_ != NULL)
//			{
//			 //return clientMSTP_->AtomicWriteFileOct(oid, invoke_id, file_start_pos, octet_count, file_data,dest);
//			}
         break;
        }
 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}


BACAPI_ERROR I_cBACClient::AtomicReadFile_ACK(BACNET_DATALINK datalink, BACNET_FILE *bac_file, uint8_t invoke, BACNET_ADDRESS *dest)
{
 BACAPI_ERROR apierr = BACAPI_OK;
 if (!clientIP_ && !client802_ && !clientMSTP_)
    {
     return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
    }

 if (clientIP_ != NULL)
    {
     return clientIP_->AtomicReadFile_ACK(bac_file,invoke,dest);
	}
 if (client802_ != NULL)
	{
	 //client802_->AtomicWriteFileOct(oid, file_data,dest);
	 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
	}

#ifdef BACAPI_BUILD_WIN32
 if (clientMSTP_ != NULL)
	{
	 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
	 //clientMSTP_->Simple_ACK(adr,service,invokeId);
	}
#endif
 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}




BACAPI_ERROR I_cBACClient::UnconfirmedEventNotification(BACNET_EVENT_NOTIFICATION *eventNotification)
{
 //uint32_t adr32 = BACAPI_GetAddressUint32((char*)TO_IP);
 //BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(adr32, BACNET_BACNET_IP_PORT,65535);  // kod ip adrese

 //clientIP_->UnconfirmedEventNotification(adr, eventNotification);

 return BACAPI_OK;
}




BACAPI_ERROR I_cBACClient::Error_ACK(BACNET_DATALINK datalink, BACNET_ERROR bacerr,int invokeId,BACNET_CONFIRMED_SERVICE service, BACNET_ADDRESS *npdu_src, BACNET_ADDRESS *npdu_dest, BACNET_ADDRESS *dest)
{
 switch (datalink)
    {
     case BACNET_DATALINK_BACIP:
       clientIP_->Error_ACK(bacerr,invokeId,service, dest);
       break;

     case BACNET_DATALINK_ETH802:
#ifdef BACAPI_BUILD_WIN32
       client802_->Error_ACK(bacerr,invokeId,service, dest);
#endif
       break;
     case BACNET_DATALINK_MSTP:
       clientMSTP_->Error_ACK(bacerr,invokeId,service, npdu_src,npdu_dest,dest);
       break;

    }

 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::Reject_ACK(BACNET_DATALINK datalink, BACNET_REJECT_CODE code, int invokeId, BACNET_ADDRESS *dest)
{
 clientIP_->Reject_ACK(code,invokeId, dest);
 return BACAPI_OK;
}



BACAPI_ERROR I_cBACClient::Abort_ACK(BACNET_DATALINK datalink, BACNET_ABORT abortcode, BACNET_ADDRESS *dest)
{

 clientIP_->Abort_ACK(abortcode, dest);

 return BACAPI_OK;
}




void I_cBACClient::Execute(void)
{

// if (clientMSTP_)
//    {
//     clientMSTP_->Execute();
//    }
}



BACAPI_ERROR I_cBACClient::DeviceCommunicationControl(BACNET_DATALINK datalink, uint16_t duration, uint32_t enable, string password, BACNET_ADDRESS *adr)
{

 switch (datalink)
        {
         case BACNET_DATALINK_BACIP:
		 if (clientIP_ != NULL)
			{
			 return clientIP_->DeviceCommunicationControl(adr,duration,enable,password);
			}
	     case BACNET_DATALINK_ETH802:
#ifdef BACAPI_BUILD_WIN32
		 if (client802_ != NULL)
			{
			 return client802_->DeviceCommunicationControl(adr,duration,enable,password);
			}
#endif
         break;
	     case BACNET_DATALINK_MSTP:
//		 if (clientMSTP_ != NULL)
//			{
//			 return clientMSTP_->DeviceCommunicationControl(duration,enable,password,adr);
//			}
         break;
        }
 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}



BACAPI_ERROR I_cBACClient::ReinitDevice(BACNET_DATALINK datalink, uint32_t start, string password, BACNET_ADDRESS *adr)
{

 switch (datalink)
        {
         case BACNET_DATALINK_BACIP:
		 if (clientIP_ != NULL)
			{
			 return clientIP_->ReinitDevice(adr,start,password);
			}
	     case BACNET_DATALINK_ETH802:
#ifdef BACAPI_BUILD_WIN32
		 if (client802_ != NULL)
			{
			 return client802_->ReinitDevice(adr,start,password);
			}
#endif
         break;
	     case BACNET_DATALINK_MSTP:
	     break;
//		 if (clientMSTP_ != NULL)
//			{
//			 return clientMSTP_->ReinitDevice(adr,start,password);
//			}
        }
 return BACAPI_ERROR_INTERFACE_NO_INTERFACE;
}





BACAPI_ERROR I_cBACClient::WhoIs_RouterToNetwork(BACNET_ADDRESS *dest)
{
 BACAPI_ERROR err = BACAPI_OK;
 if (clientIP_ != NULL)   // definirano je koristenje BAC/IP interfacea
    {
     if (dest == NULL)
        {
		 BACNET_ADDRESS *dst = NULL;
		 uint32_t ibcast = GetClientIP()->GetAce()->GetBroadcastAddress();
		 dst = BACAPI_CreateBACnetAddress(ibcast,47808,65535);
		 err = clientIP_->WhoIs_RouterToNetwork(dst);
		 delete dst;
		}else clientIP_->WhoIs_RouterToNetwork(dest);
    }else return BACAPI_ERROR_INTERFACE_IP;

 return err;
}



BACAPI_ERROR I_cBACClient::I_Am_RouterToNetwork(BACNET_DATALINK dlink, vector<uint16_t>dnetList, BACNET_ADDRESS *dest)
{
 switch (dlink)
        {
         case BACNET_DATALINK_BACIP:
          clientIP_->I_Am_RouterToNetwork(dnetList,dest);
          break;
         case BACNET_DATALINK_ETH802:
#ifdef BACAPI_BUILD_WIN32
          client802_->I_Am_RouterToNetwork(dnetList,dest);
#endif
          break;
          
          case BACNET_DATALINK_MSTP:
           clientMSTP_->I_Am_RouterToNetwork(dnetList,dest);
           break;
         default:
          
          break;
        }

 return BACAPI_OK;
}


BACAPI_ERROR I_cBACClient::Init_RoutingTable(BACNET_ADDRESS *dest)
{
 BACAPI_ERROR err = BACAPI_OK;

 if (!dest) return BACAPI_ERROR_INVALID_PARAMETER;

 if (clientIP_ != NULL)
    {

     err = clientIP_->Init_RoutingTable(dest);
    }else return BACAPI_ERROR_INTERFACE_IP;

 return err;
}







*/



