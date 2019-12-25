

//#if !defined BACAPI_BACNET_LOOKOUT // || defined BACAPI_BUILD_WIN32
//#include <winsock2.h>  // GetAddressString / htons
//#endif


#include "bacapi.h"
#include "I_cBACdec.h"
#include "namedef.h"
#include "I_cBACLog.h"
#include "macnamedef.h"
#include <stdlib.h>
#include <string.h>

#if defined BACAPI_BUILD_LINUX || defined BACAPI_BUILD_ARM9
#include <pthread.h>
#include <arpa/inet.h>
#endif



BACNET_ADDRESS *BACAPI_MyAddress;
BACNET_ADDRESS *BACAPI_BroadcastAddress;
BACNET_ADDRESS *BACAPI_GlobalBroadcastAddress;

bool BACAPI_segmentedFlag = false;

BACAPI_ERROR lastError = BACAPI_OK;
BACNET_ERROR lastBACError;

#ifdef BACAPI_BUILD_WIN32
HWND mainHWND_;
HANDLE receiveFrameFlag;   // receive frame state machine
HANDLE receiveFIFOBufferFlag;
#else
//pthread_cond_t receiveFrameFlag;
//pthread_mutex_t receiveFrameMutex;
//neosmart_event_t receiveFrameFlag;
//neosmart_event_t receivePacketFlag;
#endif




vector<BACAPI_INVOKE_ID*>vInvokeList;



void BACAPI_PrintOctetData(string filename, uint8_t *data, size_t data_len)
{
 FILE *f = fopen(filename.c_str(),"wb");

 for (int i=0,c=0;i<data_len;i++,c++)
     {
      //fprintf(f,"%02X ",data[i]);
      fprintf(f,"%c",data[i]);
      if (c== 64)
         {
          fprintf(f,"\n");
          c = 0;
         }
     }
 fclose(f);
}


string BACAPI_GetOctetData(uint8_t *data, size_t data_len)
{
 string s;
 char octdata[256];
 for (int i=0,c=0;i<data_len;i++,c++)
     {
      char byte[8];
      sprintf(byte, "%02X ", data[i]);
      s += string(byte);
     }
     

 return s;
}





void BACAPI_Init(void)
{
 for (int c=0;c<256;c++)
     {
      BACAPI_INVOKE_ID *invoke = new BACAPI_INVOKE_ID;
      invoke->active_ = false;
      invoke->dev = NULL;
      invoke->obj = NULL;
      invoke->prop = NULL;
      invoke->invokeId = c;
      vInvokeList.push_back(invoke);
     }

 BACAPI_InitEnumTypes();
}

void BACAPI_Fini(void)
{
 for (int c=0;c<256;c++)
     {
      BACAPI_INVOKE_ID *invoke = vInvokeList[c];
      delete invoke;
     }
 vInvokeList.clear();
}




// TODO: ENUM za monitor,pid
BACAPI_INVOKE_ID *BACAPI_ReserveInvokeId(BACNET_DEVICE *dev,BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop, BACAPI_INVOKE_TYPE type)
{
 for (int c=0;c<256;c++)
     {
      BACAPI_INVOKE_ID *invoke = vInvokeList[c];
      if (invoke->active_ == false)
         {
		  invoke->dev = dev;
		  invoke->obj = obj;
		  invoke->prop = prop;
		  invoke->active_ = true;
		  invoke->invoke_type = type;
          return invoke;
         }
     }


 return NULL;
}




BACAPI_INVOKE_ID *BACAPI_GetInvoke(uint8_t invokeid)
{
 for (int c=0;c<256;c++)
     {
      BACAPI_INVOKE_ID *invoke = vInvokeList[c];
      if (invoke->invokeId == invokeid && invoke->active_ == true)
         {
          return invoke;
         }
      }
 return NULL;
}


void BACAPI_ReleaseInvokeId(BACAPI_INVOKE_ID *invoke)
{
 invoke->active_ = false;
 invoke->dev = NULL;
 invoke->obj = NULL;
 invoke->prop = NULL;
 invoke->invoke_type = INVOKE_TYPE_NORMAL;
}




void BACAPI_ReleaseInvokeIdAll(void)
{
 for (int c=0;c<256;c++)
     {
      BACAPI_INVOKE_ID *invoke = vInvokeList[c];
      invoke->active_ = false;
      invoke->dev = NULL;
      invoke->obj = NULL;
      invoke->prop = NULL;
      invoke->invoke_type = INVOKE_TYPE_NORMAL;
     }
}





int BACAPI_ToInt(string s)
{


 int r = atoi(s.c_str());
 return r;
}


float BACAPI_ToFloat(string s)
{
    float f = (float)atof(s.c_str());
    return (float)f;
}



double BACAPI_ToDouble(string s)
{
    double f = (double)atof(s.c_str());
    return (double)f;
}



uint32_t BACAPI_ToUint32(string s)
{
 uint32_t r = atol(s.c_str());
 return r;
}


extern string BACAPI_ToString(int n)
{
 char *ch = new char[128];
 sprintf(ch,"%d",n);
 string s(ch);
 delete []ch;
 return s;
}

extern string BACAPI_ToString8(uint8_t n)
{
 char *ch = new char[128];
 sprintf(ch,"%d",n);
 string s(ch);
 delete []ch;
 return s;
}

extern string BACAPI_ToString16(uint16_t n)
{
 char *ch = new char[128];
 sprintf(ch,"%d",n);
 string s(ch);
 delete []ch;
 return s;
}

extern string BACAPI_ToString32(uint32_t n)
{
 char *ch = new char[128];
 sprintf(ch,"%d",n);
 string s(ch);
 delete []ch;
 return s;
}



string BACAPI_ToStringr(double n)
{
 char *ch = new char[128];
 sprintf(ch,"%f",n);
 string s(ch);
 delete []ch;
 return s;
}


string BACAPI_ToStringf(float n)
{
 char *ch = new char[128];
 sprintf(ch,"%f",n);
 string s(ch);
 delete []ch;
 return s;
}


uint16_t BACAPI_GetBACnetObjectCount(void)
{
 return BACNET_OBJECT_COUNT;
}


uint16_t BACAPI_GetBACnetPropertyCount(void)
{
 return BACNET_PROPERTY_COUNT;
}



int BACAPI_GetBACnetObjectType(string name)
{
 for (int c=0;c<BACNET_OBJECT_COUNT;c++)
     {
      if (BACAPI_BACnetObjectName(c) == name)
         {
          return c;
         }
     }
 return -1;
}




bool BACAPI_IsCorrectTime(BACNET_TIME *tm)
{
 if (tm->hour < 0 || tm->hour >24 || tm->minute < 0 || tm->minute >59 || tm->sec < 0 || tm->sec > 59)
    return false;

 return true;
}

bool BACAPI_IsCorrectDate(BACNET_DATE *date)
{
 if (date->day <= 0 || date->day >31 ||
     date->month <=0 || date->month > 12)
     return false;

 return true;
}


void BACAPI_CorrectBACnetTime(BACNET_TIME *tm)
{
 if (tm->hour <= 0 || tm->hour > 24) tm->hour = 1;
 if (tm->minute <= 0 || tm->minute > 60) tm->minute = 1;
 if (tm->sec <= 0 || tm->sec > 60) tm->sec = 1;
}



void BACAPI_CorrectBACnetDate(BACNET_DATE *dt)
{
 if (dt->day <= 0 || dt->day >31 ) dt->day = 1;
 if (dt->month <= 0 || dt->month >12) dt->month = 1;
 if (dt->year < 1970) dt->year = 1970;


}







BACNET_OBJECT_ID BACAPI_BACnetObjectId(uint16_t type, uint32_t instance)
{
 BACNET_OBJECT_ID oid;
 oid.type = type;
 oid.instance = instance;
 return oid;
}
// izbrisati
void BACAPI_SetBACnetError(BACNET_ERROR_CLASS errClass, BACNET_ERROR_CODE errCode)
{

}

//izbrisati

BACNET_ERROR *BACAPI_GetBACnetError(void)
{
 return &lastBACError;
}



// konstruira BACNET_ADDRESS i vraca pointer. user je odgovoran za dealokaciju
BACNET_ADDRESS *BACAPI_CreateBACnetAddress(uint32_t ipadr, uint16_t port, uint16_t netnum)
{
 BACNET_ADDRESS *adr = new BACNET_ADDRESS;

 int len = gDec->encode_unsigned32(adr->adr,ipadr);
 gDec->encode_unsigned16(&adr->adr[4],port);

 adr->networkNumber = netnum;
 adr->len = 6;
 return adr;
}


BACNET_ADDRESS *BACAPI_CreateBACnetAddress(const char *s, uint16_t port, uint16_t nn)
{
 uint32_t inta = BACAPI_GetAddressUint32(s);
 BACNET_ADDRESS *adr = BACAPI_CreateBACnetAddress(inta,port,nn);
 return adr;
}



BACNET_ADDRESS *BACAPI_CreateBACnetAddress(BACNET_ADDRESS *adr)
{
 if (!adr) return NULL;

 BACNET_ADDRESS *newadr = new BACNET_ADDRESS;
 memset(newadr,0,sizeof(BACNET_ADDRESS));

 memcpy(newadr,adr,sizeof(BACNET_ADDRESS));
 return newadr;
}



BACNET_ADDRESS *BACAPI_CreateBACnetAddress(unsigned char *adr, uint16_t port, uint16_t netnum)  // kod ethernet adrese
{
 BACNET_ADDRESS *dest = new BACNET_ADDRESS;
 //todo memcpy ovdje
 dest->adr[0] = adr[0];
 dest->adr[1] = adr[1];
 dest->adr[2] = adr[2];
 dest->adr[3] = adr[3];
 dest->adr[4] = (uint8_t)((port & 0xFF00) >> 8);
 dest->adr[5] = (uint8_t)(port & 0x00FF);

 dest->len = 6;
 dest->networkNumber = netnum;

 return dest;
}


BACNET_ADDRESS *BACAPI_CreateBACnetAddress(unsigned char *adr, uint16_t netnum)  // kod ethernet adrese
{
 BACNET_ADDRESS *dest = new BACNET_ADDRESS;
 //todo memcpy ovdje
 dest->adr[0] = adr[0];
 dest->adr[1] = adr[1];
 dest->adr[2] = adr[2];
 dest->adr[3] = adr[3];
 dest->adr[4] = adr[4];
 dest->adr[5] = adr[5];

 dest->len = 6;
 dest->networkNumber = netnum;

 return dest;
}




BACNET_ADDRESS *BACAPI_CreateBACnetAddress(void)
{
 BACNET_ADDRESS *newadr = new BACNET_ADDRESS;
 memset(newadr,0,sizeof(BACNET_ADDRESS));
 return newadr;
}

BACNET_ADDRESS *BACAPI_CreateBACnetAddress(uint8_t src, uint16_t netnum)
{
 BACNET_ADDRESS *newadr = new BACNET_ADDRESS;
 memset(newadr,0,sizeof(BACNET_ADDRESS));

 newadr->adr[0] = src;
 newadr->len = 1;
 newadr->networkNumber = netnum;
 return newadr;
}

void BACAPI_CopyBACnetAddress(BACNET_ADDRESS *to, BACNET_ADDRESS *from)
{
 if (from && to)
    {
     memcpy(to,from,sizeof(BACNET_ADDRESS));
    }
}




void BACAPI_SetMyAddress(unsigned char *adr, uint16_t port, uint16_t netnum)
{
}


void BACAPI_SetBroadcastAddress(unsigned char *adr)
{
}


void BACAPI_CopyBitString(BACNET_BIT_STRING *to, BACNET_BIT_STRING *from)
{
    uint8_t bits_used;
    uint8_t value[MAX_BITSTRING_BYTES];

 to->bits_used = from->bits_used;
 for (int c=0;c<MAX_BITSTRING_BYTES;c++)
     {
      to->value[c] = from->value[c];
     }
}


// maknuti
uint32_t BACAPI_GetBroadcastAddress(uint8_t port)
{
    /*
 IP_ADAPTER_INFO *FixedInfo;
 ULONG ulOutBufLen;



 //FIXME: mozda je adresni string veci od 256?
 char str_adr[256];
 gethostname(str_adr, sizeof(str_adr));

 ACE_INET_Addr adr(port,str_adr);
 uint32_t myIpAddress = adr.get_ip_address();


 FixedInfo = (IP_ADAPTER_INFO *) GlobalAlloc( GPTR, sizeof( IP_ADAPTER_INFO ) );
 ulOutBufLen = sizeof( IP_ADAPTER_INFO );



 if ( ERROR_SUCCESS != GetAdaptersInfo( FixedInfo, &ulOutBufLen ) )
    {
     //MessageBox("Cound not get the adapter information!");
     return 0;
    }



 uint32_t myIpSubnetAddress = inet_addr( FixedInfo->IpAddressList.IpMask.String );
 myIpSubnetAddress = htonl(myIpSubnetAddress);


 unsigned int net = myIpAddress & myIpSubnetAddress;
 unsigned int unique = ~myIpSubnetAddress;

 uint32_t t = net+unique;


  GlobalFree(FixedInfo);

 return net+unique;
 */
 return 0;

}


string BACAPI_GetAddressString(BACNET_ADDRESS *adr, int adr_len)
{
 string s;
 for (int c=0;c<adr_len;c++)
     {
      char tmp[4];
      sprintf(tmp,"%X ",adr->adr[c]);
      s+= tmp;
     }

 return string(s);
}


string BACAPI_GetAddressString(BACNET_ADDRESS *adr)
{
 char tmp[32];
 sprintf(tmp,"%X %X %X %X: %X%X",adr->adr[0]
                                   ,adr->adr[1]
                                   ,adr->adr[2]
                                   ,adr->adr[3]
                                   ,adr->adr[4]
                                   ,adr->adr[5]);

 return string(tmp);
}


string BACAPI_GetAddressStringIP(BACNET_ADDRESS *adr)
{
 char tmp[32];
 sprintf(tmp,"%d.%d.%d.%d: %X%X",adr->adr[0]
                                   ,adr->adr[1]
                                   ,adr->adr[2]
                                   ,adr->adr[3]
                                   ,adr->adr[4]
                                   ,adr->adr[5]);

 return string(tmp);
}

string BACAPI_GetAddressStringIP(uint8_t *adr)
{
 char tmp[32];
 sprintf(tmp,"%d.%d.%d.%d",adr[0] ,adr[1],adr[2],adr[3]);
 return string(tmp);
}


char *BACAPI_GetAddressStr(uint32_t adr)
{
/*
	struct in_addr addr;
	addr.s_addr = htonl(adr);
	return inet_ntoa(addr);
	*/
return NULL;
}




uint32_t BACAPI_GetAddressUint32(const char *adr)
{
/*
 uint32_t tmp = inet_addr(adr);
 return htonl(tmp);
 */
 return 0;
}

uint32_t BACAPI_GetAddressUint32(uint8_t *adr)
{
 string sadr = BACAPI_GetAddressStringIP(adr);
 uint32_t iadr = BACAPI_GetAddressUint32(sadr.c_str());
 return iadr;
}

// Main event loop
int BACAPI_PollEvent(BACAPI_Event *event)
{
 if (qEventList.size() > 0)
    {
     BACAPI_Event *popEvent = qEventList.front();
     event->type = popEvent->type;

     switch (event->type)
            {
             case BACAPI_EVENT_I_AM:
             gLog->AppendLog("BACAPI_PollEvent: BACAPI_EVENT_I_AM");
             event->device = popEvent->device;
             break;


             case BACAPI_EVENT_ERROR:
             event->address = popEvent->address;
             //event->error = popEvent->error;
             event->error.errClass = popEvent->error.errClass;
             event->error.errCode = popEvent->error.errCode;
             event->invokeId = popEvent->invokeId;
             break;



             // TODO: inkonponirati u BACAPI_ERROR
             case BACAPI_EVENT_REJECT:
             event->rejCode = popEvent->rejCode;
             event->invokeId = popEvent->invokeId;
			 break;

             // TODO: inkonponirati u BACAPI_ERROR
             case BACAPI_EVENT_ABORT:
             event->invokeId = popEvent->invokeId;
             event->abort = popEvent->abort;
			 break;
             


             case BACAPI_EVENT_MESSAGE:
             event->message = popEvent->message;
             event->invokeId = popEvent->invokeId;
             break;

             case BACAPI_EVENT_RP_ACK:
             event->property = popEvent->property;
             event->invokeId = popEvent->invokeId;
             event->oid.instance = popEvent->oid.instance;
             event->oid.type = popEvent->oid.type;
             event->address = popEvent->address;
             gLog->AppendLog("BACAPI: PROPERTY [%s]",BACAPI_BACnetPropertyName(event->property->property));
             break;

             case BACAPI_EVENT_RP_ACK_ARRAY_ELEMENT:
             event->property = popEvent->property;
             event->invokeId = popEvent->invokeId;
             event->oid.instance = popEvent->oid.instance;
             event->oid.type = popEvent->oid.type;
             break;

             case BACAPI_EVENT_RPM_ACK:
             event->object = popEvent->object;
             event->invokeId = popEvent->invokeId;
             break;

             case BACAPI_EVENT_SIMPLE_ACK:
             event->simpleAck = popEvent->simpleAck;
             event->invokeId = popEvent->invokeId;
             break;

             case BACAPI_EVENT_ALARM_SUMMARY:
             event->alarm = popEvent->alarm;
             event->invokeId = popEvent->invokeId;
			 break;

			 case BACAPI_EVENT_ENROLLMENT_SUMMARY:
			 event->summary = popEvent->summary;
			 event->invokeId = popEvent->invokeId;
			 break;


			 case BACAPI_EVENT_EVENT_INFORMATION:
			 event->information = popEvent->information;
			 event->invokeId = popEvent->invokeId;
			 break;

             case BACAPI_EVENT_UNCONFIRMED_COV_NOTIFICATION:
             case BACAPI_EVENT_CONFIRMED_COV_NOTIFICATION:
			 event->cov = popEvent->cov;
			 event->invokeId = popEvent->invokeId;
			 break;


			 case BACAPI_EVENT_SEGMENT_ACK:
			 event->segmentAck = popEvent->segmentAck;
			 break;


			 case BACAPI_EVENT_ERROR_UNKNOWN_PDU_TYPE: // TODO: spada u BACAPI_ERROR
			 break;

			 case BACAPI_EVENT_NO_ACTIVE_ALARMS:
			 event->invokeId = popEvent->invokeId;
			 break;

			 // REQUESTS

             case BACAPI_EVENT_WHO_IS:
             event->deviceRange = popEvent->deviceRange;
             event->address = popEvent->address;
             break;

             case BACAPI_EVENT_RP_REQ:
             event->objPropRef = popEvent->objPropRef;
             event->invokeId = popEvent->invokeId;
             event->address = popEvent->address;
             event->srcDataLink = popEvent->srcDataLink;
             break;

             case BACAPI_EVENT_WP_REQ:
             event->object = popEvent->object;
             event->property = popEvent->property;
             event->invokeId = popEvent->invokeId;
             event->address = popEvent->address;
             event->srcDataLink = popEvent->srcDataLink;
             break;

             case BACAPI_EVENT_RPM_REQ:
             event->objPropRef = popEvent->objPropRef;
             event->address = popEvent->address;
             event->invokeId = popEvent->invokeId;
             event->srcDataLink = popEvent->srcDataLink;
             break;

             case BACAPI_EVENT_NOTIFICATION_EVENT:
             event->notification = popEvent->notification;
             event->invokeId = popEvent->invokeId;
             break;

             case BACAPI_EVENT_CREATE_OBJECT:
             event->oid.type = popEvent->oid.type;
             event->oid.instance = popEvent->oid.instance;
             event->invokeId = popEvent->invokeId;
             event->address = popEvent->address;

             break;

             case BACAPI_EVENT_DELETE_OBJECT:
             event->oid.type = popEvent->oid.type;
             event->oid.instance = popEvent->oid.instance;
             event->invokeId = popEvent->invokeId;
             event->address = popEvent->address;
             break;

             case BACAPI_EVENT_WHO_HAS_REQ:
             event->deviceRange = popEvent->deviceRange;
             event->oid.instance = popEvent->oid.instance;
             event->oid.type = popEvent->oid.type;
             event->extraText = popEvent->extraText;
             break;

             case BACAPI_EVENT_I_HAVE_ACK:
             event->type = popEvent->type;
             event->device = popEvent->device;
             event->object = popEvent->object;
             break;

             case BACAPI_EVENT_BVLC_READ_BDT_NAK:
             event->type = popEvent->type;
             break;

             case BACAPI_EVENT_BVLC_READ_BDT_ACK:
             event->type = popEvent->type;
             event->bdtTable = popEvent->bdtTable;
             break;

             case BACAPI_EVENT_BVLC_READ_FDT_ACK:
             event->type = popEvent->type;
             event->fdtTable = popEvent->fdtTable;
             break;

             case BACAPI_ERROR_BVLC_READ_FDT_NAK:
              event->type = popEvent->type;
              break;


             case BACAPI_EVENT_BVLC_RESULT_SUCCESS:
             event->type = popEvent->type;
             break;

             case BACAPI_EVENT_BVLC_REGISTER_FD_NAK:
             event->type = popEvent->type;
             break;

             case BACAPI_EVENT_BVLC_DELETE_FD_NAK:
             event->type = popEvent->type;
             break;

             case BACAPI_EVENT_ATOMIC_READ_FILE_ACK:
             {
             event->pdu_type = popEvent->pdu_type;
             event->file_data = popEvent->file_data;
             event->invokeId = popEvent->invokeId;
             break;
             }

             case BACAPI_EVENT_ATOMIC_WRITE_FILE_ACK:
             {
             event->pdu_type = popEvent->pdu_type;
             event->file_data = popEvent->file_data;
             event->invokeId = popEvent->invokeId;
             break;
             }

             case BACAPI_EVENT_ATOMIC_READ_FILE_REQ:
             event->file_data = popEvent->file_data;
             event->invokeId = popEvent->invokeId;
             event->address = popEvent->address;
             break;

             case BACAPI_EVENT_ATOMIC_WRITE_FILE_REQ:
             event->file_data = popEvent->file_data;
             event->invokeId = popEvent->invokeId;
             event->address = popEvent->address;
             break;

             case BACAPI_EVENT_READ_RANGE_REQ:
             event->invokeId = popEvent->invokeId;
             event->address = popEvent->address;
             event->range_data = popEvent->range_data;
             break;

             case BACAPI_EVENT_READ_RANGE_ACK:
             event->invokeId = popEvent->invokeId;
             event->object = popEvent->object;
             break;

             case BACAPI_EVENT_MSTP:
             event->data_model = popEvent->data_model;
             break;

             case BACAPI_EVENT_I_AM_ROUTER_TO_NETWORK:
             event->router = popEvent->router;
             break;

             case BACAPI_EVENT_WHO_IS_ROUTER_TO_NETWORK:
             event->address = popEvent->address;
             event->srcDataLink = popEvent->srcDataLink;
             break;

             case BACAPI_EVENT_ROUTE_PACKET:
             //printf("BACAPI: PollEvent: Dobio route event. Postavljam paket.\n");
             event->packet = popEvent->packet;
             break;

             case BACAPI_EVENT_LOG_MESSAGE:
             event->logmsg = popEvent->logmsg;
             break;
			}


#ifdef BACAPI_BUILD_LINUX
     critical_section.acquire();
#endif
#ifdef BACAPI_BUILD_WIN32
     delete popEvent;    // LINUX baca SEGFAULT ako je ukljucen delete. TODO: provjeriti zasto
#endif
     qEventList.pop();

#ifdef BACAPI_BUILD_LINUX
     critical_section.release();
#endif
     return 1;
    }

 event = NULL;
 return 0;
}



size_t BACAPI_PushEvent(BACAPI_Event *event)
{

#ifdef BACAPI_BUILD_LINUX
    critical_section.acquire();
#endif

// EnterCriticalSection(&CriticalSection);
 qEventList.push(event);

#ifdef BACAPI_BUILD_LINUX
 critical_section.release();
#endif

 //LeaveCriticalSection(&CriticalSection);
// gLog->AppendLog("BACAPI_PushEvent: type %d, queue size = %d\n",event->type,qEventList.size());


 return qEventList.size();
}





// Legacy: ne koristiti
void BACAPI_SetLastError(BACAPI_ERROR err)
{
 lastError = err;
}


// uvijek zvati ako je lastError != BACAPI_OK
// Legacy: ne koristiti
void BACAPI_ResetLastError()
{
 lastError = BACAPI_OK;
}

// Legacy: ne koristiti
BACAPI_ERROR BACAPI_GetLastError(void)
{
 return lastError;
}





int BACAPI_SetObjectPropertyValueString(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, string value)
{
 BACNET_OBJECT_PROPERTY *prop = BACAPI_GetObjectProperty(prop_id,object);
 if (!prop) return -1;

 BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
 if (val->application_tag != BACNET_APPLICATION_TAG_CSTRING) return -1;

 val->type.character_String.encoding = CHAR_SET_ANSI_X34;
 val->type.character_String.value8 = value;
 return 0;
}






int BACAPI_SetObjectPropertyValueReal(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, float value)
{
 BACNET_OBJECT_PROPERTY *prop = BACAPI_GetObjectProperty(prop_id,object);
 if (!prop) return -1;

 BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
 if (val->application_tag != BACNET_APPLICATION_TAG_REAL) return -1;

 val->type.Real = value;
 return 0;
}


int BACAPI_SetObjectPropertyValueDouble(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, double value)
{
 BACNET_OBJECT_PROPERTY *prop = BACAPI_GetObjectProperty(prop_id,object);
 if (!prop) return -1;

 BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
 if (val->application_tag != BACNET_APPLICATION_TAG_DOUBLE) return -1;

 val->type.Double = value;
 return 0;
}



int BACAPI_SetObjectPropertyValueEnum(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, uint32_t value)
{
 BACNET_OBJECT_PROPERTY *prop = BACAPI_GetObjectProperty(prop_id,object);
 if (!prop) return -1;

 BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
 if (val->application_tag != BACNET_APPLICATION_TAG_ENUM) return -1;

 val->type.Enumerated = value;
 return 0;
}

int BACAPI_SetObjectPropertyValueBool(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, bool value)
{
 BACNET_OBJECT_PROPERTY *prop = BACAPI_GetObjectProperty(prop_id,object);
 if (!prop) return -1;

 BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
 if (val->application_tag != BACNET_APPLICATION_TAG_BOOL) return -1;

 val->type.Boolean = value;
 return 0;
}



int BACAPI_SetObjectPropertyValueUint(BACNET_OBJECT *object, BACNET_PROPERTY_ID prop_id, uint32_t value)
{
 BACNET_OBJECT_PROPERTY *prop = BACAPI_GetObjectProperty(prop_id,object);
 if (!prop) return -1;

 BACNET_APPLICATION_DATA_VALUE *val = prop->vValueList[0];
 if (val->application_tag != BACNET_APPLICATION_TAG_UINT) return -1;

 val->type.Unsigned_Int = value;
 return 0;
}




void BACAPI_DeleteDevice(BACNET_DEVICE *device)
{
 if (device)
    {
     int objCnt = (int)device->vObjects.size();
     for (int c=0;c<objCnt;c++)
         {
          BACNET_OBJECT *object = device->vObjects[c];
          BACAPI_DeleteObject(object);
         }
    }else return;

 device->vObjects.clear();


 //gLog->AppendLog("Deleting npdu data\n");
 BACAPI_DeleteNPDUData(device->npduData);
 delete device;
 device = NULL;
}



void BACAPI_DeleteObjectList(BACNET_DEVICE *device)
{
 if (device)
    {
     gLog->AppendLog("Device exist");
     int objCnt = (int)device->vObjects.size();
     gLog->AppendLog("Object cnt = %d",objCnt);
     for (int c=0;c<objCnt;c++)
         {
          BACNET_OBJECT *object = device->vObjects[c];
          gLog->AppendLog("Deleting Object %d",c);
          BACAPI_DeleteObject(object);
          object = NULL;
         }
    }else return;

 device->vObjects.clear();
}




void BACAPI_DeleteNPDUData(BACNET_NPDU_DATA *npdu)
{
 if (npdu)
    {
     if (npdu->dadr)
        {
         delete npdu->dadr;
         npdu->dadr = NULL;
        }
     if (npdu->sadr)
        {
         delete npdu->sadr;
         npdu->sadr = NULL;
        }
     if (npdu->remote)
        {
         delete npdu->remote;
         npdu->remote = NULL;
        }
     delete npdu;
     npdu = NULL;
    }
}



void BACAPI_DeleteObject(BACNET_OBJECT *object)
{
 BACAPI_DeleteProperties(object);
 delete object;
 object = NULL;
}



void BACAPI_DeleteProperties(BACNET_OBJECT *object)
{
 int cnt = (int)object->vProperties.size();

 for (int c=0;c<cnt;c++)
	 {
	  BACNET_OBJECT_PROPERTY *prop = object->vProperties[c];
	  BACAPI_DeleteObjectProperty(prop);
	 }
 object->vProperties.clear();
}



// NOTE: delete prop stvara probleme na linuxu. Trenutno sam zakomentirao delete prop. 24.3.2017
// NOTE: potvrdjeno, 21.7.2017
void BACAPI_DeleteObjectProperty(BACNET_OBJECT_PROPERTY *prop)
{
#ifdef BACAPI_BUILD_WIN32
 if (prop)
    {
	 if (BACAPI_IsComplexProperty(prop->property))
		{
		 BACAPI_DeleteComplexProperty(prop);
		}else {
		       int count = (size_t)prop->vValueList.size();
		       for (int c=0;c<count;c++)
		           {
		            BACNET_APPLICATION_DATA_VALUE *value = prop->vValueList[c];
			        BACAPI_DeletePropertyValue(value);
			       }
			   prop->vValueList.clear();
			  }
	 delete prop;
	}
#endif
 prop = NULL;
}



void BACAPI_DeleteProperty(BACNET_OBJECT *object, BACNET_PROPERTY_ID propertyId)
{
 if (object)
    {
     size_t count = object->vProperties.size();
     for (int c=0;c<count;c++)
         {
          BACNET_OBJECT_PROPERTY *prop = object->vProperties[c];
          if (prop->property == propertyId)
             {
              BACAPI_DeleteObjectProperty(prop);
              prop = NULL;
              object->vProperties.erase(object->vProperties.begin()+c);
             }
         }
    }
}



void BACAPI_DeletePropertyValue(BACNET_APPLICATION_DATA_VALUE *value)
{
 value->type.character_String.value8.clear();
 value->type.character_String.value16.clear();
 value->type.Octet_String.vValue.clear();

 delete value;
}




BACNET_APPLICATION_DATA_VALUE *BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG apptag)
{
  BACNET_APPLICATION_DATA_VALUE *value = new BACNET_APPLICATION_DATA_VALUE;
#ifdef BACAPI_BUILD_WIN32
  // Na linuxu ce se nulirati i character stringgovi. npr. character_string.value8 ce se
  // nulirati i izbacit ce se SIGSEV ako se pokusa pridruziti string.
  // na Win32 se nece to dogoditi.
  memset(value,0,sizeof(BACNET_APPLICATION_DATA_VALUE));  // nuliraj
#endif
  value->application_tag = apptag;

  switch(apptag)
        {
         case BACNET_APPLICATION_TAG_NULL:
		 case BACNET_APPLICATION_TAG_BOOL:
		 case BACNET_APPLICATION_TAG_UINT:
		 case BACNET_APPLICATION_TAG_SINT:
		 case BACNET_APPLICATION_TAG_REAL:

		 case BACNET_APPLICATION_TAG_DOUBLE:
		 case BACNET_APPLICATION_TAG_ENUM:
		 value->type.Enumerated = 0;
		 value->type.Boolean = 0;
		 value->type.Unsigned_Int = 0;
		 value->type.Signed_Int = 0;
		 value->type.Real = 0;
		 value->type.Double = 0;
		 break;

		 case BACNET_APPLICATION_TAG_BSTRING:
		 value->type.Bit_String.bits_used = 0;
		 break;


		 case BACNET_APPLICATION_TAG_OSTRING:
		 break;

		 case BACNET_APPLICATION_TAG_CSTRING:
         break;

		 case BACNET_APPLICATION_TAG_DATE:
		 value->type.Date.day = 1;
		 value->type.Date.month = 1;
		 value->type.Date.wday = 1;
		 value->type.Date.year = 2014;
		 break;
		 case BACNET_APPLICATION_TAG_TIME:
		 value->type.Time.hour = 1;
		 value->type.Time.hundredths = 1;
		 value->type.Time.minute = 1;
		 value->type.Time.sec = 1;
		 break;

		 case BACNET_APPLICATION_TAG_OID:
		 value->type.Object_Id.type = 0;
		 value->type.Object_Id.instance = 0;
		 break;
        }

  return value;
}

void BACAPI_DeleteApplicationDataValue(BACNET_APPLICATION_DATA_VALUE *value)
{
 if (value == NULL)
	{
	 return;
	}

 if (value->application_tag == BACNET_APPLICATION_TAG_CSTRING)
	{
	 value->type.character_String.encoding = CHAR_SET_ANSI_X34;
	 //value->type.character_String.value16.clear();
	 value->type.character_String.value8.clear();
	}

 if (value->application_tag == BACNET_APPLICATION_TAG_OSTRING)
    {
     value->type.Octet_String.vValue.clear();
    }

 delete value;
 value = NULL;
}



void BACAPI_DeleteObjectPropertyValues(BACNET_OBJECT_PROPERTY *prop)
{
 int count = (int)prop->vValueList.size();

 for (int c=0;c<count;c++)
     {
      BACNET_APPLICATION_DATA_VALUE *value = prop->vValueList[c];
      BACAPI_DeleteApplicationDataValue(value);
     }

 prop->vValueList.clear();
}











void BACAPI_DeleteComplexProperty(BACNET_OBJECT_PROPERTY *prop)
{
 switch (prop->property)
        {

         case PROP_ZONE_FROM:
         case PROP_ZONE_TO:
         case PROP_CREDENTIALS_IN_ZONE:
         case PROP_LAST_CREDENTIAL_ADDED:
         case PROP_LAST_CREDENTIAL_REMOVED:
         case PROP_ACCESS_EVENT_CREDENTIAL:
         case PROP_MEMBERS:
         case PROP_CREDENTIALS:
         //case PROP_ACCOMPANIED:
         case PROP_BELONGS_TO:
         case PROP_LAST_ACCESS_POINT:
         case PROP_LOG_DEVICE_OBJECT_PROPERTY:
			  {
			   size_t count = prop->complexValue.vDeviceObjectPropertyReference.size();
			   for (int c=0;c<count;c++)
			       {
			        BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *ref = prop->complexValue.vDeviceObjectPropertyReference[c];
                    delete ref;
			       }
			   prop->complexValue.vDeviceObjectPropertyReference.clear();
               break;
              }




         case PROP_TIME_OF_ACTIVE_TIME_RESET:
         case PROP_TIME_OF_STATE_COUNT_RESET:
         case PROP_CHANGE_OF_STATE_TIME:
         case PROP_MAXIMUM_VALUE_TIMESTAMP:
         case PROP_MINIMUM_VALUE_TIMESTAMP:
         case PROP_START_TIME:
         case PROP_STOP_TIME:
         case PROP_MODIFICATION_DATE:
         case PROP_UPDATE_TIME:
         case PROP_COUNT_CHANGE_TIME:
         case PROP_LAST_CREDENTIAL_ADDED_TIME:
         case PROP_LAST_CREDENTIAL_REMOVED_TIME:
         case PROP_ACTIVATION_TIME:
         case PROP_EXPIRATION_TIME:
         case PROP_LAST_USE_TIME:
			  {
			   // objekti se ne alociraju
			   break;
			  }


         case PROP_SETPOINT_REFERENCE:
         case PROP_OBJECT_PROPERTY_REFERENCE:
         case PROP_MANIPULATED_VARIABLE_REFERENCE:
         case PROP_CONTROLLED_VARIABLE_REFERENCE:
         case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
         case PROP_INPUT_REFERENCE:
			  {
			   size_t count = prop->complexValue.vObjectPropertyReference.size();
			   for (int c=0;c<count;c++)
			       {
			        BACNET_OBJECT_PROPERTY_REFERENCE *ref = prop->complexValue.vObjectPropertyReference[c];
                    BACAPI_DeleteObjectPropRef(ref);
			       }
			   prop->complexValue.vObjectPropertyReference.clear();
			   break;
			  }

         case PROP_SUBORDINATE_LIST:
         case PROP_DOOR_MEMBERS:
         case PROP_ZONE_MEMBERS:
         case PROP_MEMBER_OF:
         case PROP_ENTRY_POINTS:
         case PROP_EXIT_POINTS:
			  {
			   BACAPI_DeleteDeviceObjectRef(prop);
			   break;
			  }






         case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
         case PROP_RESTART_NOTIFICATION_RECIPIENTS:
         case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
              {
               BACAPI_DeleteRecipientList(prop);
               break;
              }
         break;



         case PROP_RECIPIENT_LIST:
			  {
			   size_t count = prop->complexValue.vDestinationList.size();
			   for (int c=0;c<count;c++)
			       {
			        BACNET_DESTINATION *dest = prop->complexValue.vDestinationList[c];
                    BACAPI_DeleteDestination(dest);
                   }
               prop->complexValue.vDestinationList.clear();
			   break;
			  }
         break;


         case PROP_EFFECTIVE_PERIOD:
              {

               break;
              }


         case PROP_EVENT_TIME_STAMPS:
         case PROP_LAST_RESTORE_TIME:
         case PROP_TIME_OF_DEVICE_RESTART:
         case PROP_ACCESS_EVENT_TIME:
			  {
			   size_t count = prop->complexValue.vTimeStamps.size();
			   for (int c=0;c<count;c++)
			       {
			        BACNET_TIMESTAMP *ptr_stamp = prop->complexValue.vTimeStamps[c];
                    BACAPI_DeleteTimeStamp(ptr_stamp);
			       }
			   prop->complexValue.vTimeStamps.clear();
			   break;
			  }

         case PROP_ACTIVE_COV_SUBSCRIPTIONS:
              {

               break;
              }
         break;


         case PROP_DATE_LIST:
              {
			   size_t count = prop->complexValue.vCalendarEntryList.size();
			   for (int c=0;c<count;c++)
			       {
			        BACNET_CALENDAR_ENTRY *ptr_entry = prop->complexValue.vCalendarEntryList[c];
                    BACAPI_DeleteCalendarEntry(ptr_entry);
			       }
			   prop->complexValue.vCalendarEntryList.clear();
               break;
              }
         break;

         case PROP_PRIORITY_ARRAY:
			  {
			   // KOMENTIRANO 13.11.2013
               BACAPI_DeleteObjectPropertyValues(prop);
			   break;
			  }


         case PROP_ACTION:
              {
               if (prop->vValueList.size() > 0)
                  {
  			       BACAPI_DeleteObjectPropertyValues(prop);   // LOOP OBJECT - PROP_ACTION prop
			      }else {
						 BACAPI_DeleteBACnetActionList(prop);   // COMMAND_OBJECT - PROP_ACTION prop
						}
               break;
              }

         case PROP_ACTION_TEXT:
              {
               // TODO: moguc mali mem leak ovdje. Provjeriti
               BACAPI_DeleteActionText(prop->complexValue.actionText);
               break;
              }

		 case PROP_ACCEPTED_MODES:
		 case PROP_LIFE_SAFETY_ALARM_VALUES:
		 case PROP_ALARM_VALUES:
		 case PROP_FAULT_VALUES:
		      {
		       // komentirano 22.11.2013
               //BACAPI_DeleteEnumList(prop);
		       break;
		      }




         case PROP_WEEKLY_SCHEDULE:
              {
               BACAPI_DeleteWeeklySchedule(&prop->complexValue.weeklySchedule);
               break;
              }
         break;

         case PROP_EXCEPTION_SCHEDULE:
              {
               BACAPI_DeleteSpecialEvent(prop);
               break;
              }
         break;


         case PROP_LIST_OF_GROUP_MEMBERS:
              {
               BACAPI_DeleteReadAccessSpec(prop->complexValue.readAccessSpec);
              }
         break;

         case PROP_DEVICE_ADDRESS_BINDING:
         case PROP_SLAVE_ADDRESS_BINDING:
              {
               uint32_t count = prop->complexValue.vBACnetAddressBindingList.size();
               for (uint32_t c=0;c<count;c++)
                   {
                    BACNET_ADDRESS_BINDING *adr_bind = prop->complexValue.vBACnetAddressBindingList[c];
                    BACAPI_DeleteDeviceAddressBinding(adr_bind);
                   }
               prop->complexValue.vBACnetAddressBindingList.clear();
               break;
              }




         break;
        }
}



bool BACAPI_IsSpecialProperty(BACNET_OBJECT_TYPE type, BACNET_PROPERTY_ID prop_id)
{

 switch(prop_id)
       {
        case PROP_ACTION:
         return true;

       case PROP_PRESENT_VALUE:
        switch (type)
               {
                case OBJECT_GROUP:
                case OBJECT_GLOBAL_GROUP:
                case OBJECT_CHANNEL:
                case OBJECT_SCHEDULE:
                case OBJECT_CREDENTIAL_DATA_INPUT:
                 return true;
               }
       default: return false;
      }
      
      
 return false;
}




void BACAPI_DeleteDeviceAddressBinding(BACNET_ADDRESS_BINDING *adr_bind)
{
 delete adr_bind;
}


bool BACAPI_IsComplexProperty(BACNET_PROPERTY_ID propid)
{
 for (int c=0;c<BACAPI_COMPLEX_PROPERTY_COUNT;c++)
     {
      BACNET_PROPERTY_ID listprop = BACAPI_ComplexPropertyList[c];
      if (propid == listprop)
         {
          return true;
         }
     }

 return false;
}



// svi propertiji veci od 511 su propriatery 512-4194303
bool BACAPI_IsPropriateryProperty(BACNET_PROPERTY_ID propid)
{
 if ((uint32_t)propid >= BACNET_PROPERTY_COUNT) return true;
 return false;
}






char *BACAPI_utf16_to_utf8(const wchar_t *str, int len)
{

 int n = 0;
 char *ret = NULL;


/*
 if (len == 1 || len == 0)
    {
     return NULL;
    }


 n = WideCharToMultiByte(CP_UTF8, 0,str,-1,0,0,NULL,NULL);
 if (n)
    {
     gLog->AppendLog("BACAPI_utf16_to_utf8: new n = %d\n",n);
     ret = new char[n];
     if (ret)
        {
         n = WideCharToMultiByte(CP_UTF8, 0,str,-1,ret,n,NULL,NULL);
         if (n == 0)
            {
             gLog->AppendLog("BACAPI_utf16_to_utf8: Deleting ret[]\n",n);
             delete []ret;
             ret = NULL;
            }
        }
    }
    */
 return ret;
}


void BACAPI_utf8_free(char *str)
{
 int len = strlen(str);

 if (len > 0)
    {
     delete []str;
    }
}



wchar_t *BACAPI_utf8_to_utf16(const char *str)
{
	int n = 0;
	wchar_t* ret = NULL;
/*
	// first determine required size to hold converted string
	n = MultiByteToWideChar(CP_UTF8,0,str,-1,ret,0);
	if (n != 0)
	{
		ret = (wchar_t*)malloc(n * sizeof(wchar_t));
		if (ret)
		{
			n = MultiByteToWideChar(CP_UTF8,0,str,-1,ret,n);
			if (n == 0)
			{
				// conversion failed
				free(ret);
				ret = NULL;
			}
		}

	}

*/

	return ret;
}




void BACAPI_DeleteObjectPropRef(BACNET_OBJECT_PROPERTY_REFERENCE *ref)
{
 if (ref == NULL)
    {
     return;
    }

 delete ref;
}



void BACAPI_DeleteDeviceObjectPropRef(BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *ref)
{
 //if (ref == NULL)
 //   {
 //    return;
 //   }


 //BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *next = ref->next;

 //delete ref;
 //ref = NULL;

 //BACAPI_DeleteDeviceObjectPropRef(next);
}



void BACAPI_DeleteRecipientList(BACNET_OBJECT_PROPERTY *property)
{
 size_t count = property->complexValue.vRecipientList.size();
 for (int c=0;c<count;c++)
     {
      BACNET_RECIPIENT *ptr_recipient = property->complexValue.vRecipientList[c];
      delete ptr_recipient;
      ptr_recipient = NULL;
     }
 property->complexValue.vRecipientList.clear();
}




void BACAPI_DeleteDeviceObjectRef(BACNET_OBJECT_PROPERTY *prop)
{
 int count = (size_t)prop->complexValue.vDeviceObjectReference.size();

 for (int c=0;c<count;c++)
     {
      BACNET_DEVICE_OBJECT_REFERENCE *ref = prop->complexValue.vDeviceObjectReference[c];
      delete ref;
     }

 prop->complexValue.vDeviceObjectReference.clear();
}



void BACAPI_DeleteObjectPropertyRef(BACNET_OBJECT_PROPERTY *prop)
{
 int count = (size_t)prop->complexValue.vObjectPropertyReference.size();

 for (int c=0;c<count;c++)
     {
      BACNET_OBJECT_PROPERTY_REFERENCE *ref = prop->complexValue.vObjectPropertyReference[c];
      delete ref;
     }

 prop->complexValue.vObjectPropertyReference.clear();
}




void BACAPI_DeleteEnumList(BACNET_APPLICATION_DATA_VALUE *val)
{
  if (val == NULL)
    {
     return;
    }

// BACNET_APPLICATION_DATA_VALUE *next = val->next;

 delete val;
 val = NULL;

// BACAPI_DeleteEnumList(next);
}




void BACAPI_DeleteTimeStamp(BACNET_TIMESTAMP *stamp)
{
 delete stamp;
 stamp = NULL;
}





void BACAPI_DeleteDestination(BACNET_DESTINATION *dest)
{
 delete dest;
}



void BACAPI_DeleteReadAccessSpec(BACNET_READ_ACCESS_SPECIFICATION *spec)
{
 if (spec == NULL)
    {
     return;
    }

 BACNET_READ_ACCESS_SPECIFICATION *next = spec->next;

 delete spec;
 spec = NULL;

 BACAPI_DeleteReadAccessSpec(next);
}




void BACAPI_DeleteWeeklySchedule(BACNET_WEEKLY_SCHEDULE *sch)
{
 for (int c=0;c<7;c++)
     {
      sch->weeklySchedule[c].vDaySchedule.clear();
     }
}


void BACAPI_DeleteSpecialEvent(BACNET_OBJECT_PROPERTY *prop)
{
 size_t count = prop->complexValue.vSpecialEventList.size();
 for (size_t c=0;c<count;c++)
     {
      BACNET_SPECIAL_EVENT *event = &prop->complexValue.vSpecialEventList[c];

      //int a = event->vTimeValueList.size();

      event->vTimeValueList.clear();
     }
 prop->complexValue.vSpecialEventList.clear();
 return;
}


void BACAPI_DeleteCalendarEntry(BACNET_CALENDAR_ENTRY *entry)
{
 if (!entry) return;
 delete entry;
 entry = NULL;
}



// komentirano 22.11.2013
void BACAPI_DeleteCOVValue(BACNET_COV_VALUE *val)
{
 //if (val == NULL)
 //   {
 //    return;
 //   }

 //BACAPI_DeleteApplicationDataValue(val->value);

 //BACNET_COV_VALUE *next = val->next;

 //delete val;
 //val = NULL;


 //BACAPI_DeleteCOVValue(next);
}









void BACAPI_DeleteBACnetAlarm(BACNET_ALARM *alarm)
{
 if (alarm) delete alarm;
 return;
}



void BACAPI_DeleteBACnetActionList(BACNET_OBJECT_PROPERTY *property)
{
 if (!property) return;

 size_t count = property->complexValue.vBACnetActionList.size();
 for (size_t c=0;c<count;c++)
     {
      BACNET_ACTION_LIST *ptr_list = property->complexValue.vBACnetActionList[c];
      BACAPI_DeleteActionCommandList(ptr_list);
     }
 property->complexValue.vBACnetActionList.clear();
}



void BACAPI_DeleteActionCommandList(BACNET_ACTION_LIST *list)
{
 if (!list) return;

 size_t com_count = list->vBACnetActionCommand.size();
 for (size_t i=0;i<com_count;i++)
     {
      BACNET_ACTION_COMMAND *ptr_command = &list->vBACnetActionCommand[i];
      BACAPI_DeleteApplicationDataValue(ptr_command->property->value);
     }
 list->vBACnetActionCommand.clear();
 delete list;
}




void BACAPI_DeleteCOVNotification(BACNET_COV_NOTIFICATION *cov)
{
 if (!cov) return;

 size_t count = cov->vValueList.size();
 for (int c=0;c<count;c++)
     {
      BACNET_COV_VALUE *cov_val = &cov->vValueList[c];
      BACAPI_DeleteApplicationDataValue(cov_val->value);
     }

 delete cov;
}


void BACAPI_DeleteActionText(BACNET_ACTION_TEXT *text)
{
 //if (text == NULL)
 //   {
 //    return;
 //   }


 //if (text->value.type.character_String.encoding == CHAR_SET_UCS2)
	//{
	// //delete []text->value.type.character_String.value16;
	// text->value.type.character_String.value16.clear();
 //   }else if (text->value.type.character_String.encoding == CHAR_SET_ANSI_X34)
	//	     {
	//		  if (text->value.type.character_String.value8)
	//		     {
	//			  //delete text->value.type.character_String.value8;
	//			  text->value.type.character_String.value8.clear();
	//			 }
	//		 }

 //delete text->value.type.character_String;

 //BACNET_ACTION_TEXT *next = text->next;
 //text = NULL;

 //BACAPI_DeleteActionText(next);

}




void BACAPI_ZeroString(char *str, int size)
{
 for (int c=0;c<size;c++)
     {
      str[c] = 0;
     }

}


BACNET_NPDU_DATA *BACAPI_CreateNPDUData(BACNET_NPDU_DATA *src)
{
 BACNET_NPDU_DATA *data = new BACNET_NPDU_DATA;
 memcpy(data,src,sizeof(BACNET_NPDU_DATA));
 data->sadr = BACAPI_CreateBACnetAddress(src->sadr);
 data->dadr = BACAPI_CreateBACnetAddress(src->dadr);
 data->remote = BACAPI_CreateBACnetAddress(src->remote);
 return data;
}


BACNET_NPDU_DATA *BACAPI_CreateNPDUData(BACNET_ADDRESS *src, BACNET_ADDRESS *dest, BACNET_ADDRESS *remote, BACNET_DATALINK dlink)
{
 BACNET_NPDU_DATA *data = new BACNET_NPDU_DATA;
 memset(data,0,sizeof(BACNET_NPDU_DATA));

 data->sadr = BACAPI_CreateBACnetAddress(src);
 data->dadr = BACAPI_CreateBACnetAddress(dest);
 data->remote = BACAPI_CreateBACnetAddress(remote);;
 data->hopCount = 255;
 data->protocolVersionNumber = 0;
 data->network_layer_message = false;
 data->dataLink = dlink;
 data->priority = 0;
 data->npduMessageType = (BACNET_NPDU_MESSAGE_TYPE)0;
 data->vendor_id = 0;  // ovaj parametar se nalazi ovdje zato sto DecodeNPDU dekodira ovaj parametar.
                       // trebalo bi ujediniti ovaj i dev->vendorId parametar.{

 return data;
}


BACNET_NPDU_DATA *BACAPI_CreateNPDUData(BACNET_ADDRESS *src, BACNET_ADDRESS *dest)
{

 BACNET_NPDU_DATA *data = new BACNET_NPDU_DATA;
 memset(data,0,sizeof(BACNET_NPDU_DATA));
 data->dadr = BACAPI_CreateBACnetAddress(dest);
 data->sadr = BACAPI_CreateBACnetAddress(src);
 data->remote = NULL;
 data->hopCount = 255;
 data->protocolVersionNumber = 0;
 data->network_layer_message = false;
 data->priority = 0;
 data->npduMessageType = (BACNET_NPDU_MESSAGE_TYPE)0;
 data->vendor_id = 0;  // ovaj parametar se nalazi ovdje zato sto DecodeNPDU dekodira ovaj parametar.
                       // trebalo bi ujediniti ovaj i dev->vendorId parametar.

 return data;
}





BACNET_DEVICE *BACAPI_CreateBACnetDevice(uint32_t instance,uint16_t maxApdu, BACNET_SEGMENTATION seg, uint16_t vendor)
{
 BACNET_DEVICE *dev = new BACNET_DEVICE;
 memset(dev,0,sizeof(BACNET_DEVICE));
 dev->oid.instance = instance;
 dev->oid.type = OBJECT_DEVICE;
 dev->maximumAPDU = maxApdu;
 dev->segmentation = seg;
 dev->vendorId = vendor;
 dev->npduData = NULL;
 return dev;
}


BACNET_DEVICE *BACAPI_CreateBACnetDevice(void)
{
 BACNET_DEVICE *dev = new BACNET_DEVICE;
 memset(dev,0,sizeof(BACNET_DEVICE));

 return dev;
}




BACNET_DEVICE *BACAPI_CreateBACnetDevice(BACNET_DEVICE *dev)
{
 BACNET_DEVICE *new_device = BACAPI_CreateBACnetDevice(dev->oid.instance,dev->maximumAPDU,dev->segmentation,dev->vendorId);

 if (dev->npduData)
    {
	 new_device->npduData = BACAPI_CreateNPDUData(0,0);


	 memcpy(new_device->npduData,dev->npduData,sizeof(BACNET_NPDU_DATA));

	 if (new_device->npduData->dadr)
		{
		 new_device->npduData->sadr = BACAPI_CreateBACnetAddress(dev->npduData->sadr);
		 new_device->npduData->dadr = BACAPI_CreateBACnetAddress(dev->npduData->dadr);
		 new_device->npduData->remote = BACAPI_CreateBACnetAddress(dev->npduData->remote);
		}
    }
 return new_device;
}



BACNET_OBJECT *BACAPI_CreateBACnetObject(uint16_t type, uint32_t instance)
{
 BACNET_OBJECT *obj = new BACNET_OBJECT;
 memset(obj,0,sizeof(BACNET_OBJECT));
 obj->oid.instance = instance;
 obj->oid.type = type;
 obj->bac_file.access_type = 0;
 obj->bac_file.eof = 0;
 obj->bac_file.octet_count = 0;
 obj->bac_file.oid.instance = instance;
 obj->bac_file.oid.type = type;
 obj->bac_file.start_pos = 0;
 return obj;
}




//---- Zastarjelo, koristiti BACAPI_DeleteBACnetDevice()
//void BACAPI_DeleteBACnetDevice(BACNET_DEVICE *dev)
//{
// uint32_t obj_count = dev->vObjects.size();
// for (uint32_t c=0;c<obj_count;c++)
//     {
//      BACNET_OBJECT *obj = dev->vObjects[c];
//      BACAPI_DeleteObject(obj);
//     }
//
// if (dev->npduData)
//    {
//     if (dev->npduData->dadr) delete dev->npduData->dadr;
//     if (dev->npduData->sadr) delete dev->npduData->sadr;
//     if (dev->npduData->local) delete dev->npduData->local;
//     if (dev->npduData->remote) delete dev->npduData->remote;
//
//     delete dev->npduData;
//    }
//
// delete dev;
//}

// LEGACY: Ne koristiti vise i izbaciti
//bool BACAPI_Is2004Standard(BACNET_OBJECT_TYPE object)
//{
// for (int c=0;c<BACAPI_OBJECT_2004_COUNT;c++)
//     {
//      if (object == BACAPI_Object_2004_List[c])
//         {
//          return true;
//         }
//     }
// return false;
//}




BACNET_PROPERTY_ID *BACAPI_GetPropertyList(BACNET_OBJECT_TYPE object)
{

 switch (object)
        {
		 case OBJECT_ACCUMULATOR:
              return BACAPI_Object_Accumulator;
		 case OBJECT_ANALOG_INPUT:
              return BACAPI_Object_Analog_Input;
		 case OBJECT_ANALOG_OUTPUT:
              return BACAPI_Object_Analog_Output;
		 case OBJECT_ANALOG_VALUE:
              return BACAPI_Object_Analog_Value;
		 case OBJECT_BINARY_INPUT:
              return BACAPI_Object_Binary_Input;
		 case OBJECT_BINARY_OUTPUT:
              return BACAPI_Object_Binary_Output;
		 case OBJECT_BINARY_VALUE:
              return BACAPI_Object_Binary_Value;
		 case OBJECT_AVERAGING:
              return BACAPI_Object_Averaging;
		 case OBJECT_CALENDAR:
              return BACAPI_Object_Calendar;
		 case OBJECT_COMMAND:
              return BACAPI_Object_Command;
		 case OBJECT_DEVICE:
              return BACAPI_Object_Device;
		 case OBJECT_EVENT_ENROLLMENT:
              return BACAPI_Object_Event_Enrollment;
		 case OBJECT_FILE:
              return BACAPI_Object_File;
		 case OBJECT_GROUP:
              return BACAPI_Object_Group;
		 case OBJECT_LIFE_SAFETY_POINT:
              return BACAPI_Object_Life_Safety_Point;
		 case OBJECT_LIFE_SAFETY_ZONE:
              return BACAPI_Object_Life_Safety_Zone;
		 case OBJECT_LOOP:
              return BACAPI_Object_Loop;
		 case OBJECT_MULTI_STATE_INPUT:
              return BACAPI_Object_Multi_State_Input;
		 case OBJECT_MULTI_STATE_OUTPUT:
              return BACAPI_Object_Multi_State_Output;
		 case OBJECT_MULTI_STATE_VALUE:
              return BACAPI_Object_Multi_State_Value;
		 case OBJECT_NOTIFICATION_CLASS:
              return BACAPI_Object_Notification_Class;
		 case OBJECT_PROGRAM:
              return BACAPI_Object_Program;
		 case OBJECT_PULSE_CONVERTER:
              return BACAPI_Object_Pulse_Converter;
		 case OBJECT_SCHEDULE:
              return BACAPI_Object_Schedule;
		 case OBJECT_TREND_LOG:
              return BACAPI_Object_Trend_Log;

		 case OBJECT_EVENT_LOG: return BACAPI_Object_Event_Log;
		 case OBJECT_GLOBAL_GROUP: return BACAPI_Object_Global_Group;
		 case OBJECT_TREND_LOG_MULTIPLE:return BACAPI_Object_Trend_Log_Multiple;
		 case OBJECT_LOAD_CONTROL:return BACAPI_Object_Load_Control;
		 case OBJECT_STRUCTURED_VIEW:return NULL;
		 case OBJECT_ACCESS_DOOR:return BACAPI_Object_Access_Door;
		 case OBJECT_ACCESS_CREDENTIAL:return BACAPI_Object_Access_Credential;
		 case OBJECT_ACCESS_POINT:return BACAPI_Object_Access_Point;
		 case OBJECT_ACCESS_RIGHTS:return BACAPI_Object_Access_Rights;
		 case OBJECT_ACCESS_USER:return BACAPI_Object_Access_User;
		 case OBJECT_ACCESS_ZONE:return BACAPI_Object_Access_Zone;
		 case OBJECT_CREDENTIAL_DATA_INPUT:return BACAPI_Object_Credential_Data_Input;
		 case OBJECT_NETWORK_SECURITY:return BACAPI_Object_Network_Security;
		 case OBJECT_BITSTRING_VALUE: return BACAPI_Object_BitString_Value;
		 case OBJECT_CHARACTERSTRING_VALUE:return BACAPI_Object_CharacterString_Value;
		 case OBJECT_DATE_PATTERN_VALUE:return BACAPI_Object_Date_Pattern_Value;
		 case OBJECT_DATE_VALUE:return BACAPI_Object_Date_Value;
		 case OBJECT_DATETIME_PATTERN_VALUE:return BACAPI_Object_DateTime_Pattern_Value;
		 case OBJECT_DATETIME_VALUE:return BACAPI_Object_DateTime_Value;
		 case OBJECT_INTEGER_VALUE :return BACAPI_Object_Integer_Value;
		 case OBJECT_LARGE_ANALOG_VALUE:return BACAPI_Object_Large_Analog_Value;
		 case OBJECT_OCTETSTRING_VALUE:return BACAPI_Object_OctetString_Value;
		 case OBJECT_POSITIVE_INTEGER_VALUE:return BACAPI_Object_Positive_Integer_Value;
		 case OBJECT_TIME_PATTERN_VALUE:return BACAPI_Object_Time_Pattern_Value;
		 case OBJECT_TIME_VALUE:return BACAPI_Object_Time_Value;
		 case OBJECT_NOTIFICATION_FORWARDER:return BACAPI_Object_Notification_Forwarder;
		 case OBJECT_ALERT_ENROLLMENT:return BACAPI_Object_Alert_Enrollment;
		 case OBJECT_CHANNEL:return BACAPI_Object_Channel;
		 case OBJECT_LIGHTING_OUTPUT:return BACAPI_Object_Lighting_Output;
         case OBJECT_NETWORK_PORT: return BACAPI_Object_Network_Port;
         case OBJECT_BINARY_LIGHTING_OUTPUT: return BACAPI_Object_Binary_Lighting_Output;
         case OBJECT_ELEVATOR_GROUP: return BACAPI_Object_Elevator_Group;
         case OBJECT_ESCALATOR: return BACAPI_Object_Escalator;
         case OBJECT_LIFT: return BACAPI_Object_Lift;
         case OBJECT_TIMER: return BACAPI_Object_Timer;

         case OBJECT_LINK_OBJECT: return BACAPI_Object_Link_Object;


         default: return NULL;
        }

 return 0;
}



int BACAPI_GetPropertyCount(BACNET_OBJECT_TYPE object)
{


 switch (object)
        {
		 case OBJECT_ACCUMULATOR:
              return BACAPI_ACCUMULATOR_PROP_COUNT;
		 case OBJECT_ANALOG_INPUT:
              return BACAPI_ANALOG_INPUT_PROP_COUNT;
		 case OBJECT_ANALOG_OUTPUT:
              return BACAPI_ANALOG_OUTPUT_PROP_COUNT;
		 case OBJECT_ANALOG_VALUE:
              return BACAPI_ANALOG_VALUE_PROP_COUNT;
		 case OBJECT_BINARY_INPUT:
              return BACAPI_BINARY_INPUT_PROP_COUNT;
		 case OBJECT_BINARY_OUTPUT:
              return BACAPI_BINARY_OUTPUT_PROP_COUNT;
		 case OBJECT_BINARY_VALUE:
              return BACAPI_BINARY_VALUE_PROP_COUNT;
		 case OBJECT_AVERAGING:
              return BACAPI_AVERAGING_PROP_COUNT;
		 case OBJECT_CALENDAR:
              return BACAPI_CALENDAR_PROP_COUNT;
		 case OBJECT_COMMAND:
              return BACAPI_COMMAND_PROP_COUNT;

		 case OBJECT_DEVICE:
              return BACAPI_DEVICE_PROP_COUNT;

		 case OBJECT_EVENT_ENROLLMENT:
              return BACAPI_EVENT_ENROLLMENT_PROP_COUNT;

		 case OBJECT_FILE:
              return BACAPI_FILE_PROP_COUNT;

		 case OBJECT_GROUP:
              return BACAPI_GROUP_PROP_COUNT;

		 case OBJECT_LIFE_SAFETY_POINT:
              return BACAPI_LIFE_SAFETY_POINT_PROP_COUNT;

		 case OBJECT_LIFE_SAFETY_ZONE:
              return BACAPI_LIFE_SAFETY_ZONE_PROP_COUNT;

		 case OBJECT_LOOP:
              return BACAPI_LOOP_PROP_COUNT;

		 case OBJECT_MULTI_STATE_INPUT:
              return BACAPI_MSI_PROP_COUNT;

		 case OBJECT_MULTI_STATE_OUTPUT:
              return BACAPI_MSO_PROP_COUNT;

		 case OBJECT_MULTI_STATE_VALUE:
              return BACAPI_MSV_PROP_COUNT;

		 case OBJECT_NOTIFICATION_CLASS:
              return BACAPI_NOTIFICATION_CLASS_PROP_COUNT;

		 case OBJECT_PROGRAM:
              return BACAPI_PROGRAM_PROP_COUNT;

		 case OBJECT_PULSE_CONVERTER:
              return BACAPI_PULSE_CONVERTER_PROP_COUNT;

		 case OBJECT_SCHEDULE:
              return BACAPI_SCHEDULE_PROP_COUNT;

		 case OBJECT_TREND_LOG:
              return BACAPI_TREND_LOG_PROP_COUNT;

         case OBJECT_LINK_OBJECT:
         return BACAPI_LINK_OBJECT_PROP_COUNT;

         case OBJECT_ACCESS_DOOR:
              return BACAPI_ACCESS_DOOR_PROP_COUNT;

         case OBJECT_EVENT_LOG:
              return BACAPI_EVENT_LOG_PROP_COUNT;
         case OBJECT_LOAD_CONTROL:
              return BACAPI_LOAD_CONTROL_PROP_COUNT;
         case OBJECT_TREND_LOG_MULTIPLE:
              return BACAPI_TREND_LOG_MULTIPLE_PROP_COUNT;
         case OBJECT_ACCESS_POINT:
              return BACAPI_ACCESS_POINT_PROP_COUNT;
         case OBJECT_ACCESS_ZONE:
              return BACAPI_ACCESS_ZONE_PROP_COUNT;
         case OBJECT_ACCESS_USER:
              return BACAPI_ACCESS_USER_PROP_COUNT;
         case OBJECT_ACCESS_RIGHTS:
              return BACAPI_ACCESS_RIGHTS_PROP_COUNT;
         case OBJECT_ACCESS_CREDENTIAL:
              return BACAPI_ACCESS_CREDENTIAL_PROP_COUNT;
         case OBJECT_CREDENTIAL_DATA_INPUT:
              return BACAPI_CREDENTIAL_DATA_INPUT_PROP_COUNT;
         case OBJECT_CHARACTERSTRING_VALUE:
              return BACAPI_CHARACTERSTRING_VALUE_PROP_COUNT;
         case OBJECT_DATETIME_VALUE:
              return BACAPI_DATETIME_VALUE_PROP_COUNT;
         case OBJECT_BITSTRING_VALUE:
              return BACAPI_BITSTRING_VALUE_PROP_COUNT;
         case OBJECT_OCTETSTRING_VALUE:
              return BACAPI_OCTETSTRING_VALUE_PROP_COUNT;
         case OBJECT_TIME_VALUE:
              return BACAPI_TIME_VALUE_PROP_COUNT;
         case OBJECT_LARGE_ANALOG_VALUE:
              return BACAPI_LARGE_ANALOG_VALUE_PROP_COUNT;
         case OBJECT_INTEGER_VALUE:
              return BACAPI_INTEGER_VALUE_PROP_COUNT;
         case OBJECT_POSITIVE_INTEGER_VALUE:
              return BACAPI_POSITIVE_INTEGER_VALUE_PROP_COUNT;
         case OBJECT_DATE_VALUE:
              return BACAPI_DATE_VALUE_PROP_COUNT;
         case OBJECT_DATETIME_PATTERN_VALUE:
              return BACAPI_DATETIME_PATTERN_VALUE_PROP_COUNT;
         case OBJECT_TIME_PATTERN_VALUE:
              return BACAPI_TIME_PATTERN_VALUE_PROP_COUNT;
         case OBJECT_DATE_PATTERN_VALUE:
              return BACAPI_DATE_PATTERN_VALUE_PROP_COUNT;
         case OBJECT_NETWORK_SECURITY:
              return BACAPI_NETWORK_SECURITY_PROP_COUNT;
         case OBJECT_GLOBAL_GROUP:
              return BACAPI_GLOBAL_GROUP_PROP_COUNT;
         case OBJECT_NOTIFICATION_FORWARDER:
              return BACAPI_NOTIFICATION_FORWARDER_PROP_COUNT;
         case OBJECT_ALERT_ENROLLMENT:
              return BACAPI_ALERT_ENROLLMENT_PROP_COUNT;
         case OBJECT_CHANNEL:
              return BACAPI_CHANNEL_PROP_COUNT;
         case OBJECT_LIGHTING_OUTPUT:
              return BACAPI_LIGHTING_OUTPUT_PROP_COUNT;
         case OBJECT_PRIMITIVE_TYPE_TEST:
              return BACAPI_PRIMITIVE_TYPE_TEST_PROP_COUNT;
         case OBJECT_SCRIPT_OBJECT:
              return BACAPI_SCRIPT_PROP_COUNT;
              
         case OBJECT_BINARY_LIGHTING_OUTPUT:
              return BACAPI_BINARY_LIGHTING_OUTPUT_PROP_COUNT;
         
         case OBJECT_NETWORK_PORT:
              return BACAPI_NETWORK_PORT_PROP_COUNT;
         
         case OBJECT_ELEVATOR_GROUP:
              return BACAPI_ELEVATOR_GROUP_PROP_COUNT;
         
         case OBJECT_ESCALATOR:
              return BACAPI_ESCALATOR_PROP_COUNT;
              
         case OBJECT_TIMER:
              return BACAPI_TIMER_PROP_COUNT;
         
         case OBJECT_LIFT:
              return BACAPI_LIFT_PROP_COUNT;
        
              


         default: return 0;
        }


}


/*
BACNET_CHARACTER_STRING *BACAPI_CreateCharacterString(const char *txt, int len, BACNET_TEXT_ENCODING encoding)
{
    // CHAR_SET_ANSI_X34 = 0,
    //CHAR_SET_MS_DBCS = 1,
    //CHAR_SET_JISC_6226 = 2,
    //CHAR_SET_UCS4 = 3,
    //CHAR_SET_UCS2 = 4,
    //CHAR_SET_ISO8859 = 5,
    //MAX_CHAR_SET_ENCODING = 6

 switch (encoding)
        {
         case CHAR_SET_UCS2:
         {
          BACNET_CHARACTER_STRING *tmp = new BACNET_CHARACTER_STRING;
          tmp->encoding = encoding;
          //for (
          //tmp->value16 =
          return tmp;
         }
         case CHAR_SET_JISC_6226:
         case CHAR_SET_ISO8859:
         case CHAR_SET_ANSI_X34:
         {
          BACNET_CHARACTER_STRING *tmp = new BACNET_CHARACTER_STRING;
          tmp->encoding = encoding;
          tmp->value8 = txt;
          return tmp;
         }
         default: break;
        }

 return NULL;
}
*/

void BACAPI_SetCharacterString(BACNET_CHARACTER_STRING *str, char *txt, int len)
{
 switch (str->encoding)
        {
         case CHAR_SET_UCS2:
         {
          wchar_t *wch_tmp = BACAPI_utf8_to_utf16(txt);
          str->value16 = wch_tmp;
          //  TODO: utf16_free
          break;
         }

         case CHAR_SET_JISC_6226:
         case CHAR_SET_ISO8859:
         case CHAR_SET_ANSI_X34:
         {
          str->value8 = txt;
          break;
         }
         default: break;
        }
}


const char *BACAPI_BACnetObjectName(uint16_t type)
{
 if (type == 130) return "BrightCore Link Object";

 if (type == 131) return "BrightCore Script Object";

 if (type > BACNET_OBJECT_COUNT)
    {
     return "Proprietary Object";
    }else if (type < 0)
             {
              return "Object name error";
             }
 return bacnetObjectTypeName[type].name;
}

const char *BACAPI_BACnetPropertyName(BACNET_PROPERTY_ID id)
{
 if (id > BACNET_PROPERTY_COUNT)
    {
     return "Proprietary Property";
    }else if (id < 0)
             {
              return "Property name error";
             }

 int t = (int)id;

 return bacnetPropertyName[id].name;
}


const char *BACAPI_BACnetPropertyNormalName(BACNET_PROPERTY_ID id)
{
 if (id > BACNET_PROPERTY_COUNT)
    {
     return "Proprietary Property";
    }else if (id < 0)
             {
              return "Property name error";
             }

 return bacnetPropertyName[id].name;
}

const char *BACAPI_BACnetErrorClassName(BACNET_ERROR_CLASS errclass)
{
 if (errclass > FIRST_PROPRIETARY_ERROR_CLASS && errclass < MAX_BACNET_ERROR_CLASS)
    {
     return "Proprietary Error Class";
    }else if (errclass > MAX_BACNET_ERROR_CLASS || errclass < 0)
             {
              return "Unknown Error Class";
             }


 return bacnetErrorClassName[errclass].name;
}


const char *BACAPI_BACnetErrorCodeName(BACNET_ERROR_CODE errcode)
{
 if (errcode > FIRST_PROPRIETARY_ERROR_CODE && errcode < LAST_PROPRIETARY_ERROR_CODE)
    {
     return "Proprietary Error Code";
    }else if (errcode > MAX_BACNET_ERROR_CODE || errcode < 0)
             {
              return "Unknown Error Code";
             }


 return bacnetErrorCodeName[errcode].name;
}

const char *BACAPI_BACnetConfirmedServiceName(BACNET_CONFIRMED_SERVICE service)
{
 if (service > MAX_BACNET_CONFIRMED_SERVICE)
    {
     return "Reserved by ASHRAE";
    }else if (service < 0)
             {
              return "Service name error";
             }

 return bacnetConfirmedServiceName[service].name;
}


const char *BACAPI_BACnetUnconfirmedServiceName(BACNET_UNCONFIRMED_SERVICE service)
{
 if (service > MAX_BACNET_UNCONFIRMED_SERVICE)
    {
     return "Reserved by ASHRAE";
    }else if (service < 0)
             {
              return "Service name error";
             }

 return bacnetUnconfirmedServiceName[service].name;
}


const char *BACAPI_BACnetServiceName(BACNET_PDU_TYPE type, uint8_t service)
{
 if (type == PDU_TYPE_CONFIRMED_SERVICE_REQUEST)
    {
     return BACAPI_BACnetConfirmedServiceName((BACNET_CONFIRMED_SERVICE)service);
    }else if (type == PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST)
             {
              return BACAPI_BACnetUnconfirmedServiceName((BACNET_UNCONFIRMED_SERVICE)service);
             }

 return "Unknown Service";
}


const char *BACAPI_BACnetSupportedServicesName(uint8_t service)
{
 if (service > MAX_BACNET_SUPPORTED_SERVICES)
    {
     return "Service doesn't exist";
    }else if (service < 0)
             {
              return "Service name error";
             }

 return bacnetSupportedServicesName[service].name;
}



//// NE KORISTITI
//const char *BACAPI_BACnetSupportedObjectsName(BACNET_PROPERTY_ID object)
//{
//
// if (object > MAX_BACNET_SUPPORTED_OBJECTS)
//    {
//     return "Proprietary Object";
//    }else if (object < 0)
//             {
//              return "Object name error";
//             }
//
// return bacnetSupportedObjectsName[object].name;
//}



const char *BACAPI_BACnetStatusFlagsName(uint8_t flag)
{
 if (flag > MAX_BACNET_STATUS_FLAGS || flag < 0)
    {
     return "Status Flag error";
    }

 return bacnetStatusFlagsName[flag].name;
}


const char *BACAPI_BACnetEventStateName(BACNET_EVENT_STATE event)
{
 if (event > EVENT_STATE_COUNT || event < 0)
    {
     return "Event State Error";
    }

 return bacnetEventStateName[event].name;
}




const char *BACAPI_BACnetNotifyTypeName(BACNET_NOTIFY_TYPE type)
{
 if (type > NOTIFY_TYPE_COUNT || type < 0)
    {
     return "Unknown Notify Type";
    }

 return bacnetNotifyTypeName[type].name;
}

const char *BACAPI_BACnetEventTypeName(BACNET_EVENT_TYPE type)
{
 if (type > EVENT_TYPE_COUNT || type < 0)
    {
     return "Event Type Error";
    }

 return bacnetEventTypeName[type].name;
}



const char *BACAPI_BACnetPDUTypeName(BACNET_PDU_TYPE type)
{
 switch(type)
       {
		case PDU_TYPE_CONFIRMED_SERVICE_REQUEST:
		 return bacnetPDUTypeName[0].name;
		case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
		 return bacnetPDUTypeName[1].name;
		case PDU_TYPE_SIMPLE_ACK :
		 return bacnetPDUTypeName[2].name;
		case PDU_TYPE_COMPLEX_ACK:
		 return bacnetPDUTypeName[3].name;
		case PDU_TYPE_SEGMENT_ACK:
		 return bacnetPDUTypeName[4].name;
		case PDU_TYPE_ERROR:
		 return bacnetPDUTypeName[5].name;
		case PDU_TYPE_REJECT:
		 return bacnetPDUTypeName[6].name;
		case PDU_TYPE_ABORT:
		 return bacnetPDUTypeName[7].name;
		default: return "PDU Type Error";
       }
 return "PDU Type Error";
}



const char *BACAPI_BACnetFileAccessTypeName(int at)
{
 if (at < 0 || at > 1)
    return "Unknown";

 return bacnetFileAccessTypeName[at].name;
}

const char *BACAPI_BACnetEOFName(bool eof)
{
 if (eof == 0) return "FALSE";
 else if (eof == 1) return "TRUE";
 else return "Out of Range";
}


const char *BACAPI_MACName(BACNET_ADDRESS *adr)
{
 char txt[10];

 sprintf(txt,"%02X%02X%02X",adr->adr[0],adr->adr[1],adr->adr[2]);

 //strtok
 string s = txt;
 for (int c=0;c<21742;c++)
     {
      string r = macAddressName[c].text;
      if (r == s)
         return macAddressName[c].name;
     }
 return "";
}




const char *BACAPI_BACnetMSTPFrameTypeName(MSTP_FRAME_TYPE ftype)
{
 switch (ftype)
        {
         case FRAME_TYPE_TOKEN:
          return "FRAME_TYPE_TOKEN";
         case FRAME_TYPE_POLL_FOR_MASTER:
          return "FRAME_TYPE_POLL_FOR_MASTER";
         case FRAME_TYPE_REPLY_POLL_FOR_MASTER:
          return "FRAME_TYPE_REPLY_POLL_FOR_MASTER";
         case FRAME_TYPE_TEST_REQUEST:
          return "FRAME_TYPE_TEST_REQUEST";
         case FRAME_TYPE_TEST_RESPONSE:
          return "FRAME_TYPE_TEST_RESPONSE";
         case FRAME_TYPE_DATA_EXPECTING_REPLY:
          return "FRAME_TYPE_DATA_EXPECTING_REPLY";
         case FRAME_TYPE_DATA_NOT_EXPECTING_REPLY:
          return "FRAME_TYPE_DATA_NOT_EXPECTING_REPLY";
         case FRAME_TYPE_REPLY_POSTPONED:
          return "FRAME_TYPE_REPLY_POSTPONED";

         default:
          return "Unknown";
        }
}



const char *BACAPI_BACnetMSTPMasterStateName(MSTP_MASTER_STATE state)
{
 switch (state)
        {
         case MSTP_MASTER_STATE_IDLE:   // 1
          return "MSTP_MASTER_STATE_IDLE";
          break;

         case MSTP_MASTER_STATE_POLL_FOR_MASTER:
          return "MSTP_MASTER_STATE_POLL_FOR_MASTER";
          break;

         case MSTP_MASTER_STATE_NO_TOKEN:
          return "MSTP_MASTER_STATE_NO_TOKEN";
          break;

         case MSTP_MASTER_STATE_USE_TOKEN:
          return "MSTP_MASTER_STATE_USE_TOKEN";
          break;

         case MSTP_MASTER_STATE_DONE_WITH_TOKEN:
          return "MSTP_MASTER_STATE_DONE_WITH_TOKEN";
          break;

         case MSTP_MASTER_STATE_WAIT_FOR_REPLY:
          return "MSTP_MASTER_STATE_WAIT_FOR_REPLY";
          break;

         case MSTP_MASTER_STATE_PASS_TOKEN:
          return "MSTP_MASTER_STATE_PASS_TOKEN";
          break;

         default: break;
          return "MSTP_MASTER_STATE_UNKNOWN";
        }
}


const char *BACAPI_BACnetRouterRejectMessageName(BACNET_ROUTER_REJECT_MESSAGE msg)
{
 if ((int)msg > 6 /*ROUTER_REJECT_ERROR_IN_SADR_DARD*/ || (int)msg < 0)
    {
     return "Unknow Error Message";
    }

 return routerRejectMessageName[msg].name;
}


const char *BACAPI_BACnetWeekdayName(uint8_t day)
{
 if (day > MAX_BACNET_WEEKDAYS || day < 0)
    {
     return "Week Day Name Error";
    }

 return bacnetWeekdayName[day].name;
}

const char *BACAPI_BACnetTransBitsName(BACNET_EVENT_TRANSITION_BITS bit)
{
 if (bit > MAX_BACNET_EVENT_TRANSITION_BITS || bit < 0)
    {
     return "Event Transition Bits Error";
    }

 return bacnetTransBitsName[bit].name;
}

const char *BACAPI_BACnetEngUnitsName(BACNET_ENGINEERING_UNITS unit)
{
 if (unit > MAX_BACNET_ENGINEERING_UNITS || unit < 0)
    {
     return "Engineering Units Error";
    }

 return bacnetEngUnitsName[unit].name;
}

const char *BACAPI_BACnetRejectCodeName(BACNET_REJECT_CODE code)
{
 if (code > MAX_BACNET_REJECT_CODE || code < 0)
    {
     return "Reject Code Error";
    }

 return bacnetRejectCodeName[code].name;
}

const char *BACAPI_BACnetAbortReasonName(BACNET_ABORT_REASON abort)
{
 if (abort > MAX_BACNET_ABORT_REASON || abort < 0)
    {
     return "Abort Reason Error";
    }

 return bacnetAbortReasonName[abort].name;
}

const char *BACAPI_BACnetSegmentationName(BACNET_SEGMENTATION seg)
{
 if (seg > MAX_BACNET_SEGMENTATION || seg < 0)
    {
     return "Segmentation Error";
    }

 return bacnetSegmentationName[seg].name;
}


const char *BACAPI_BACnetPriorityName(BACNET_MESSAGE_PRIORITY prio)
{
 if (prio > MAX_BACNET_PRIORITY || prio < 0)
    {
     return "Priority Error";
    }

 return bacnetPriorityName[prio].name;
}



//const char *BACAPI_BACnetErrorName(BACNET_ERRORn err)
//{
//
//}

/*
BACAPI_INVOKE_ID *BACAPI_ReleaseInvokeId(BACNET_DEVICE *dev,BACNET_OBJECT *obj, BACNET_OBJECT_PROPERTY *prop, bool monitor)
{
 if (globalInvokeId > 255)
    {
     return NULL;  // svi slobodni invokeovi se koriste
    }

 BACAPI_INVOKE_ID *newinvoke = new BACAPI_INVOKE_ID;
 newinvoke->invokeId = globalInvokeId;
 newinvoke->dev = dev;
 newinvoke->obj = obj;
 newinvoke->prop = prop;
 newinvoke->monitor = monitor;

 globalInvokeId++;
 return newinvoke;
}


void BACAPI_ReleaseInvokeId(BACAPI_INVOKE_ID *invoke)
{
 globalInvokeId--;
 //gLog->AppendLog("BACAPI_ReleaseInvokeId: globalInvokeId = %d\n",globalInvokeId);

 if (globalInvokeId < 0)   // za svaki slucaj
    {
     globalInvokeId = 0;
    }

 delete invoke;
 invoke = NULL;
}*/





const char *BACAPI_BACnetVendorName(int vendorid)
{
 if (vendorid > MAX_BACNET_VENDOR_ID || vendorid < 0)
    {
     return "Unknown";
    }

 return bacapiVendorName[vendorid].name;
}


const char *BACAPI_BACnetAccumulatorStatusName(BACNET_ACCUMULATOR_STATUS status)
{
 if ((int)status >= ACCU_STATUS_MAX || (int)status < 0)
    {
     return "Unknown Status";
    }

 return bacnetAccumulatorStatus[(int)status].name;

}

const char *BACAPI_BACnetFaultTypeName(BACNET_FAULT_TYPE type)
{
 if ((int)type >= FAULT_TYPE_COUNT || (int)type < 0)
    {
     return "Unknown Fault Type";
    }
 return bacnetFaultTypeName[(int)type].name;
}


const char *BACAPI_BACnetLifeSafetyStateName(BACNET_LIFE_SAFETY_STATE state)
{
 if ((int)state >= LIFE_SAFETY_STATE_COUNT || (int)state < 0)
    {
     return "Unknown State";
    }
 return bacnetLifeSafetyStateName[(int)state].name;
}



const char *BACAPI_BACnetLifeSafetyModeName(BACNET_LIFE_SAFETY_MODE mode)
{
 if ((int)mode >= LIFE_SAFETY_MODE_MAX || (int)mode < 0)
    {
     return "Unknown Mode";
    }
 return bacnetLifeSafetyModeName[(int)mode].name;
}



const char *BACAPI_BACnetApplicationTagName(BACNET_APPLICATION_TAG tag)
{
 if ((int)tag >= MAX_BACNET_APPLICATION_TAG || (int)tag < 0)
    {
     return "Unknown";
    }
 return bacnetApplicationTagName[(int)tag].name;
}


const char *BACAPI_BACnetReinitStateName(BACNET_REINIT_STATE state)
{
 if ((int)state >= MAX_REINIT_STATE || (int)state < 0)
    {
     return "Unknown";
    }
  return bacnetReinitStateName[(int)state].name;
}



const char *BACAPI_BACnetReliabilityName(BACNET_RELIABILITY rel)
{
 if ((int)rel >= RELIABILITY_COUNT || (int)rel < 0)
    {
     return "Unknown Reliability State";
    }
  return bacnetReliabilityName[(int)rel].name; 
}



const char *BACAPI_BACnetBinaryLightingPVName(BACNET_BINARY_LIGHTING_PV pv)
{
 if ((int)pv >= BINARY_LIHGTING_COUNT || (int)pv < 0)
    {
     return "Unknown Binary Lighting Present Value";
    }
  return binaryLightingPVName[(int)pv].name; 
}


const char *BACAPI_BACnetLiftGroupModeName(BACNET_LIFT_GROUP_MODE mode)
{
 if ((int)mode >= GROUP_MODE_COUNT || (int)mode < 0)
    {
     return "Unknown Lift Group Mode";
    }
  return liftGroupModeName[(int)mode].name; 
}



// NOTE: proveriti kako se vraca str na Win i Linx
const char *BACAPI_BACnetLiftDirectionName(BACNET_LIFT_CAR_DIRECTION dir)
{
 if ((int)dir >= DIRECTION_COUNT || (int)dir < 0)
    {
     char str[256];
     sprintf(str,"Proprietary Lift Direction (%d)",(int)dir);
     return str;
    }
  return liftCarDirectionName[(int)dir].name;  
}



const char *BACAPI_BACnetNNQualityName(BACNET_NN_QUALITY q)
{
 if ((int)q >= QUALITY_COUNT || (int)q < 0)
    {
     char str[256];
     sprintf(str,"Unknown Network Number Quality (%d)",(int)q);
     return str;
    }
  return bacnetNNQualityName[(int)q].name;   
}




const char *BACAPI_BACnetAccessAuthFactorDisableName(BACNET_ACCESS_AUTH_FACTOR_DISABLE fact)
{
 int f = (int)fact;
 if ((int)fact >= 6 && (int)fact <=  63)
    {
     char str[256];
     sprintf(str,"Reserved for us by ASHRAE: (%d)",(int)fact);
     return str;
    }
 
  if ((int)fact > 63)
     {
      char str[256];
      sprintf(str,"Proprietary: (%d)",(int)fact);
      return str;
     }
    
  return bacnetAccessAuthFactorDisable[(int)fact].name; 
}



const char *BACAPI_BACnetAuthenticationFactorTypeName(BACNET_AUTHENTICATION_FACTOR_TYPE type)
{
if ((int)type >= TYPE_COUNT_MAX || (int)type < 0)
    {
     char str[256];
     sprintf(str,"Unknown Authentication Factor Type (%d)",(int)type);
     return str;
    }
  return bacnetAuthenticationFactorType[(int)type].name;  
}


const char *BACAPI_BACnetAuthorizationExemptionsName(BACNET_AUTHORIZATION_EXEMPTIONS e)
{
 int ex = (int)e;
 if ((int)e >= 7 && (int)e <=  63)
    {
     char str[256];
     sprintf(str,"Reserved for us by ASHRAE: (%d)",(int)e);
     return str;
    }
 
  if ((int)e > 63)
     {
      char str[256];
      sprintf(str,"Proprietary: (%d)",(int)e);
      return str;
     }
    
  return bacnetAuthorizationExemptions[(int)e].name; 
}


const char *BACAPI_BACnetAccessCredentialDisableReasonName(BACNET_ACCESS_CREDENTIAL_DISABLE_REASON n)
{
 if ((int)n >= 10 && (int)n <=  63)
    {
     char str[256];
     sprintf(str,"Reserved for us by ASHRAE: (%d)",(int)n);
     return str;
    }
 
  if ((int)n > 65535)
     {
      char str[256];
      sprintf(str,"Proprietary: (%d)",(int)n);
      return str;
     }
    
  return bacnetAccessCredentialDisableReason[(int)n].name; 
}


const char *BACAPI_BACnetDCCStateName(BACNET_DCC_STATE state)
{
 if ((int)state >= MAX_DCC || (int)state < 0)
    {
     return "Unknown DCC State";
    }
  return bacnetDCCStateName[(int)state].name;
}


const char *BACAPI_BACnetSeverity(BACAPI_SEVERITY state)
{
 if ((int)state >= BACAPI_SEVERITY_MAX || (int)state < 0)
    {
     return "Unknown";
    }
  return bacnetSeverityName[(int)state].name;
}



const char *BACAPI_BACnetDataLinkName(BACNET_DATALINK link)
{
 if ((int)link >= 4 || (int)link < 0)
    {
     return "Unknown";
    }
  return dataLinkName[(int)link].name;
}




void BACAPI_DeletePropertyValues(BACNET_OBJECT_PROPERTY *prop)
{
 int count = (size_t)prop->vValueList.size();
 for (int c=0;c<count;c++)
     {
      BACNET_APPLICATION_DATA_VALUE *value = prop->vValueList[c];
      if (value)
         {
          BACAPI_DeletePropertyValue(value);
         }
     }
 prop->vValueList.clear();

}



// kopirati ce sve objekte i propertije.
void BACAPI_CopyBACnetDevice(BACNET_DEVICE *to, BACNET_DEVICE *from)
{
 memcpy(to,from,sizeof(BACNET_DEVICE));

 if (from->npduData)
    {
     to->npduData = new BACNET_NPDU_DATA;
     memcpy(to->npduData,from->npduData,sizeof(BACNET_NPDU_DATA));

     to->npduData->sadr = BACAPI_CreateBACnetAddress(from->npduData->sadr);
     to->npduData->dadr = BACAPI_CreateBACnetAddress(from->npduData->dadr);
     to->npduData->remote = BACAPI_CreateBACnetAddress(from->npduData->remote);
    }

}


void BACAPI_CopyBACnetObject(BACNET_OBJECT *to, BACNET_OBJECT *from)
{
 to->name = from->name;
 to->oid.instance = from->oid.instance;
 to->oid.type = from->oid.type;
}



void BACAPI_CopyBACnetProperty(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from)
{
 to->index = from->index;
 to->priority = from->priority;
 to->property = from->property;
 to->readOnly = from->readOnly;

 if (BACAPI_IsComplexProperty(from->property))
     BACAPI_CopyPropertyComplexValue(to,from);
     else BACAPI_CopyPropertyNormalValue(to,from);

}


void BACAPI_CopyPropertyNormalValue(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from)
{
 int count = 0;

 BACAPI_DeletePropertyValues(to);


 count = from->vValueList.size();
 for (int c=0;c<count;c++)
     {
      BACNET_APPLICATION_DATA_VALUE *from_value = from->vValueList[c];
      BACNET_APPLICATION_DATA_VALUE *to_value = BACAPI_CreateApplicationDataValue((BACNET_APPLICATION_TAG)from_value->application_tag);

      to->property = from->property;
      to->index = from->index;
      to->readOnly = from->readOnly;

      // TODO: isto za value16
      if (from_value->application_tag == BACNET_APPLICATION_TAG_CSTRING)
         {
          to_value->type.character_String.value8 = from_value->type.character_String.value8;
          to_value->type.character_String.value16 = from_value->type.character_String.value16;
         }else memcpy(to_value,from_value,sizeof(BACNET_APPLICATION_DATA_VALUE));

/*
	  switch (from_value->application_tag)
			 {
			  case BACNET_APPLICATION_TAG_BOOL:
			  {
			   to_value->type.Boolean = from_value->type.Boolean;
			   break;
			  }
			  case BACNET_APPLICATION_TAG_UINT:
			  {
			   to_value->type.Unsigned_Int = from_value->type.Unsigned_Int;
			   break;
			  }
			  case BACNET_APPLICATION_TAG_SINT:
			  {
			   to_value->type.Signed_Int = from_value->type.Signed_Int;
			   break;
			  }
#ifdef BACAPI_DOUBLE
			  case BACNET_APPLICATION_TAG_DOUBLE:
			  {
			   to_value->type.Double = from_value->type.Double;
			   break;
			  }
#else
			  case BACNET_APPLICATION_TAG_REAL:
			  {
			   to_value->type.Real = from_value->type.Real;
			   break;
			  }
#endif
			  case BACNET_APPLICATION_TAG_OSTRING:
			  {
			   to_value->type.Octet_String = new BACNET_OCTET_STRING;
			   BACAPI_CopyBACnetOctetString(to_value->type.Octet_String,from_value->type.Octet_String);

			   break;
			  }
			  case BACNET_APPLICATION_TAG_CSTRING:
			  {
			   to_value->type.character_String = new BACNET_CHARACTER_STRING;
			   to_value->type.character_String.value8 = from_value->type.character_String.value8;
			   to_value->type.character_String.value16 = from_value->type.character_String.value16;
			   to_value->type.character_String.encoding = from_value->type.character_String.encoding;
			   //memcpy(to_value->type.character_String, from_value->type.character_String,sizeof(BACNET_CHARACTER_STRING));
			   break;
			  }
			  case BACNET_APPLICATION_TAG_ENUM:
			  {
			   to_value->type.Enumerated = from_value->type.Enumerated;
			   break;
			  }
			  case BACNET_APPLICATION_TAG_BSTRING:
			  {
			   to_value->type.Bit_String = from_value->type.Bit_String;
			   break;
			  }
			  case BACNET_APPLICATION_TAG_DATE:
			  {
			   to_value->type.Date.day = from_value->type.Date.day;
			   to_value->type.Date.month = from_value->type.Date.month;
			   to_value->type.Date.wday = from_value->type.Date.wday;
			   to_value->type.Date.year = from_value->type.Date.year;
			   break;
			  }

			  case BACNET_APPLICATION_TAG_TIME:
			  {
			   to_value->type.Time.hour = from_value->type.Time.hour;
			   to_value->type.Time.minute = from_value->type.Time.minute;
			   to_value->type.Time.sec = from_value->type.Time.sec;
			   break;
			  }
			  case BACNET_APPLICATION_TAG_OID:
			  {
			   to_value->type.Object_Id.type = from_value->type.Object_Id.type;
			   to_value->type.Object_Id.instance = from_value->type.Object_Id.instance;
			   break;
			  }
			 }
*/
	  to->vValueList.push_back(to_value);
     }

}


void BACAPI_CopyPropertyValue(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from)
{

 if (BACAPI_IsComplexProperty(to->property))
    {
     // TODO:
     BACAPI_CopyPropertyComplexValue(to,from);
    }else {
           BACAPI_CopyPropertyNormalValue(to,from);
          }
}



BACNET_OBJECT_PROPERTY *BACAPI_GetObjectProperty(BACNET_PROPERTY_ID propId, BACNET_OBJECT *obj)
{
 if (!obj) return NULL;

 size_t cnt = obj->vProperties.size();

 for (int c=0;c<cnt;c++)
     {
      BACNET_OBJECT_PROPERTY *prop = obj->vProperties[c];
      //gLog->AppendLog("BACAPI_GetObjectProperty: check propId %d == prop->property %d",propId,prop->property);
      if (prop->property == propId)
         {
          return prop;
         }
     }
 return NULL;
}



BACNET_OBJECT_PROPERTY *BACAPI_GetObjectProperty(int num, BACNET_OBJECT *obj)
{
 size_t cnt = obj->vProperties.size();
 if (cnt == 0) return NULL;

 if (num >= cnt || num < 0) return NULL;

 return obj->vProperties[num];
}



BACAPI_Event *BACAPI_CreateEvent(void)
{
 BACAPI_Event *event = NULL;
 try {
      event = new BACAPI_Event;
     }catch (std::bad_alloc& ba)
            {
             gLog->AppendLog("BACAPI_CreateEvent: bad_alloc");
             event = NULL;
             return NULL;
            }

 memset(event,0,sizeof(BACAPI_Event));


 return event;
}

// privatna, zove se samo iz BACAPI_ReleaseEvent
void BACAPI_DeleteEventMessage(BACAPI_MESSAGE *msg)
{
 switch(msg->type)
       {
        case BACAPI_MESSAGE_RECEIVED_I_AM:
         BACAPI_DeleteDevice(msg->dev);
         break;

        case BACAPI_ERROR_MALFORMED_PACKET:
         delete msg->adr;
         break;

        default:
         break;
       }

 delete msg;
}

void BACAPI_ReleaseEvent(BACAPI_Event *event)
{
 switch (event->type)
        {
         case BACAPI_EVENT_WHO_IS:
         delete event->address;
         event->address = NULL;
         break;

         case BACAPI_EVENT_I_AM:
         BACAPI_DeleteDevice(event->device);
         event->device = NULL;
         break;

         case BACAPI_EVENT_MESSAGE:
         BACAPI_DeleteEventMessage(event->message);
         event->message = NULL;
         break;

         case BACAPI_EVENT_RP_REQ:
         delete event->address;
         delete event->objPropRef;
         event->objPropRef = NULL;
         break;

         case BACAPI_EVENT_RP_ACK:
         BACAPI_DeleteObjectProperty(event->property);
         event->property = NULL;
         break;

         case BACAPI_EVENT_RP_ACK_ARRAY_ELEMENT:
         delete event->property;
         event->property = NULL;
         break;

         case BACAPI_EVENT_RPM_REQ:
         delete event->objPropRef;
         delete event->address;
         event->objPropRef = NULL;
         event->address = NULL;
         break;

         case BACAPI_EVENT_RPM_ACK:
         delete event->object;
         event->object = NULL;
         break;

         case BACAPI_EVENT_SIMPLE_ACK:
         delete event->simpleAck;
         event->simpleAck = NULL;
         break;


         case BACAPI_EVENT_ABORT:
         // nije stvoren
		 break;

         case BACAPI_EVENT_ALARM_SUMMARY:
         delete event->alarm;
         event->alarm = NULL;
		 break;

         case BACAPI_EVENT_UNCONFIRMED_COV_NOTIFICATION:
         case BACAPI_EVENT_CONFIRMED_COV_NOTIFICATION:
		 delete event->cov;  //  TODO: delete cov->vValueList
		 event->cov = NULL;
		 break;


		 case BACAPI_EVENT_SEGMENT_ACK:
		 delete event->segmentAck;
		 event->segmentAck = NULL;
		 break;

		 case BACAPI_EVENT_WP_REQ:
         BACAPI_DeleteObjectProperty(event->property);
         BACAPI_DeleteObject(event->object);
         delete event->address;
         break;

         case BACAPI_EVENT_CREATE_OBJECT:

         delete event->address;
         break;

         case BACAPI_EVENT_DELETE_OBJECT:
         delete event->address;
         break;

         case BACAPI_EVENT_ATOMIC_READ_FILE_ACK:
         delete event->file_data;
         break;

         case BACAPI_EVENT_ATOMIC_WRITE_FILE_ACK:
         delete event->file_data;
         break;

         case BACAPI_EVENT_REJECT:
         delete event->address;
         event->address = NULL;
         break;

         case BACAPI_EVENT_ROUTE_PACKET:
          BACAPI_ReleasePacket(event->packet);
          event->packet = NULL;
          break;

         case BACAPI_EVENT_ERROR:
          delete event->address;
          break;




         default:
          printf("");
          break;

        }


}


string BACAPI_GetShortStringValue(BACNET_SHORT_APPLICATION_DATA_VALUE *value)
{
   if (value)
       {
        switch (value->tag)
               {
            case BACNET_APPLICATION_TAG_NULL:
                return "NULL";
                break;
            case BACNET_APPLICATION_TAG_BOOL:
                if (value->type.Boolean) return "1";
                else return "0";
                break;
            case BACNET_APPLICATION_TAG_UINT:
               {
                char msg[256];
                sprintf(msg,"%d",value->type.Unsigned_Int);
                return msg;
                break;
               }
            case BACNET_APPLICATION_TAG_SINT:
               {
                char msg[256];
                sprintf(msg,"%d",value->type.Signed_Int);
                return msg;
                break;
               }
            case BACNET_APPLICATION_TAG_REAL:
               {
                char msg[256];
                sprintf(msg,"%f",value->type.Real);
                return msg;
                break;
               }
            case BACNET_APPLICATION_TAG_DOUBLE:
                char msg[256];
                sprintf(msg,"%f",value->type.Double);
                return msg;
                break;

            case BACNET_APPLICATION_TAG_CSTRING:
				 {
				  switch (value->type.character_string.encoding)
						 {
						  case CHAR_SET_JISC_6226:
						  case CHAR_SET_ISO8859:
						  case CHAR_SET_ANSI_X34:   // normalni char set
						   return value->type.character_string.value8;

						  case CHAR_SET_UCS2:
						  {
						   char *ret = BACAPI_utf16_to_utf8(value->type.character_string.value16.c_str(),value->type.character_string.value16.size());
						   if (ret == NULL)
							   return "";
						   string tmp = ret;
						   BACAPI_utf8_free(ret);
						   return tmp;
						  }

						   default:
						   return "Character set not supported";
						 }
				  break;
                }

            case BACNET_APPLICATION_TAG_ENUM:
                {
                 char msg[256];
                 sprintf(msg,"%d",value->type.Enumerated);
                 return msg;
                 break;
                }


            default:break;
        }
    }
 return "Not a Primitive Value.";
}



// TODO: svi  case koji vracaju char moraju vracati string
string BACAPI_GetStringValue( BACNET_APPLICATION_DATA_VALUE *value)
{
    if (value)
       {
        switch (value->application_tag)
               {
            case BACNET_APPLICATION_TAG_NULL:
                return "NULL";
                break;
            case BACNET_APPLICATION_TAG_BOOL:
                if (value->type.Boolean) return "TRUE";
                else return "FALSE";
                break;
            case BACNET_APPLICATION_TAG_UINT:
               {
                char msg[256];
                sprintf(msg,"%d",value->type.Unsigned_Int);
                return msg;
                break;
               }
            case BACNET_APPLICATION_TAG_SINT:
               {
                char msg[256];
                sprintf(msg,"%d",value->type.Signed_Int);
                return msg;
                break;
               }
            case BACNET_APPLICATION_TAG_REAL:
               {
                char msg[256];
                sprintf(msg,"%f",value->type.Real);
                return msg;
                break;
               }
#if defined (BACAPP_DOUBLE)
            case BACNET_APPLICATION_TAG_DOUBLE:
                char msg[256];
                sprintf(msg,"%f",value->type.Double);
                return msg;
                break;
#endif
            case BACNET_APPLICATION_TAG_OSTRING:
               {
                int len = value->type.Octet_String.vValue.size();
                //char *stream = new char[len*2];
                string s;
                for (int i = 0; i < len; i++)
                    {
                     char byte[8];
                     sprintf(byte, "%02X", value->type.Octet_String.vValue[i]);
                     s += string(byte);
                    }
                //string tmp = stream;
                //delete []stream;
                return s;
               }
            break;
            case BACNET_APPLICATION_TAG_CSTRING:
                {
                 //printf("BACAPI_GetStringValue: BACNET_APPLICATION_TAG_CSTRING: str %s\n",value->type.character_String.value8.c_str());
                 switch (value->type.character_String.encoding)
                        {
                         case CHAR_SET_ANSI_X34:  // normalni char set
                         case CHAR_SET_ISO8859:
                         case CHAR_SET_JISC_6226:
                          //printf("%s",value->type.character_String.value8.c_str());
                          return value->type.character_String.value8;

                         case CHAR_SET_UCS2:
                         {
                          string str(value->type.character_String.value16.begin(), value->type.character_String.value16.end());
                          return str.c_str();
						 }

                          default:
                          return "Character set not supported";
                        }
                 break;
                }

            case BACNET_APPLICATION_TAG_ENUM:
                {
                 char msg[256];
                 sprintf(msg,"%d",value->type.Enumerated);
                 return msg;
                 break;
                }

            case BACNET_APPLICATION_TAG_BSTRING:
                 {
				  int len = gDec->bitstring_bits_used(&value->type.Bit_String);

				  if (len == 0)
				      return "Empty";

                  char strbit[3];
                  string s;

				  for (int i = 0, pos = 0; i < len; i++,pos++)
					  {

                       sprintf(strbit,"%d,",gDec->bitstring_bit(&value->type.Bit_String,(uint8_t) i));
                       s += string(strbit);
                      }

                  return s;
                  break;
                 }

            case BACNET_APPLICATION_TAG_DATE:
                {
                char msg[256];

                sprintf(msg, "%d, %d ",value->type.Date.wday,value->type.Date.month);


                if (value->type.Date.day == 255)
                   {
                    sprintf(msg, "(unspecified), ");
                   } else {
                           sprintf(msg, "%u, ", (unsigned) value->type.Date.day);
                          }

                if (value->type.Date.year == 255)
                   {
                    sprintf(msg, "(unspecified), ");
                   } else {
                           sprintf(msg, "%u", (unsigned) value->type.Date.year);
                          }
                return msg;
                break;
               }

            case BACNET_APPLICATION_TAG_TIME:
            {
             char stream[128];
                if (value->type.Time.hour == 255)
                   {
                    sprintf(stream, "**:");
                   } else {
                           sprintf(stream, "%02u:", (unsigned) value->type.Time.hour);
                          }

                if (value->type.Time.minute== 255)
                   {
                    sprintf(stream, "**:");
                   } else {
						   sprintf(stream, "%02u:", (unsigned) value->type.Time.minute);
                          }

                if (value->type.Time.sec == 255)
                   {
                    sprintf(stream, "**.");
                   } else {
                           sprintf(stream, "%02u.", (unsigned) value->type.Time.sec);
                          }

                if (value->type.Time.hundredths == 255)
                   {
                    sprintf(stream, "**");
                   } else {
                           sprintf(stream, "%02u",(unsigned) value->type.Time.hundredths);
                          }
                break;
            }

            case BACNET_APPLICATION_TAG_OID:
                 {
                  char stream[256];
                  if (value->type.Object_Id.type < BACNET_OBJECT_COUNT)
                     {


                      sprintf(stream, "%s, %u", BACAPI_BACnetObjectName(value->type.Object_Id.type), value->type.Object_Id.instance);
                     } else if (value->type.Object_Id.type < 128)
                               {
                                sprintf(stream, "reserved %u, %u",value->type.Object_Id.type,  value->type.Object_Id.instance);
                               } else {
                                       sprintf(stream, "proprietary %u, %u",value->type.Object_Id.type,value->type.Object_Id.instance);
                                      }

                 return stream;
                 break;
                }


            default:

                break;
        }
    }

    return "";
}




string BACAPI_GetStringValue( BACNET_OBJECT_PROPERTY *prop)
{
 if (prop)
    {
     if (prop->vValueList.size() > 0)
        {
         BACNET_APPLICATION_DATA_VALUE *value = prop->vValueList[0];
         if (value)
            {
             switch (value->application_tag)
               {
            case BACNET_APPLICATION_TAG_NULL:
                return "NULL";
                break;
            case BACNET_APPLICATION_TAG_BOOL:
                if (value->type.Boolean) return "TRUE";
                else return "FALSE";
                break;
            case BACNET_APPLICATION_TAG_UINT:
               {
                char msg[256];
                sprintf(msg,"%d",value->type.Unsigned_Int);
                return msg;
                break;
               }
            case BACNET_APPLICATION_TAG_SINT:
               {
                char msg[256];
                sprintf(msg,"%d",value->type.Signed_Int);
                return msg;
                break;
               }


//#ifdef BACAPI_DOUBLE
            case BACNET_APPLICATION_TAG_DOUBLE:
                char msg[256];
                sprintf(msg,"%f",value->type.Double);
                return msg;
                break;
//#else
            case BACNET_APPLICATION_TAG_REAL:
               {
                char msg[256];
                sprintf(msg,"%f",value->type.Real);
                return msg;
                break;
               }
//#endif
            case BACNET_APPLICATION_TAG_OSTRING:
               {
                int len = value->type.Octet_String.vValue.size();
                char *stream = new char[len*2];
                for (int i = 0; i < len; i++)
                    {
                     sprintf(stream, "%02X", value->type.Octet_String.vValue[i]);
                    }
                string tmp = stream;
                delete []stream;
                return tmp;
               }
            break;
            case BACNET_APPLICATION_TAG_CSTRING:
                {
                 switch (value->type.character_String.encoding)
                        {
                         case CHAR_SET_ANSI_X34:  // normalni char set
                         case CHAR_SET_ISO8859:
                         case CHAR_SET_JISC_6226:
                          return value->type.character_String.value8.c_str();

                         case CHAR_SET_UCS2:
                         {
                          string str(value->type.character_String.value16.begin(), value->type.character_String.value16.end());
                          return str.c_str();
						 }

                          default:
                          return "Character set not supported";
                        }
                 break;
                }

            case BACNET_APPLICATION_TAG_ENUM:
                {
                 char msg[256];
                 sprintf(msg,"%d",value->type.Enumerated);
                 return msg;
                 break;
                }

            case BACNET_APPLICATION_TAG_BSTRING:
                 {
				  int len = gDec->bitstring_bits_used(&value->type.Bit_String);

				  if (len == 0)
				      return "Empty";

                  char strbit[3];
                  string s;

				  for (int i = 0, pos = 0; i < len; i++,pos++)
					  {

                       sprintf(strbit,"%d,",gDec->bitstring_bit(&value->type.Bit_String,(uint8_t) i));
                       s += string(strbit);
                      }

                  return s;
                  break;
                 }

            case BACNET_APPLICATION_TAG_DATE:
                {
                char msg[256];

                sprintf(msg, "%d, %d ",value->type.Date.wday,value->type.Date.month);


                if (value->type.Date.day == 255)
                   {
                    sprintf(msg, "(unspecified), ");
                   } else {
                           sprintf(msg, "%u, ", (unsigned) value->type.Date.day);
                          }

                if (value->type.Date.year == 255)
                   {
                    sprintf(msg, "(unspecified), ");
                   } else {
                           sprintf(msg, "%u", (unsigned) value->type.Date.year);
                          }
                return msg;
                break;
               }

            case BACNET_APPLICATION_TAG_TIME:
            {
             char stream[128];
                if (value->type.Time.hour == 255)
                   {
                    sprintf(stream, "**:");
                   } else {
                           sprintf(stream, "%02u:", (unsigned) value->type.Time.hour);
                          }

                if (value->type.Time.minute== 255)
                   {
                    sprintf(stream, "**:");
                   } else {
						   sprintf(stream, "%02u:", (unsigned) value->type.Time.minute);
                          }

                if (value->type.Time.sec == 255)
                   {
                    sprintf(stream, "**.");
                   } else {
                           sprintf(stream, "%02u.", (unsigned) value->type.Time.sec);
                          }

                if (value->type.Time.hundredths == 255)
                   {
                    sprintf(stream, "**");
                   } else {
                           sprintf(stream, "%02u",(unsigned) value->type.Time.hundredths);
                          }
                break;
            }

            case BACNET_APPLICATION_TAG_OID:
                 {
                  char stream[256];
                  if (value->type.Object_Id.type < BACNET_OBJECT_COUNT)
                     {


                      sprintf(stream, "%s, %u", BACAPI_BACnetObjectName(value->type.Object_Id.type), value->type.Object_Id.instance);
                     } else if (value->type.Object_Id.type < 128)
                               {
                                sprintf(stream, "reserved %u, %u",value->type.Object_Id.type,  value->type.Object_Id.instance);
                               } else {
                                       sprintf(stream, "proprietary %u, %u",value->type.Object_Id.type,value->type.Object_Id.instance);
                                      }

                 return stream;
                 break;
                }


            default:
                 char str_unkown[128];
                 sprintf(str_unkown,"Unknown Value: AppTag: %d",value->application_tag);
                 return str_unkown;
                break;
        }
            }
        }
    }

 return "Unknown Value";
}


// Stvori novi property, postavi prop_id
// Nuliraj, i postavi defaultne vrijednosti
// vrati property
BACNET_OBJECT_PROPERTY *BACAPI_CreateObjectProperty(BACNET_OBJECT_ID oid, BACNET_PROPERTY_ID propid)
{
 BACNET_OBJECT_PROPERTY *prop = new BACNET_OBJECT_PROPERTY;
 memset(prop,0,sizeof(BACNET_OBJECT_PROPERTY));  // nuliraj

 prop->property = propid;


 // -- stvara se samo prop ne i value

 // ako je komplexsni property
 if (BACAPI_IsComplexProperty(prop->property))
    {
     // stvori complex value
     BACAPI_DefaultComplexValue(prop);
    }else {
           // NOTE: ne inicijalizira se ovdje, popraviti ovo
           BACAPI_DefaultNormalValue(oid, prop);
          }

 return prop;
}



// Stvori novi property,
// Nuliraj,
// Vrati prop;
BACNET_OBJECT_PROPERTY *BACAPI_CreateObjectProperty(void)
{
 BACNET_OBJECT_PROPERTY *prop = new BACNET_OBJECT_PROPERTY;
 memset(prop,0,sizeof(BACNET_OBJECT_PROPERTY));  // nuliraj
 return prop;
}


// Stvori novu defaultnu vrijdnost u odnosu na OID
// postavi app_tag za vrijdnost
// ubaci u listu vrijednosti zqa taj property
void BACAPI_DefaultNormalValue(BACNET_OBJECT_ID oid, BACNET_OBJECT_PROPERTY *prop)
{
 BACNET_APPLICATION_TAG tag = BACAPI_GetApplicationTag((BACNET_OBJECT_TYPE)oid.type,prop->property);
 BACNET_APPLICATION_DATA_VALUE *value = BACAPI_CreateApplicationDataValue(tag);
 prop->vValueList.push_back(value);
}





void BACAPI_CopyPropertyComplexValue(BACNET_OBJECT_PROPERTY *to, BACNET_OBJECT_PROPERTY *from)
{

 switch (from->property)
        {
		 case PROP_ACCESS_EVENT_CREDENTIAL:
		 case PROP_ACCESS_DOORS:
		 case PROP_ZONE_FROM:
		 case PROP_ZONE_TO:
		 case PROP_CREDENTIALS_IN_ZONE:
		 case PROP_LAST_CREDENTIAL_ADDED:
		 case PROP_LAST_CREDENTIAL_REMOVED:
		 case PROP_MEMBERS:
		 case PROP_CREDENTIALS:
		 case PROP_BELONGS_TO:
		 case PROP_LAST_ACCESS_POINT:
		 case PROP_LOG_DEVICE_OBJECT_PROPERTY:
		 case PROP_LINK_REFERENCE_IN:
		 case PROP_LINK_REFERENCE_OUT:
			  {
               //BACAPI_DeleteDeviceObjectPropRef(to->complexValue.deviceObjectPropReference);
               //to->complexValue.deviceObjectPropReference = from->complexValue.deviceObjectPropReference;
			   break;
              }


         case PROP_SUBORDINATE_LIST:
         case PROP_DOOR_MEMBERS:
         case PROP_ZONE_MEMBERS:
         case PROP_MEMBER_OF:
         case PROP_ENTRY_POINTS:
         case PROP_EXIT_POINTS:
			  {
			   BACAPI_DeleteDeviceObjectRef(to); // isprazni sve

			   int count = from->complexValue.vDeviceObjectReference.size();
			   for (int c=0;c<count;c++)
			       {
			        BACNET_DEVICE_OBJECT_REFERENCE *ref = from->complexValue.vDeviceObjectReference[c];
			        to->complexValue.vDeviceObjectReference.push_back(ref);  // spremam
			       }
               from->complexValue.vDeviceObjectReference.clear();
			   break;
			  }



         case PROP_SETPOINT_REFERENCE:
         case PROP_MANIPULATED_VARIABLE_REFERENCE:
         case PROP_CONTROLLED_VARIABLE_REFERENCE:
         case PROP_INPUT_REFERENCE:
         case PROP_OBJECT_PROPERTY_REFERENCE:
         case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:
         case PROP_EVENT_ALGORITHM_INHIBIT_REF:
			  {
			   BACAPI_DeleteObjectPropertyRef(to); // isprazni sve kod nas
			   int count = from->complexValue.vObjectPropertyReference.size();
			   for (int c=0;c<count;c++)
			       {
			        BACNET_OBJECT_PROPERTY_REFERENCE *new_ref = new BACNET_OBJECT_PROPERTY_REFERENCE;
			        BACNET_OBJECT_PROPERTY_REFERENCE *ptr_ref = from->complexValue.vObjectPropertyReference[c];
			        memcpy(new_ref,ptr_ref,sizeof(BACNET_OBJECT_PROPERTY_REFERENCE));
			        gLog->AppendLog("BACAPI_CopyPropertyComplexValue: object: %s, property %s\n",BACAPI_BACnetObjectName(new_ref->oid.type),BACAPI_BACnetPropertyName(new_ref->propertyId));
			        to->complexValue.vObjectPropertyReference.push_back(new_ref);
			       }
			   from->complexValue.vCalendarEntryList.clear();
			   break;
			  }

         case PROP_EFFECTIVE_PERIOD:
              {
               memcpy(&to->complexValue.dateRange,&from->complexValue.dateRange,sizeof(BACNET_DATE_RANGE));
               break;
              }

         case PROP_WEEKLY_SCHEDULE:
              {
               memcpy(&to->complexValue.weeklySchedule, &from->complexValue.weeklySchedule, sizeof(BACNET_WEEKLY_SCHEDULE));
               break;
              }
         case PROP_EXCEPTION_SCHEDULE:
              {
               size_t count = from->complexValue.vSpecialEventList.size();
               for (size_t c=0;c<count;c++)
                   {
                    BACNET_SPECIAL_EVENT *event = &from->complexValue.vSpecialEventList[c];
                    BACNET_SPECIAL_EVENT new_event;
                    new_event.TagSpecialEventType = event->TagSpecialEventType;
                    switch (event->TagSpecialEventType)
                           {
                            case 0: // calendar entry
                            new_event.period.calendarEntry.TagEntryType = event->period.calendarEntry.TagEntryType;
                            switch (event->period.calendarEntry.TagEntryType)
                                   {
                                    case 0:
                                    memcpy(&new_event.period.calendarEntry.Entry.date,&event->period.calendarEntry.Entry.date,sizeof(BACNET_DATE));
                                    break;
                                    case 1:
                                    memcpy(&new_event.period.calendarEntry.Entry.dateRange.startdate,&event->period.calendarEntry.Entry.dateRange.startdate,sizeof(BACNET_DATE));
                                    memcpy(&new_event.period.calendarEntry.Entry.dateRange.enddate, &event->period.calendarEntry.Entry.dateRange.enddate,sizeof(BACNET_DATE));
                                    case 2:
                                    memcpy(&new_event.period.calendarEntry.Entry.weekNDay,&event->period.calendarEntry.Entry.weekNDay,sizeof(BACNET_WEEKNDAY));
                                    break;
                                    default: break;
                                   }
                            event->period.calendarEntry.TagEntryType = event->period.calendarEntry.TagEntryType;
                            break;

                            case 1:  // object id
                            new_event.period.calendarReference.instance = event->period.calendarReference.instance;
                            new_event.period.calendarReference.type = event->period.calendarReference.type;
                            break;

                            default: break;
                           }
                    size_t time_val_count = event->vTimeValueList.size();

                    for (size_t i=0;i<time_val_count;i++)
                        {
                         BACNET_TIME_VALUE *time_val = &event->vTimeValueList[i];
                         BACNET_TIME_VALUE new_time_val;
                         memcpy(&new_time_val.Time,&time_val->Time,sizeof(BACNET_TIME));
                         memcpy(&new_time_val.Value,&time_val->Value,sizeof(BACNET_SHORT_APPLICATION_DATA_VALUE));
                         new_event.vTimeValueList.push_back(new_time_val);
                        }
                    new_event.eventPriority = event->eventPriority;
                    to->complexValue.vSpecialEventList.push_back(new_event);
                   }
               break;
              }




         case PROP_PRIORITY_ARRAY:
              {
               // READ_ONLY
               break;
              }
         case PROP_DATE_LIST:
              {
               BACAPI_DeleteComplexProperty(to);  // isprazni primatelja

               size_t count = from->complexValue.vCalendarEntryList.size();
               for (size_t c=0;c<count;c++)
                   {
                    BACNET_CALENDAR_ENTRY *ptr_entry = from->complexValue.vCalendarEntryList[c];
                    to->complexValue.vCalendarEntryList.push_back(ptr_entry);
                   }
               from->complexValue.vCalendarEntryList.clear();
              }


         case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
         case PROP_RESTART_NOTIFICATION_RECIPIENTS:
         case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
              {
               // izbrisi sve stare vrijednosti
               // kopiraj sve vrijednosti iz from u to
               // isprazni from vektor
               BACAPI_DeleteRecipientList(to);

               size_t count = from->complexValue.vRecipientList.size();
               for (size_t c=0;c<count;c++)
                   {
                    BACNET_RECIPIENT *ptr_recipient = from->complexValue.vRecipientList[c];
                    to->complexValue.vRecipientList.push_back(ptr_recipient);
                   }
               from->complexValue.vRecipientList.clear();
               break;
              }

         case PROP_ACTION:
              {
               BACAPI_DeleteActionLists(to);
               size_t list_count,action_count;

               list_count = from->complexValue.vBACnetActionList.size();
               for (int c=0;c<list_count;c++)
				   {
					BACNET_ACTION_LIST *ptr_list = from->complexValue.vBACnetActionList[c];
					to->complexValue.vBACnetActionList.push_back(ptr_list);
				   }
               from->complexValue.vBACnetActionList.clear();
               break;
              }

         case PROP_MANUAL_SLAVE_ADDRESS_BINDING:
              {
               to->complexValue.vBACnetAddressBindingList = from->complexValue.vBACnetAddressBindingList;
               break;
              }


         default: break;
        }
}


extern void BACAPI_DeleteActionLists(BACNET_OBJECT_PROPERTY *prop)
{
 int list_count,action_count;
 list_count = (int)prop->complexValue.vBACnetActionList.size();
 for (int c=0;c<list_count;c++)
     {
      BACNET_ACTION_LIST *ptr_list = prop->complexValue.vBACnetActionList[c];

      action_count = (int)ptr_list->vBACnetActionCommand.size();
      for (int i=0;i<action_count;i++)
          {
           BACNET_ACTION_COMMAND *ptr_command = &ptr_list->vBACnetActionCommand[i];
           BACAPI_DeleteActionProperty(ptr_command->property);
          }
      ptr_list->vBACnetActionCommand.clear();

      delete ptr_list;
      ptr_list = NULL;
     }
 prop->complexValue.vBACnetActionList.clear();
}



void BACAPI_DeleteActionProperty(BACNET_ACTION_PROPERTY *prop)
{
 BACAPI_DeleteApplicationDataValue(prop->value);
 delete prop;
 prop = NULL;
}


// NOTE: provjeriti ovu funkciju i njezinu svrhu
void BACAPI_DefaultComplexValue(BACNET_OBJECT_PROPERTY *prop)
{
 switch (prop->property)
        {
        case PROP_ACCESS_EVENT_CREDENTIAL:
        case PROP_ACCESS_DOORS:
        case PROP_ZONE_FROM:
        case PROP_ZONE_TO:
        case PROP_CREDENTIALS_IN_ZONE:
        case PROP_LAST_CREDENTIAL_ADDED:
        case PROP_LAST_CREDENTIAL_REMOVED:
        case PROP_MEMBERS:
        case PROP_CREDENTIALS:
        case PROP_BELONGS_TO:
        case PROP_LAST_ACCESS_POINT:
        case PROP_LOG_DEVICE_OBJECT_PROPERTY:
        case PROP_LINK_REFERENCE_IN:
        case PROP_LINK_REFERENCE_OUT:
        case PROP_LIST_OF_OBJECT_PROPERTY_REFERENCES:

			  {
			   //BACAPI_CreateDeviceObjectPropertyReference(prop);

			   break;
              }

        case PROP_TIME_OF_ACTIVE_TIME_RESET:
        case PROP_TIME_OF_STATE_COUNT_RESET:
        case PROP_CHANGE_OF_STATE_TIME:
        case PROP_MAXIMUM_VALUE_TIMESTAMP:
        case PROP_MINIMUM_VALUE_TIMESTAMP:
        case PROP_VALUE_CHANGE_TIME:
        case PROP_START_TIME:
        case PROP_STOP_TIME:
        case PROP_MODIFICATION_DATE:
        case PROP_UPDATE_TIME:
        case PROP_COUNT_CHANGE_TIME:
        case PROP_LAST_CREDENTIAL_ADDED_TIME:
        case PROP_LAST_CREDENTIAL_REMOVED_TIME:
        case PROP_ACTIVATION_TIME:
        case PROP_EXPIRATION_TIME:
        case PROP_LAST_USE_TIME:
			  {
			   //BACAPI_CreateDateTime(prop);
			   break;
			  }

         case PROP_SUBORDINATE_LIST:
         case PROP_DOOR_MEMBERS:
         case PROP_ZONE_MEMBERS:
         case PROP_MEMBER_OF:
         case PROP_ENTRY_POINTS:
         case PROP_EXIT_POINTS:
         case PROP_ACCOMPANIMENT:
			  {
			   //BACAPI_CreateDeviceObjectReference(prop);
			   break;
			  }



         case PROP_SETPOINT_REFERENCE:
         case PROP_MANIPULATED_VARIABLE_REFERENCE:
         case PROP_CONTROLLED_VARIABLE_REFERENCE:
         case PROP_INPUT_REFERENCE:
         case PROP_OBJECT_PROPERTY_REFERENCE:
			  {
			   //BACAPI_CreateObjectPropertyReference(prop);
			   break;
			  }




         case PROP_TIME_SYNCHRONIZATION_RECIPIENTS:
         case PROP_RESTART_NOTIFICATION_RECIPIENTS:
         case PROP_UTC_TIME_SYNCHRONIZATION_RECIPIENTS:
              {
			   //BACAPI_CreateRecipient(prop);
               break;
              }


         case PROP_RECIPIENT_LIST:
			  {
			   //BACAPI_CreateBACDectination(prop);
			   break;
			  }


         case PROP_EFFECTIVE_PERIOD:
              {
			   BACAPI_CreateDateRange(prop);
               break;
              }


         case PROP_EVENT_TIME_STAMPS:
         case PROP_LAST_RESTORE_TIME:
         case PROP_TIME_OF_DEVICE_RESTART:
         case PROP_ACCESS_EVENT_TIME:
			  {
			   //BACAPI_CreateTimeStamps(prop);
			   break;
			  }

         case PROP_ACTIVE_COV_SUBSCRIPTIONS:
              {
               //BACAPI_CreateCOVSubscriptions(prop);
               break;
              }
         break;


         case PROP_DATE_LIST:
              {
               //BACAPI_CreateCalendarEntry(prop);
               break;
              }
         break;

         case PROP_PRIORITY_ARRAY:
			  {
			   //BACAPI_CreatePriorityList(prop);
			   break;
			  }

         case PROP_WEEKLY_SCHEDULE:
              {
               //BACAPI_CreateWeeklySchedule(prop);
               break;
              }

         case PROP_EXCEPTION_SCHEDULE:
              {
               //BACAPI_CreateSpecialEvent(prop);
               break;
              }

         case PROP_LIST_OF_GROUP_MEMBERS:
              {
               //BACAPI_CreateReadAccessSpecification(prop);
               break;
              }

         case PROP_DEVICE_ADDRESS_BINDING:
         case PROP_SLAVE_ADDRESS_BINDING:
              {
               //BACAPI_CreateDeviceAddressBind(prop);
               break;
              }

         case PROP_ACTION:
              {
               //BACAPI_CreateBACnetActionList(prop);
               break;
              }

         case PROP_ACTION_TEXT:
              {
               //BACAPI_CreateActionText(prop);
               break;
              }

		 case PROP_ACCEPTED_MODES:
		 case PROP_LIFE_SAFETY_ALARM_VALUES:
		 case PROP_ALARM_VALUES:
		 case PROP_FAULT_VALUES:
		      {
		       //BACAPI_CreateEnumList(prop);
		       break;
		      }

		 case PROP_EVENT_PARAMETERS:
		      {
		       //BACAPI_CreateEventParameters(prop);
		       break;
		      }

         case PROP_LOG_BUFFER:
              {
               //BACAPI_CreateLogBuffer(prop);
               break;
              }

         case PROP_SCALE:
              {
               //BACAPI_CreateScale(prop);
               break;
              }

         case PROP_PRESCALE:
              {
               //BACAPI_CreatePreScale(prop);
               break;
              }

         case PROP_REQUESTED_SHED_LEVEL:
         case PROP_ACTUAL_SHED_LEVEL:
         case PROP_EXPECTED_SHED_LEVEL:
              {
               //BACAPI_CreateShedLevel(prop);
               break;
              }
         case PROP_LOGGING_RECORD:
              {
               // prazna.
               // logging record se ne stvara
               break;
              }
        }

}


void BACAPI_CreateDateRange(BACNET_OBJECT_PROPERTY *prop)
{
 prop->complexValue.dateRange.enddate.day = 1;
 prop->complexValue.dateRange.enddate.month = 1;
 prop->complexValue.dateRange.enddate.wday = 1;
 prop->complexValue.dateRange.enddate.year = 2000;

 prop->complexValue.dateRange.startdate.day = 1;
 prop->complexValue.dateRange.startdate.month = 1;
 prop->complexValue.dateRange.startdate.wday = 1;
 prop->complexValue.dateRange.startdate.year = 2000;
}


//void BACAPI_DeleteObjectPropRef(BACNET_OBJECT_PROPERTY_REFERENCE *ref);  //zove se iz  BACAPI_DeleteComplexProperty
//void BACAPI_DeleteDeviceObjectPropRef(BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE *ref);  //zove se iz  BACAPI_DeleteComplexProperty
//void BACAPI_DeleteDeviceObjectRef(BACNET_DEVICE_OBJECT_REFERENCE *ref);  //zove se iz  BACAPI_DeleteComplexProperty
//void BACAPI_DeleteEnumList(BACNET_APPLICATION_DATA_VALUE *val);
//void BACAPI_DeleteTimeStamps(BACNET_TIMESTAMP *val);
//void BACAPI_DeleteRecipient(BACNET_RECIPIENT *rec);
//void BACAPI_DeleteDestination(BACNET_DESTINATION *dest);
//void BACAPI_DeleteWeeklySchedule(BACNET_WEEKLY_SCHEDULE *sch);
//void BACAPI_DeleteCalendarDateList(BACNET_CALENDAR_ENTRY *cal);
//void BACAPI_DeleteReadAccessSpec(BACNET_READ_ACCESS_SPECIFICATION *spec);
//void BACAPI_DeleteBACnetActionList(BACNET_ACTION_LIST *action);
//void BACAPI_DeleteBACnetActionCommand(BACNET_ACTION_COMMAND *command);
//void BACAPI_DeleteActionText(BACNET_ACTION_TEXT *text);


//
//void BACAPI_CreateDeviceObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop)
//{
//// prop->complexValue.deviceObjectPropReference = NULL;
// //prop->complexValue.deviceObjectPropReference = new BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE;
// //ZeroMemory(prop->complexValue.deviceObjectPropReference,sizeof(BACNET_DEVICE_OBJECT_PROPERTY_REFERENCE));
// //prop->complexValue.deviceObjectPropReference->arrayIndex = -1;
// //prop->complexValue.deviceObjectPropReference->deviceIndentifier.type = 8;
// //prop->complexValue.deviceObjectPropReference->deviceIndentifier.instance = 0;
// //prop->complexValue.deviceObjectPropReference->objectIdentifier.type = 0;
// //prop->complexValue.deviceObjectPropReference->objectIdentifier.instance = 0;
// //prop->complexValue.deviceObjectPropReference->propertyIdentifier = (BACNET_PROPERTY_ID)0;
//}
//
//
//void BACAPI_CreateDateTime(BACNET_OBJECT_PROPERTY *prop)
//{
//    /*
// SYSTEMTIME st;
// GetSystemTime(&st);
// prop->complexValue.dateTime.date.day = st.wDay;
// prop->complexValue.dateTime.date.month = st.wMonth;
// prop->complexValue.dateTime.date.wday = st.wDayOfWeek;
// prop->complexValue.dateTime.date.year = st.wYear;
// prop->complexValue.dateTime.time.hour = st.wHour;
// prop->complexValue.dateTime.time.minute = st.wMinute;
// prop->complexValue.dateTime.time.sec = st.wSecond;
// */
//}
//
//
//void BACAPI_CreateDeviceObjectReference(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.deviceObjectReference = NULL;
// //prop->complexValue.deviceObjectReference = new BACNET_DEVICE_OBJECT_REFERENCE;
// //ZeroMemory(prop->complexValue.deviceObjectReference,sizeof(BACNET_DEVICE_OBJECT_REFERENCE));
//}
//
//
//void BACAPI_CreateSetPointObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop)
//{
//// net postoji
//}
//
//
//void BACAPI_CreateObjectPropertyReference(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.objectPropertyReference = NULL;
// //prop->complexValue.objectPropertyReference = new BACNET_OBJECT_PROPERTY_REFERENCE;
// //ZeroMemory(prop->complexValue.objectPropertyReference,sizeof(BACNET_OBJECT_PROPERTY_REFERENCE));
//}
//
//
//void BACAPI_CreateRecipient(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.recipient = NULL;
// //prop->complexValue.recipient = new BACNET_RECIPIENT;
// //ZeroMemory(prop->complexValue.recipient,sizeof(BACNET_RECIPIENT));
//}
//
//
//void BACAPI_CreateBACDectination(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.destination = NULL;
// //prop->complexValue.destination = new BACNET_DESTINATION;
// //ZeroMemory(prop->complexValue.destination,sizeof(BACNET_DESTINATION));
//}
//
//
//void BACAPI_CreateTimeStamps(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.timeStamp = NULL;
// //prop->complexValue.timeStamp = new BACNET_TIMESTAMP;
// //ZeroMemory(prop->complexValue.timeStamp,sizeof(BACNET_TIMESTAMP));
//}
//
//void BACAPI_CreateCOVSubscriptions(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.covs = NULL;
// //prop->complexValue.covs = new BACNET_COV_SUBSCRIPTION;
// //ZeroMemory(prop->complexValue.covs,sizeof(BACNET_COV_SUBSCRIPTION));
//}
//
//void BACAPI_CreateCalendarEntry(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.calendarEntry = NULL;
// //prop->complexValue.calendarEntry = new BACNET_CALENDAR_ENTRY;
// //ZeroMemory(prop->complexValue.calendarEntry,sizeof(BACNET_CALENDAR_ENTRY));
//}
//
//
//
//void BACAPI_CreatePriorityList(BACNET_OBJECT_PROPERTY *prop)
//{
// // pocetni priority list se stvara s app_tagom NULL
// //for (int c=0;c<16;c++)
// //    {
// //     BACNET_APPLICATION_DATA_VALUE *priority_value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_NULL);
// //     prop->vValueList.push_back(priority_value);
// //    }
//}
//
//
//
//
//void BACAPI_CreateWeeklySchedule(BACNET_OBJECT_PROPERTY *prop)
//{
// //for (int c=0;c<7;c++)
// //    {
// //     prop->complexValue.weeklySchedule.weeklySchedule[c].dayValueCount = 0;
//
// //     for (int i=0;i<MAX_DAY_SCHEDULE_VALUES;i++)
// //         {
//	//       prop->complexValue.weeklySchedule.weeklySchedule[c].daySchedule[i].Time.hour = 1;
//	//       prop->complexValue.weeklySchedule.weeklySchedule[c].daySchedule[i].Time.sec = 1;
//	//       prop->complexValue.weeklySchedule.weeklySchedule[c].daySchedule[i].Time.minute = 1;
//	//       prop->complexValue.weeklySchedule.weeklySchedule[c].daySchedule[i].Value.tag = BACNET_APPLICATION_TAG_NULL;
//	//      }
//	// }
//}
//
//void BACAPI_CreateSpecialEvent(BACNET_OBJECT_PROPERTY *prop)
//{
// prop->complexValue.vSpecialEventList.clear();
// // prazan
//}
//
//void BACAPI_CreateReadAccessSpecification(BACNET_OBJECT_PROPERTY *prop)
//{
// prop->complexValue.readAccessSpec = NULL;
// //prop->complexValue.readAccessSpec = new BACNET_READ_ACCESS_SPECIFICATION;
// //ZeroMemory(prop->complexValue.readAccessSpec,sizeof(BACNET_READ_ACCESS_SPECIFICATION));
//}
//
//void BACAPI_CreateDeviceAddressBind(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.addressBinding = NULL;
// //prop->complexValue.addressBinding = new BACNET_ADDRESS_BINDING;
// //ZeroMemory(prop->complexValue.addressBinding,sizeof(BACNET_ADDRESS_BINDING));
//}
//
//void BACAPI_CreateBACnetActionList(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->complexValue.actionCommandList = NULL;
///* prop->complexValue.actionCommandList = new BACNET_ACTION_LIST;
// ZeroMemory(prop->complexValue.actionCommandList,sizeof(BACNET_ACTION_LIST)); */
//}
//
//void BACAPI_CreateActionText(BACNET_OBJECT_PROPERTY *prop)
//{
// prop->complexValue.actionText = NULL;
// //prop->complexValue.actionText = new BACNET_ACTION_TEXT;
// //ZeroMemory(prop->complexValue.actionText,sizeof(BACNET_ACTION_TEXT));
//}
//
//
//
//// KOMENTIRANO 13.11.2013
//void BACAPI_CreateEnumList(BACNET_OBJECT_PROPERTY *prop)
//{
// //prop->value = NULL;
// ////prop->value = BACAPI_CreateApplicationDataValue(BACNET_APPLICATION_TAG_ENUM);
//}
//
//
//void BACAPI_CreateEventParameters(BACNET_OBJECT_PROPERTY *prop)
//{
// // prazna
//}
//
//void BACAPI_CreateLogBuffer(BACNET_OBJECT_PROPERTY *prop)
//{
// // prazna, jos nije stvoren data tip
//}
//
//void BACAPI_CreateScale(BACNET_OBJECT_PROPERTY *prop)
//{
// // prazna
//}
//
//
//void BACAPI_CreatePreScale(BACNET_OBJECT_PROPERTY *prop)
//{
// // prazna
//}
//
//
//void BACAPI_CreateShedLevel(BACNET_OBJECT_PROPERTY *prop)
//{
// // prazna
//}
//



bool BACAPI_SetApplicationDataValue(string str_value, BACNET_APPLICATION_DATA_VALUE *val)
{
 switch (val->application_tag)
        {
         // ovi se ne izvrsavaju
		 case BACNET_APPLICATION_TAG_NULL:
		 case BACNET_APPLICATION_TAG_DATE:
		 case BACNET_APPLICATION_TAG_TIME:
		 return false;

		 case BACNET_APPLICATION_TAG_OID:
		 {
		  val->type.Object_Id.instance = BACAPI_ToUint32(str_value);
		  return true;
		 }

		 case BACNET_APPLICATION_TAG_BSTRING:
         return false;

		 case BACNET_APPLICATION_TAG_BOOL:
		 {
		  bool b = BACAPI_ToInt(str_value);
		  val->type.Boolean = b;
		  return true;
		 }

		 case BACNET_APPLICATION_TAG_UINT:
		 val->type.Unsigned_Int = BACAPI_ToUint32(str_value);
		 return true;

		 case BACNET_APPLICATION_TAG_SINT:
		 val->type.Signed_Int = BACAPI_ToInt(str_value);
		 return true;

		 case BACNET_APPLICATION_TAG_REAL:
		 val->type.Real = BACAPI_ToFloat(str_value);
		 return true;

		 case BACNET_APPLICATION_TAG_DOUBLE:
		 val->type.Double = BACAPI_ToDouble(str_value);
		 return true;

		 case BACNET_APPLICATION_TAG_OSTRING:
		 {
		  int len = (int)str_value.size()/2;
		  
		  uint8_t *out = new uint8_t[len];
		  BACAPI_ascii2hex_block(out,len,str_value.c_str());
		  val->type.Octet_String.vValue.clear();
		  
		  BACAPI_InitBACnetOctetString(&val->type.Octet_String,out,len);
		  return true;
		 }

		 case BACNET_APPLICATION_TAG_CSTRING:
		 {
		  switch (val->type.character_String.encoding)
		         {
				  case CHAR_SET_UCS2:
				  {
				   wstring wstr(str_value.begin(), str_value.end());
				   val->type.character_String.value16 = wstr;
				   break;
				  }

				  case CHAR_SET_JISC_6226:
				  case CHAR_SET_ISO8859:
				  case CHAR_SET_ANSI_X34:
				  {
				   val->type.character_String.value8 = str_value;
				   break;
				  }
		         }
		  //val->type.character_String = new BACNET_CHARACTER_STRING;
		  //val->type.character_String.encoding = CHAR_SET_ANSI_X34;
		  //val->type.character_String.value8 = str_value;
		  return true;

		 }

		 case BACNET_APPLICATION_TAG_ENUM:
		 val->type.Enumerated = BACAPI_ToUint32(str_value);
		 return true;

         default: return false;
        }
}





// vrati tag u odnosu na property
// TODO: promjeniti oid u samo type
BACNET_APPLICATION_TAG BACAPI_GetApplicationTag(BACNET_OBJECT_TYPE type, BACNET_PROPERTY_ID propid)
{
 switch (propid)
        {
         // OID
         case PROP_OBJECT_IDENTIFIER:
         case PROP_OBJECT_LIST:
         case PROP_STRUCTURED_OBJECT_LIST:
         case PROP_CONFIGURATION_FILES:
         case PROP_LOGGING_OBJECT:
         {
          return BACNET_APPLICATION_TAG_OID;
         }

         // REAL
         case PROP_DIRECT_READING:
         case PROP_MIN_PRES_VALUE:
         case PROP_MAX_PRES_VALUE:
         case PROP_RESOLUTION:
         case PROP_COV_INCREMENT:
         case PROP_HIGH_LIMIT:
         case PROP_LOW_LIMIT:
         case PROP_DEADBAND:
         case PROP_RELINQUISH_DEFAULT:
         case PROP_MINIMUM_VALUE:
         case PROP_AVERAGE_VALUE:
         case PROP_VARIANCE_VALUE:
         case PROP_MAXIMUM_VALUE:
         case PROP_PROPORTIONAL_CONSTANT:
         case PROP_INTEGRAL_CONSTANT:
         case PROP_DERIVATIVE_CONSTANT:
         case PROP_BIAS:
         case PROP_MAXIMUM_OUTPUT:
         case PROP_MINIMUM_OUTPUT:
         case PROP_ERROR_LIMIT:
         case PROP_SETPOINT:
         case PROP_CONTROLLED_VARIABLE_VALUE:
         case PROP_SCALE_FACTOR:
         case PROP_ADJUST_VALUE:
         case PROP_FULL_DUTY_BASELINE:
         case PROP_CLIENT_COV_INCREMENT:  // CHOICE: real_increment, default_increment [null]
         case PROP_DEFAULT_RAMP_RATE:
         case PROP_DEFAULT_STEP_INCREMENT:
         case PROP_POWER:
         case PROP_INSTANTANEOUS_POWER:
         case PROP_MAX_ACTUAL_VALUE:
         case PROP_MIN_ACTUAL_VALUE:
         case PROP_LINK_SPEED:
         case PROP_LINK_SPEEDS:
         {
          return BACNET_APPLICATION_TAG_REAL;
         }


         case PROP_PRESENT_VALUE:
         {
          switch (type)
                 {
                  case OBJECT_BINARY_INPUT:
                  case OBJECT_BINARY_OUTPUT:
                  case OBJECT_BINARY_VALUE:
                  case OBJECT_LIFE_SAFETY_POINT:  // BACnetLifeSafetyState
                  return BACNET_APPLICATION_TAG_ENUM;

                  case OBJECT_COMMAND:
                  return BACNET_APPLICATION_TAG_UINT;
                  default: break;
                 }


#ifdef BACAPI_DOUBLE
          return BACNET_APPLICATION_TAG_DOUBLE;
#else
          return BACNET_APPLICATION_TAG_REAL;
#endif
         }

         // ENUM
         case PROP_OBJECT_TYPE:
         case PROP_SYSTEM_STATUS:
         case PROP_SEGMENTATION_SUPPORTED:
         case PROP_EVENT_STATE:
         case PROP_RELIABILITY:
         case PROP_UNITS:
         case PROP_NOTIFY_TYPE:
         case PROP_POLARITY:
         case PROP_ALARM_VALUE:
         case PROP_FEEDBACK_VALUE:
         case PROP_VT_CLASSES_SUPPORTED:
         case PROP_BACKUP_AND_RESTORE_STATE:
         case PROP_LAST_RESTART_REASON:
         case PROP_PROPERTY_LIST:
         case PROP_FILE_ACCESS_METHOD:
         case PROP_FAULT_TYPE:
         case PROP_TRACKING_VALUE:
         case PROP_MODE:
         case PROP_SILENCED:
         case PROP_OPERATION_EXPECTED:
         case PROP_MAINTENANCE_REQUIRED:
         case PROP_OUTPUT_UNITS:
         case PROP_PROPORTIONAL_CONSTANT_UNITS:
         case PROP_INTEGRAL_CONSTANT_UNITS:
         case PROP_DERIVATIVE_CONSTANT_UNITS:
         case PROP_CONTROLLED_VARIABLE_UNITS:
         case PROP_PROGRAM_STATE:
         case PROP_PROGRAM_CHANGE:
         case PROP_REASON_FOR_HALT:
         case PROP_DOOR_STATUS:
         case PROP_LOCK_STATUS:
         case PROP_SECURED_STATUS:
         case PROP_DOOR_ALARM_STATE:
         case PROP_MASKED_ALARM_VALUES:
         case PROP_LOGGING_TYPE:
         case PROP_AUTHENTICATION_STATUS:
         case PROP_AUTHORIZATION_MODE:
         case PROP_FAILED_ATTEMPT_EVENTS: // list of BACnetAccessEvent enum
         case PROP_ACCESS_EVENT:
         case PROP_ACCESS_ALARM_EVENTS:  // list of BACnetAccessEvent enum
         case PROP_ACCESS_TRANSACTION_EVENTS: //list of BACnetAccessEvent enum
         case PROP_ACCESS_EVENT_AUTHENTICATION_FACTOR:
         case PROP_OCCUPANCY_STATE:
         case PROP_PASSBACK_MODE:
         case PROP_USER_TYPE:
         case PROP_CREDENTIAL_STATUS: // BinaryPV
         case PROP_REASON_FOR_DISABLE: // list of BACnetAccessCredentialDisableReason
         case PROP_CREDENTIAL_DISABLE:
         case PROP_LAST_ACCESS_EVENT:
         case PROP_AUTHORIZATION_EXEMPTIONS:  // list of BACnetAuthorizationExemptions
         case PROP_WRITE_STATUS:
         case PROP_IN_PROGRESS:
         case PROP_TRANSITION:
         case PROP_NETWORK_TYPE:
         case PROP_PROTOCOL_LEVEL:
         case PROP_COMMAND:   
         case PROP_NETWORK_NUMBER_QUALITY:
         case PROP_BACNET_IP_MODE:
         case PROP_BACNET_IPV6_MODE:
         {
          return BACNET_APPLICATION_TAG_ENUM;
         }

         // CSTRING
         case PROP_ACTIVE_TEXT:
         case PROP_APPLICATION_SOFTWARE_VERSION:
         case PROP_OBJECT_NAME:
         case PROP_VENDOR_NAME:
         case PROP_MODEL_NAME:
         case PROP_FIRMWARE_REVISION:
         case PROP_LOCATION:
         case PROP_DESCRIPTION:
         case PROP_DEVICE_TYPE:
         case PROP_PROFILE_NAME:
         case PROP_INACTIVE_TEXT:
         case PROP_SERIAL_NUMBER:
         case PROP_EVENT_MESSAGE_TEXTS:
         case PROP_EVENT_MESSAGE_TEXTS_CONFIG:
         case PROP_FILE_TYPE:
         case PROP_DESCRIPTION_OF_HALT:
         case PROP_PROGRAM_LOCATION:
         case PROP_INSTANCE_OF:
         case PROP_STATE_DESCRIPTION:
         case PROP_SHED_LEVEL_DESCRIPTIONS:  // array[n] of cstring
         case PROP_AUTHENTICATION_POLICY_NAMES:  // array
         case PROP_USER_NAME:
         case PROP_USER_EXTERNAL_IDENTIFIER:
         case PROP_USER_INFORMATION_REFERENCE:
         case PROP_BIT_TEXT:
         case PROP_GROUP_MEMBER_NAMES:
         case PROP_NETWORK_INTERFACE_NAME:
         case PROP_PROFILE_LOCATION:
         case PROP_IPV6_ZONE_INDEX:
         {
          return BACNET_APPLICATION_TAG_CSTRING;
         }

         // UINT
         case PROP_SETTING:  // UINT8
         case PROP_APDU_SEGMENT_TIMEOUT:
         case PROP_APDU_TIMEOUT:
         case PROP_ALARM_VALUES:
         case PROP_BACKUP_FAILURE_TIMEOUT:
         case PROP_BACKUP_PREPARATION_TIME:
         case PROP_CHANGE_OF_STATE_COUNT:
         case PROP_DATABASE_REVISION:
         case PROP_ELAPSED_ACTIVE_TIME:
         case PROP_FILE_SIZE:
         case PROP_FAULT_VALUES:
         case PROP_PROTOCOL_VERSION:
         case PROP_PROTOCOL_REVISION:
         case PROP_UPDATE_INTERVAL:
         case PROP_MAX_APDU_LENGTH_ACCEPTED:
         case PROP_MAX_MASTER:
         case PROP_MAX_INFO_FRAMES:
         case PROP_MINIMUM_OFF_TIME:
         case PROP_NUMBER_OF_APDU_RETRIES:
         case PROP_NOTIFICATION_CLASS:
         case PROP_MINIMUM_ON_TIME:
         case PROP_MAX_SEGMENTS_ACCEPTED:
         case PROP_VENDOR_IDENTIFIER:
         case PROP_RESTORE_PREPARATION_TIME:
         case PROP_RESTORE_COMPLETION_TIME:
         case PROP_TIME_DELAY:
         case PROP_TIME_DELAY_NORMAL:
         case PROP_TIME_SYNCHRONIZATION_INTERVAL:
         case PROP_RECORD_COUNT:
         case PROP_VALUE_BEFORE_CHANGE:
         case PROP_VALUE_SET:
         case PROP_PULSE_RATE:
         case PROP_LIMIT_MONITORING_INTERVAL:
         case PROP_ATTEMPTED_SAMPLES:
         case PROP_VALID_SAMPLES:
         case PROP_WINDOW_INTERVAL:
         case PROP_WINDOW_SAMPLES:
         case PROP_PRIORITY_FOR_WRITING:
         case PROP_COUNT:
         case PROP_COUNT_BEFORE_CHANGE:
         case PROP_COV_PERIOD:
		 case PROP_DOOR_PULSE_TIME:
		 case PROP_DOOR_EXTENDED_PULSE_TIME:
		 case PROP_DOOR_UNLOCK_DELAY_TIME:
		 case PROP_DOOR_OPEN_TOO_LONG_TIME:
		 case PROP_BUFFER_SIZE: // uin32
		 case PROP_TOTAL_RECORD_COUNT:
		 case PROP_NOTIFICATION_THRESHOLD:
		 case PROP_RECORDS_SINCE_NOTIFICATION:
		 case PROP_LAST_NOTIFY_RECORD:
		 case PROP_SHED_DURATION:
		 case PROP_DUTY_WINDOW:
		 case PROP_SHED_LEVELS:  //Array[n] Unsigned
		 case PROP_LOG_INTERVAL:
		 case PROP_INTERVAL_OFFSET:
		 case PROP_ACTIVE_AUTHENTICATION_POLICY:
		 case PROP_NUMBER_OF_AUTHENTICATION_POLICIES:
		 case PROP_VERIFICATION_TIME:
		 case PROP_LOCKOUT_RELINQUISH_TIME:
		 case PROP_FAILED_ATTEMPTS:
		 case PROP_MAX_FAILED_ATTEMPTS:
		 case PROP_FAILED_ATTEMPTS_TIME:
		 case PROP_THREAT_LEVEL:  // BACnetAccessThreatLevel uint (1...100)
		 case PROP_THREAT_AUTHORITY: // BACnetAccessThreatLevel uint (1...100)
		 case PROP_ACCOMPANIMENT_TIME:
		 case PROP_ACCESS_EVENT_TAG:
		 //case PROP_PRIORITY_FOR_WRITING:  // 1...16
		 case PROP_TRANSACTION_NOTIFICATION_CLASS:
		 case PROP_GLOBAL_IDENTIFIER:  // uin32
		 case PROP_OCCUPANCY_COUNT:
		 case PROP_OCCUPANCY_UPPER_LIMIT:
		 case PROP_OCCUPANCY_LOWER_LIMIT:
		 case PROP_PASSBACK_TIMEOUT:
		 case PROP_ABSENTEE_LIMIT:
		 case PROP_SUPPORTED_FORMAT_CLASSES:  // array
		 case PROP_SECURITY_TIME_WINDOW:
		 case PROP_PACKET_REORDER_TIME:
		 case PROP_DISTRIBUTION_KEY_REVISION:  // uint8
		 case PROP_SECURITY_PDU_TIMEOUT:    // uint16
		 case PROP_UPDATE_KEY_SET_TIMEOUT:  // uint16
		 case PROP_SUPPORTED_SECURITY_ALGORITHM:  // list of uint8
         case PROP_REQUESTED_UPDATE_INTERVAL:
         case PROP_COV_RESUBSCRIPTION_INTERVAL:
         case PROP_COVU_PERIOD:
         case PROP_PROCESS_IDENTIFIER_FILTER:
         case PROP_LAST_PRIORITY:
         case PROP_EXECUTION_DELAY:  // array of uint
         case PROP_CHANNEL_NUMBER:  // uint16
         case PROP_CONTROL_GROUPS:  // array of uint32
         case PROP_EGRESS_TIME:
         case PROP_DEFAULT_FADE_TIME:
         case PROP_LIGHTING_COMMAND_DEFAULT_PRIORITY:
         case PROP_REFERENCE_PORT:
         case PROP_NETWORK_NUMBER:
         case PROP_APDU_LENGTH:
         case PROP_BACNET_IP_UDP_PORT:
         case PROP_IP_DHCP_LEASE_TIME:
         case PROP_IP_DHCP_LEASE_TIME_REMAINING:
         case PROP_FD_SUBSCRIPTION_LIFETIME:
         case PROP_IPV6_PREFIX_LENGTH:
		 case PROP_BACNET_IPV6_UDP_PORT:
         case PROP_IPV6_DHCP_LEASE_TIME:
	     case PROP_IPV6_DHCP_LEASE_TIME_REMAINING:
         {
          return BACNET_APPLICATION_TAG_UINT;
         }

         // BSTR
         case PROP_PROTOCOL_SERVICES_SUPPORTED:
         case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
         case PROP_STATUS_FLAGS:
         case PROP_LIMIT_ENABLE:
         case PROP_ACKED_TRANSITIONS:
         case PROP_EVENT_ENABLE:
         case PROP_BIT_MASK:
         case PROP_MEMBER_STATUS_FLAGS:
         {
          return BACNET_APPLICATION_TAG_BSTRING;
         }

         // TIME
         case PROP_LOCAL_TIME:
         {
          return BACNET_APPLICATION_TAG_TIME;
         }

         // DATE
         case PROP_LOCAL_DATE:
         {
          return BACNET_APPLICATION_TAG_DATE;
         }

         // SINT
         case PROP_UTC_OFFSET:
         case PROP_DAYS_REMAINING:
         case PROP_USES_REMAINING:
         {
          return BACNET_APPLICATION_TAG_SINT;
         }

         // BOOL
         case PROP_AUTO_SLAVE_DISCOVERY:  // array
         case PROP_ALIGN_INTERVALS:
         case PROP_DAYLIGHT_SAVINGS_STATUS:
         case PROP_EVENT_DETECTION_ENABLE:
         case PROP_EVENT_ALGORITHM_INHIBIT:
         case PROP_SLAVE_PROXY_ENABLE:        // array
         case PROP_OUT_OF_SERVICE:
         case PROP_RELIABILITY_EVALUATION_INHIBIT:
         case PROP_ENABLE:
         case PROP_ARCHIVE:
         case PROP_READ_ONLY:
         case PROP_STOP_WHEN_FULL:
         case PROP_TRIGGER:
         case PROP_LOCKOUT:
         case PROP_OCCUPANCY_UPPER_LIMIT_ENFORCED:
         case PROP_OCCUPANCY_LOWER_LIMIT_ENFORCED:
         case PROP_OCCUPANCY_COUNT_ADJUST:
         case PROP_MUSTER_POINT:
         case PROP_OCCUPANCY_COUNT_ENABLE:
         case PROP_TRACE_FLAG:
         case PROP_EXTENDED_TIME_ENABLE:
         case PROP_IS_UTC:
         case PROP_DO_NOT_HIDE:
         case PROP_LOCAL_FORWARDING_ONLY:
         case PROP_ALLOW_GROUP_DELAY_INHIBIT:
         case PROP_BLINK_WARN_ENABLE:
         case PROP_EGRESS_ACTIVE:
         case PROP_CHANGES_PENDING:
         case PROP_LINK_SPEED_AUTONEGOTIATE:
         case PROP_IP_DHCP_ENABLE:
         case PROP_BACNET_IP_NAT_TRAVERSAL:
         case PROP_BBMD_ACCEPT_FD_REGISTRATIONS:
         case PROP_IPV6_AUTO_ADDRESSING_ENABLE:
         {
          return BACNET_APPLICATION_TAG_BOOL;
         }
         
         
         // OCTET STRING
         case PROP_MAC_ADDRESS:
         case PROP_IP_ADDRESS:
		 case PROP_IP_SUBNET_MASK:
		 case PROP_IP_DEFAULT_GATEWAY:
		 case PROP_BACNET_IP_MULTICAST_ADDRESS:
		 case PROP_IP_DHCP_SERVER:
		 case PROP_IPV6_ADDRESS:
		 case PROP_IPV6_DEFAULT_GATEWAY:
		 case PROP_BACNET_IPV6_MULTICAST_ADDRESS:
		 case PROP_IPV6_DHCP_SERVER:
		 case PROP_IP_DNS_SERVER:
         {
          return BACNET_APPLICATION_TAG_OSTRING;
         }

 //BACNET_APPLICATION_TAG_REAL     = 4,     // float
 //BACNET_APPLICATION_TAG_DOUBLE   = 5,     // double
 //BACNET_APPLICATION_TAG_OSTRING  = 6,     // oktet string
 //BACNET_APPLICATION_TAG_BSTRING  = 8,     // bit str

        }
  return BACNET_APPLICATION_TAG_NULL;
}




bool BACAPI_GetBitState(BACNET_BIT_STRING *bstr, int bit)
{
 return gDec->bitstring_bit(bstr,bit);
}




BACNET_OBJECT *BACAPI_GetObject(BACNET_DEVICE *device, BACNET_OBJECT_ID oid)
{
 size_t count = device->vObjects.size();
 for (size_t c=0;c<count;c++)
     {
      BACNET_OBJECT *obj = device->vObjects[c];
      if (obj->oid.instance == oid.instance && obj->oid.type == oid.type)
         {
          return obj;
         }
     }
 return 0;
}



BACNET_OBJECT *BACAPI_GetObject(BACNET_DEVICE *device, uint16_t type, uint32_t instance)
{
 size_t count = device->vObjects.size();
 for (size_t c=0;c<count;c++)
     {
      BACNET_OBJECT *obj = device->vObjects[c];
      if (obj->oid.instance == instance && obj->oid.type == type)
         {
          return obj;
         }
     }
 return 0;
}



const char *BACAPI_GetErrorString(BACAPI_ERROR err)
{
 //int er = err;

 if (err < MAX_BACAPI_ERROR_STATES || err > 0)
    {
     return "Unknown error state";
    }
 return bacapiErrorName[abs(err)].name;
}



BACNET_TIMESTAMP *BACAPI_CreateNewTimeStamp(BACNET_OBJECT_PROPERTY *prop)
{
 BACNET_TIMESTAMP *new_stamp = new BACNET_TIMESTAMP;
 new_stamp->tag = TIME_STAMP_TIME;
 new_stamp->value.dateTime.date.day = 1;
 new_stamp->value.dateTime.date.month = 1;
 new_stamp->value.dateTime.date.wday = 1;
 new_stamp->value.dateTime.date.year = 2012;

 new_stamp->value.time.hour = 1;
 new_stamp->value.time.hundredths = 1;
 new_stamp->value.time.minute = 1;
 new_stamp->value.time.sec = 1;

 prop->complexValue.vTimeStamps.push_back(new_stamp);

 return new_stamp;
}


// KOMENTIRANO 13.11.2013
BACNET_APPLICATION_DATA_VALUE *BACAPI_GetArrayElement(BACNET_APPLICATION_DATA_VALUE *value, int element)
{
 //int count = 0;
 //BACNET_APPLICATION_DATA_VALUE *linkedValue = value;
 // while (linkedValue != NULL && count < element)
 //       {
 //        count++;
 //        linkedValue = linkedValue->next;
 //       }
 //return linkedValue;

 return NULL;
}


void BACAPI_SetSupportedObject(BACNET_BIT_STRING *bstr, BACNET_OBJECT_TYPE object_type)
{




 switch(object_type)
       {
        case OBJECT_ANALOG_INPUT: bstr->value[0] += SET_BIT(0); break;
        case OBJECT_ANALOG_OUTPUT: bstr->value[0] += SET_BIT(1); break;
        case OBJECT_ANALOG_VALUE: bstr->value[0] += SET_BIT(2); break;
        case OBJECT_BINARY_INPUT: bstr->value[0] += SET_BIT(3); break;
        case OBJECT_BINARY_OUTPUT: bstr->value[0] += SET_BIT(4); break;
        case OBJECT_BINARY_VALUE : bstr->value[0] += SET_BIT(5); break;
        case OBJECT_CALENDAR : bstr->value[0] += SET_BIT(6); break;
        case OBJECT_COMMAND: bstr->value[0] += SET_BIT(7); break;
        case OBJECT_DEVICE: bstr->value[1] += SET_BIT(0); break;
    //OBJECT_EVENT_ENROLLMENT = 9,
    //OBJECT_FILE = 10,
    //OBJECT_GROUP = 11,
    //OBJECT_LOOP = 12,
    //OBJECT_MULTI_STATE_INPUT = 13,   // +
    //OBJECT_MULTI_STATE_OUTPUT = 14,    // +
    //OBJECT_NOTIFICATION_CLASS = 15,    // +
    //OBJECT_PROGRAM = 16,
    //OBJECT_SCHEDULE = 17,     // +
    //OBJECT_AVERAGING = 18,
    //OBJECT_MULTI_STATE_VALUE = 19,    // +
    //OBJECT_TRENDLOG = 20,
    //OBJECT_LIFE_SAFETY_POINT = 21,
    //OBJECT_LIFE_SAFETY_ZONE = 22,
    //OBJECT_ACCUMULATOR = 23,
    //OBJECT_PULSE_CONVERTER = 24,
    //OBJECT_EVENT_LOG = 25,
    //OBJECT_GLOBAL_GROUP = 26,
    //OBJECT_TREND_LOG_MULTIPLE = 27,
    //OBJECT_LOAD_CONTROL = 28,
    //OBJECT_STRUCTURED_VIEW = 29,
    //OBJECT_ACCESS_DOOR = 30,
    //OBJECT_LIGHTING_OUTPUT = 31,
    //OBJECT_ACCESS_CREDENTIAL = 32,
    //OBJECT_ACCESS_POINT = 33,
    //OBJECT_ACCESS_RIGHTS = 34,
    //OBJECT_ACCESS_USER = 35,
    //OBJECT_ACCESS_ZONE = 36,
    //OBJECT_AUTHENTICATION_FACTOR_INPUT = 37,
        default:
        break;
       }
  printf("");
}


void BACAPI_SetSupportedService(BACNET_BIT_STRING *bstr, BACNET_SUPPORTED_SERVICE service)
{
 switch(service)
       {
        // BYTE [0]
		case ACK_ALARM:
		bstr->value[0] += SET_BIT(0);
		break;
		case CONFIRMED_COV_NOTIF:
		bstr->value[0] += SET_BIT(1);
		break;
		case CONFIRMED_EVENT_NOTIF:
		bstr->value[0] += SET_BIT(2);
		break;
		case GET_ALARM_SUMM:
		bstr->value[0] += SET_BIT(3);
		break;
		case GET_ENROLL_SUMM:
		bstr->value[0] += SET_BIT(4);
		break;
		case SUBSCRIBE_COV:
		bstr->value[0] += SET_BIT(5);
		break;
		case ATOMIC_READ:
		bstr->value[0] += SET_BIT(6);
		break;
		case ATOMIC_WRITE:
		bstr->value[0] += SET_BIT(7);
		break;

		// BYTE [1];
        case ADD_LIST_ELEM:
		bstr->value[1] += SET_BIT(0);
		break;
        case REM_LIST_ELEM:
		bstr->value[1] += SET_BIT(1);
		break;
        case CREATE_OBJECT:
		bstr->value[1] += SET_BIT(2);
		break;
        case DELETE_OBJECT:
		bstr->value[1] += SET_BIT(3);
		break;
        case READ_PROP:
		bstr->value[1] += SET_BIT(4);
		break;
        case READ_PROP_COND:
		bstr->value[1] += SET_BIT(5);
		break;
        case READ_PROP_MULTI:
		bstr->value[1] += SET_BIT(6);
		break;
        case WRITE_PROP:
		bstr->value[1] += SET_BIT(7);
		break;

        // BYTE [2]
		case WRITE_PROP_MULTI:
		bstr->value[2] += SET_BIT(0);
		break;
		case DEV_COMM_CONTROL:
		bstr->value[2] += SET_BIT(1);
		break;
        case CONF_PRIVATE_TRANS:
		bstr->value[2] += SET_BIT(2);
		break;
		case CONF_TEXT_MSG:
		bstr->value[2] += SET_BIT(3);
		break;
		case REINIT_DEVICE:
		bstr->value[2] += SET_BIT(4);
		break;
		case VT_OPEN:
		bstr->value[2] += SET_BIT(5);
		break;
		case VC_CLOSE:
		bstr->value[2] += SET_BIT(6);
		break;
		case VT_DATA:
		bstr->value[2] += SET_BIT(7);
		break;

        // BYTE [3]
		case AUTHENTICATE:
		bstr->value[3] += SET_BIT(0);
		break;
		case REQUEST_KEY:
		bstr->value[3] += SET_BIT(1);
		break;
		case I_AM:
		bstr->value[3] += SET_BIT(2);
		break;
		case I_HAVE:
		bstr->value[3] += SET_BIT(3);
		break;
		case UNCONF_COV_NOTIF:
		bstr->value[3] += SET_BIT(4);
		break;
		case UNCONF_EVENT_NOTIF:
		bstr->value[3] += SET_BIT(5);
		break;
		case UNCONF_PRIV_TRANS:
		bstr->value[3] += SET_BIT(6);
		break;
		case UNCONF_TEXT_MSG:
		bstr->value[3] += SET_BIT(7);
		break;

        // BYTE [4]
		case TIME_SYNC:
		bstr->value[4] += SET_BIT(0);
		break;
		case WHO_HAS:
		bstr->value[4] += SET_BIT(1);
		break;
		case WHO_IS:
		bstr->value[4] += SET_BIT(2);
		break;
		case READ_RANGE:
		bstr->value[4] += SET_BIT(3);
		break;
		case UTC_TIME_SYNC:
		bstr->value[4] += SET_BIT(4);
		break;
		case LIFE_SAFETY_OP:
		bstr->value[4] += SET_BIT(5);
		break;
		case SUBSCRIBE_COV_PROP:
		bstr->value[4] += SET_BIT(6);
		break;
		case GET_EVENT_INFO:
		bstr->value[4] += SET_BIT(7);
		break;
	}
}





BACAPI_INVOKE_ID *BACAPI_GetInvokeLink(uint8_t invokeid)
{
 size_t cnt = vInvokeList.size();

 for (size_t c=0;c<cnt;c++)
     {
      BACAPI_INVOKE_ID *id = vInvokeList[c];
      if (id->invokeId == invokeid && id->active_ == true)
         {
          return vInvokeList[c];
         }
     }
 return NULL;
}



void BACAPI_FlushInvokeLink(void)
{
 size_t cnt = vInvokeList.size();

 for (size_t c=0;c<cnt;c++)
     {
      BACAPI_INVOKE_ID *id = vInvokeList[c];
      delete id;
     }
 vInvokeList.clear();
}



bool BACAPI_IsCommandableObject(BACNET_OBJECT_TYPE object_type)
{
 if (object_type == OBJECT_ANALOG_OUTPUT ||
     object_type == OBJECT_BINARY_OUTPUT ||
     object_type == OBJECT_MULTI_STATE_OUTPUT ||
     object_type == OBJECT_MULTI_STATE_VALUE ||
     object_type == OBJECT_ANALOG_VALUE ||
     object_type == OBJECT_BINARY_VALUE)
    {
     return true;
    }
 return false;
}

/*

m_sec	int	seconds after the minute	0-60*
tm_min	int	minutes after the hour	0-59
tm_hour	int	hours since midnight	0-23
tm_mday	int	day of the month	1-31
tm_mon	int	months since January	0-11
tm_year	int	years since 1900
tm_wday	int	days since Sunday	0-6
tm_yday	int	days since January 1	0-365
tm_isdst	int	Daylight Saving Time flag

*/
void BACAPI_SysDateToBACnetDate(BACNET_DATE *bac_date, struct tm *tt_date)
{
 bac_date->day = tt_date->tm_mday+1;
 bac_date->month = tt_date->tm_mon+1;
 bac_date->wday = tt_date->tm_wday+1;
 bac_date->year = tt_date->tm_year;
}
void BACAPI_BACnetDateToSysDate(struct tm *tt_date, BACNET_DATE *bac_date)
{
 tt_date->tm_mday = bac_date->day-1;
 tt_date->tm_mon = bac_date->month-1;
 tt_date->tm_wday = bac_date->wday-1;
 tt_date->tm_year = bac_date->year;
}
void BACAPI_SysTimeToBACnetTime(BACNET_TIME *bac_time, struct tm *tt_time)
{
 bac_time->hour = tt_time->tm_hour+1;
 bac_time->minute = tt_time->tm_min+1;
 bac_time->sec = tt_time->tm_sec+1;
 bac_time->hundredths = 1;
}
void BACAPI_BACnetTimeToSysTime(struct tm *tt_time, BACNET_TIME *bac_time)
{
 tt_time->tm_hour = bac_time->hour-1;
 tt_time->tm_min = bac_time->minute-1;
 tt_time->tm_sec = bac_time->sec-1;
}


void BACAPI_BACnetDateTimeToSysDateTime(struct tm  *tt_datetime, BACNET_DATE_TIME *bac_datetime)
{
 tt_datetime->tm_mday = bac_datetime->date.day;
 tt_datetime->tm_mon = bac_datetime->date.month;
 tt_datetime->tm_wday = bac_datetime->date.wday;
 tt_datetime->tm_year = bac_datetime->date.year;
 
 tt_datetime->tm_hour = bac_datetime->time.hour;
 tt_datetime->tm_min = bac_datetime->time.minute;
 tt_datetime->tm_sec = bac_datetime->time.sec; 
}

/*
void BACAPI_BACnetDateTimeToSysDateTime(SYSTEMTIME* sys_datetime, BACNET_DATE_TIME *bac_datetime)
{
 sys_datetime->wDay= bac_datetime->date.day;
 sys_datetime->wMonth = bac_datetime->date.month;
 sys_datetime->wDayOfWeek = bac_datetime->date.wday;
 sys_datetime->wYear = bac_datetime->date.year;
 
 sys_datetime->wHour = bac_datetime->time.hour;
 sys_datetime->wMinute = bac_datetime->time.minute;
 sys_datetime->wSecond = bac_datetime->time.sec; 
 sys_datetime->wMilliseconds = bac_datetime->time.hundredths; 
}*/

// pointer na count log rekorda koji se traze iz log_buffer propertija
// NULL kod greske
// TODO: provjere da li je out of range
BACNET_LOG_RECORD **BACAPI_GetLogRecordList(BACNET_OBJECT_PROPERTY *prop, BACNET_READ_RANGE *range)
{
 switch (range->requestType)
        {
		 case RANGE_BY_POSITION:
		 {
		  // da li postoji uopce koji log record?
		  if (prop->complexValue.vLogRecordList.size() == 0) return NULL;

		  BACNET_LOG_RECORD **log_list = new BACNET_LOG_RECORD*[range->rangeParam.count];
		  for (int c=range->rangeParam.index, i=0;c<range->rangeParam.count;c++)
		      {
		       log_list[i++] = prop->complexValue.vLogRecordList[c];
		      }
		  return log_list;
		 }

		 RANGE_BY_SEQUENCE:
		 {
		  break;
		 }

		 RANGE_BY_TIME:
		 {
		  break;
		 }
        }
 return NULL;
}



BACNET_LOG_MULTIPLE_RECORD **BACAPI_GetLogMultipleRecordList(BACNET_OBJECT_PROPERTY *prop, BACNET_READ_RANGE *range)
{
 switch (range->requestType)
        {
		 case RANGE_BY_POSITION:
		 {
		  // da li postoji uopce koji log record?
		  if (prop->complexValue.vLogMultipleList.size() == 0) return NULL;

		  BACNET_LOG_MULTIPLE_RECORD **log_list = new BACNET_LOG_MULTIPLE_RECORD*[range->rangeParam.count];
		  for (int c=range->rangeParam.index, i=0;c<range->rangeParam.count;c++)
		      {
		       log_list[i++] = prop->complexValue.vLogMultipleList[c];
		      }
		  return log_list;
		 }

		 RANGE_BY_SEQUENCE:
		 {
		  break;
		 }

		 RANGE_BY_TIME:
		 {
		  break;
		 }
        }
 return NULL;
}


int BACAPI_CopyBACnetOctetString(BACNET_OCTET_STRING *to, BACNET_OCTET_STRING *from)
{
 if (to && from)
    {
	 for (uint32_t c=0;c<from->vValue.size();c++)
		 {
		  to->vValue.push_back(from->vValue[c]);
		 }
     return (int)from->vValue.size();
    }
 return -1;
}



BACNET_OCTET_STRING *BACAPI_CreateOctetString(uint8_t *source, uint32_t len)
{
 BACNET_OCTET_STRING *str = new BACNET_OCTET_STRING;
 BACAPI_InitBACnetOctetString(str,source,len);
 return str;
}

void BACAPI_InitBACnetOctetString(BACNET_OCTET_STRING *ostr, uint8_t *source, uint32_t len)
{
 for (uint32_t c=0;c<len;c++)
     {
      ostr->vValue.push_back(source[c]);
     }
}


// korisnik je duzan izbrisati return value
uint8_t *BACAPI_GetOctetStringValue(BACNET_OCTET_STRING *ostr)
{
 if (!ostr) return NULL;

 uint8_t *tmp = new uint8_t[ostr->vValue.size()];
 for (uint32_t c=0;c<ostr->vValue.size();c++)
     {
      tmp[c] = ostr->vValue[c];
     }
 return tmp;
}




BACAPI_ENUM_STRUCT *BACAPI_GetEnumerationList(BACNET_OBJECT_TYPE type, BACNET_PROPERTY_ID propid)
{
 int cnt = vEnumList.size();

 for (int c=0;c<cnt;c++)
     {
      BACAPI_ENUM_STRUCT *st = &vEnumList[c];
      if (propid == st->propid && type == st->obj_type)
         {
          return st;
         }
     }

 return NULL;
}





uint8_t BACAPI_CRC_Header(uint8_t dataValue,uint8_t crcValue)
{
    uint16_t crc;

    crc = crcValue ^ dataValue; /* XOR C7..C0 with D7..D0 */

    /* Exclusive OR the terms in the table (top down) */
    crc = crc ^ (crc << 1) ^ (crc << 2) ^ (crc << 3)
        ^ (crc << 4) ^ (crc << 5) ^ (crc << 6)
        ^ (crc << 7);

    /* Combine bits shifted out left hand end */
    return (crc & 0xfe) ^ ((crc >> 8) & 1);
}


uint16_t BACAPI_CRC_Data(uint8_t dataValue,uint16_t crcValue)
{
    uint16_t crcLow;

    crcLow = (crcValue & 0xff) ^ dataValue;     /* XOR C7..C0 with D7..D0 */

    /* Exclusive OR the terms in the table (top down) */
    return (crcValue >> 8) ^ (crcLow << 8) ^ (crcLow << 3)
        ^ (crcLow << 12) ^ (crcLow >> 4)
        ^ (crcLow & 0x0f) ^ ((crcLow & 0x0f) << 7);
}

void BACAPI_print_buffer(uint8_t *buffer, int len)
{
 for (int c=0;c<len;c++)
     {
      printf("%2X ",buffer[c]);
     }
 printf("\n");
}



int BACAPI_GetPropertyId(string prop_name)
{
 for (int c=0;c<BACAPI_GetBACnetPropertyCount();c++)
     {
      string s = BACAPI_BACnetPropertyName((BACNET_PROPERTY_ID)c);
      if (s == prop_name) return c;
     }
 return -1;
}




BACNET_APPLICATION_DATA_VALUE *BACAPI_GetPropertyValue(BACNET_OBJECT_PROPERTY *prop)
{
 if (prop->vValueList.size() > 0)
    {
     return prop->vValueList[0];
    }
 return NULL;
}


string BACAPI_GetObjectString(BACNET_OBJECT_ID oid)
{
 char tmp[128];
 sprintf(tmp,"%s, %d",BACAPI_BACnetObjectName(oid.type),oid.instance);
 string s = tmp;
 return s;
}



int BACAPI_CopyBACnetFile(BACNET_FILE *to, BACNET_FILE *from)
{
 int ret = 0;
 if (to && from)
    {
     to->access_type = from->access_type;
     to->eof = from->eof;
     to->octet_count = from->octet_count;
     to->oid = from->oid;
     to->start_pos = from->start_pos;
     ret = BACAPI_CopyBACnetOctetString(&to->data, &from->data);
    }else return -1;
 return ret;
}



int BACAPI_AppendBACnetFile(BACNET_FILE *to, BACNET_FILE *from)
{
 int c = 0;
 uint8_t *octet_data = BACAPI_GetOctetStringValue(&from->data);
 for (c=0; c<from->data.vValue.size(); c++)
     {
      to->data.vValue.push_back(octet_data[c]);
     }

 delete []octet_data;
 return c;
}




int BACAPI_LoadBACnetFile(BACNET_FILE *bac_file, string filename)
{
 FILE *f = fopen(filename.c_str(),"rb");

 if (f)
    {
	 fseek (f , 0 , SEEK_END);
	 long lSize = ftell (f);
	 rewind (f);

	 // allocate memory to contain the whole file:
	 uint8_t *buffer = new uint8_t[lSize];

	 // copy the file into the buffer:
	 size_t res = fread (buffer,1,lSize,f);
	 if (res != lSize)
	    {
	     fclose(f);
	     delete []buffer;
	     return -1;
	    }

     BACAPI_InitBACnetOctetString(&bac_file->data,buffer,lSize);
     bac_file->access_type = 0;
     bac_file->eof = true;
     bac_file->octet_count = lSize;
     bac_file->start_pos = 0;

     delete []buffer;
	 fclose(f);
	 return lSize;
	}

 return -1;
}

void BACAPI_ClearBACnetFile(BACNET_FILE *bac_file)
{
 if (bac_file)
    {
     bac_file->data.vValue.clear();
     bac_file->access_type = 0;
     bac_file->eof = true;
     bac_file->octet_count = 0;
     bac_file->start_pos = 0;
    }
}





BACAPI_PACKET *BACAPI_CreatePacket(uint8_t *apdu, BACNET_NPDU_DATA *npdu,  int apdulen, uint8_t bvlc_len)
{
 BACAPI_PACKET *packet = new BACAPI_PACKET;
 memset(packet,0,sizeof(BACAPI_PACKET));

 packet->npdu = BACAPI_CreateNPDUData(npdu);
 packet->apdu_len = apdulen;
 packet->bvlc_len = bvlc_len;
 memcpy(packet->apdu,apdu,apdulen);
 return packet;
}

void BACAPI_ReleasePacket(BACAPI_PACKET *packet)
{
 if (!packet) return;

 if (packet->npdu)
     BACAPI_DeleteNPDUData(packet->npdu);


 delete packet;
 packet = NULL;
}


#ifdef BACAPI_BUILD_LINUX
void BACAPI_GetAbsoluteTime(struct timespec *abstime, unsigned long milliseconds)
{
    struct timeval now, offset, result;

    gettimeofday(&now, NULL);
    offset.tv_sec = 0;
    offset.tv_usec = milliseconds * 1000;
    timeradd(&now, &offset, &result);
    abstime->tv_sec = result.tv_sec;
    abstime->tv_nsec = result.tv_usec * 1000;
}
#endif



bool BACAPI_CompareAddressData(BACNET_ADDRESS *adr1, BACNET_ADDRESS *adr2)
{
 if (!adr1 || !adr2) return false;


 uint8_t *ptr_adr1 = (uint8_t*)adr1;
 uint8_t *ptr_adr2 = (uint8_t*)adr2;

 for (int c=0;c<sizeof(BACNET_ADDRESS);c++)
     {
      if ((*ptr_adr1++) != (*ptr_adr2++))
         {
          return false;
         }
     }

 return true;
}



BACNET_APPLICATION_DATA_VALUE *BACAPI_GetApplicationDataValue(BACNET_OBJECT_PROPERTY *prop, uint8_t val_num)
{
 if (prop->vValueList.size() == 0)
    return NULL;

 if (val_num > prop->vValueList.size()-1)
     return NULL;

 return prop->vValueList[val_num];
}






uint8_t BACAPI_ascii2hex( char inASCII )
{
    uint8_t retHex=0xff;

    if( (inASCII>=48) && (inASCII<=57) )
        retHex = inASCII-48;
    else if( (inASCII>=65) && (inASCII<=70) )
        retHex = inASCII-55;
    else if( (inASCII>=97) && (inASCII<=102) )
        retHex = inASCII-87;

    return retHex;
}


uint32_t BACAPI_ascii2hex_block( uint8_t * outMessage, int32_t out_len, const char * inMessage )
{
    #define SET_BIT(U,N)   ((U) |=   (0x1 << (N)))

    int32_t   i = 0;
    int32_t   k = 0;
    int32_t   blockLen = 0;
    char    inChar;
    uint8_t   hexVal;
    uint32_t  retVal = 0;

    while ( inMessage[blockLen]!='\0' )     blockLen++;
    blockLen = blockLen >> 1;

    if (blockLen <= out_len)                        // not enough space in output
    {
        retVal = blockLen;
        for (i = 0; i < blockLen; i++)
        {
            outMessage[i] = 0;
            inChar = inMessage[k];
            hexVal = BACAPI_ascii2hex( inChar );
            if (hexVal == 0xff) retVal = 0;     // found an invalid character
            if ( (hexVal & (0x1 << 0) ) != 0 )  SET_BIT( outMessage[i], 4 );
            if ( (hexVal & (0x1 << 1) ) != 0 )  SET_BIT( outMessage[i], 5 );
            if ( (hexVal & (0x1 << 2) ) != 0 )  SET_BIT( outMessage[i], 6 );
            if ( (hexVal & (0x1 << 3) ) != 0 )  SET_BIT( outMessage[i], 7 );
            k++;
            inChar = inMessage[k];
            hexVal = BACAPI_ascii2hex( inChar );
            if ( (hexVal & (0x1 << 0) ) != 0 )  SET_BIT( outMessage[i], 0 );
            if ( (hexVal & (0x1 << 1) ) != 0 )  SET_BIT( outMessage[i], 1 );
            if ( (hexVal & (0x1 << 2) ) != 0 )  SET_BIT( outMessage[i], 2 );
            if ( (hexVal & (0x1 << 3) ) != 0 )  SET_BIT( outMessage[i], 3 );
            k++;
        }
    }   

    return retVal;
}
