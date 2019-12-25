#include "I_cSettings.h"
#include "I_cBACdec.h"
#include "I_cBACLog.h"
//#include "I_cBBMD.h"
#include <stdio.h>



I_cSettings::I_cSettings()
{
 serviceDelay_ = 100;  // ms
 pollingInt_ = 10000;  // 10 sec  - najmanja vrijednost
 enableMonitor_ = false;
 bacPort_ = 47808;
 sortDeviceTree_ = true;
 whoisOnStart_ = true;
 rpmDefault_ = true;
 ethAdapter_ = "";
 notificationListener_ = true;
 whoisOnStart_ = true;
 bacip_ = true;
 baceth_ = false;
 mstp_ = false;
 wire_ = false;
 showDesc_ = true;
 comPort_ = "COM1";
 pipeName_ = "";
 arrayIndex_ = false;
 ip_retryCount_ = 3;
 ip_replyTimeout_ = 3000;
 rp_delay_ = 10;
 masterAdr_ = 1;
 max_masters_ = 127;
 checkProtocolRev_ = true;
 checkPropList_ = false;
 hideErrors_ = false;

 pipeName_ = "pipename";
 mstpReplyTimeout_ = 300;
 mstpReplyDelay_ = 250;
 usage_timeout_ = 95;
 token_timeout_ = 500;
 master_poll_ = 50;


 inc_token_traffic = false;
 out_token_traffic = false;
 inc_frame_traffic = false;
 out_frame_traffic = false;

 source_address_ = 1;
 max_frames_ = 25;

 baudRate_ = 38400;

 pv_color_.r = 0;
 pv_color_.g = 255;
 pv_color_.b = 0;

 sp_color_.r = 255;
 sp_color_.g = 0;
 sp_color_.b = 0;

 ov_color_.r = 0;
 ov_color_.g = 0;
 ov_color_.b = 255;


 pid_tick_ = 0.1;
 pid_pmin_ = 0, pid_pmax_ = 5;
 pid_imin_ = 0, pid_imax_ = 5;
 pid_dmin_ = 0, pid_dmax_ = 5;

 bbmd_enable_ = false;
}


void I_cSettings::SetFilename(string path)
{
 filename_ = path;
}


int I_cSettings::LoadSettings(void)
{
 int len = 0;
 uint32_t len_val = 0;
 uint32_t value = 0;
 uint8_t tag = 0;



 FILE *f = fopen(filename_.c_str(),"rb");
 string s;

 if (f == NULL)
    {
     gLog->AppendLog("I_cSettings::LoadSettings: ERROR: No setting file found\n");
     return -1;
    }


 fseek (f, 0, SEEK_END);   // non-portable
 size_t size = ftell (f);
 uint8_t *buffer = new uint8_t[size];
 rewind(f);
 // copy;
 try
 {

 fread(buffer,1,size,f);
 fclose(f);
 uint32_t tmp32 = 0;


 len += gDec->decode_unsigned32(&buffer[len],&value);
 uint32_t magicNum = value;

 // imamo razliciti settings file. Ucitaj file u starom formatu, i snimi u novom.
 // nakon toga ucitaj u novom.
 if (magicNum != MAGIC_NUMBER_HEX)
    {
     delete []buffer;
  
     //LoadSettings279();
     //SaveSettings();
     //LoadSettings();
     return -1;
    }


 len += gDec->decode_unsigned16(&buffer[len],&bacPort_);
 len += gDec->decode_unsigned32(&buffer[len],&tmp32);// instance
 serviceDelay_ = tmp32;
 enableMonitor_ = buffer[len++];
 sortDeviceTree_ = buffer[len++];
 whoisOnStart_ = buffer[len++];
 rpmDefault_ = buffer[len++];
 //checkProtocolRev_ = buffer[len++];
 checkPropList_ = buffer[len++];
 hideErrors_ = buffer[len++];
 bacip_ = buffer[len++];
 baceth_ = buffer[len++];
 mstp_ = buffer[len++];
 wire_ = buffer[len++];
 showDesc_ = buffer[len++];
 notificationListener_= buffer[len++];
 arrayIndex_ = buffer[len++];

 ip_retryCount_ = buffer[len++];
 inc_token_traffic = buffer[len++];
 out_token_traffic = buffer[len++];
 inc_frame_traffic = buffer[len++];
 out_frame_traffic = buffer[len++];
 inc_apdu_traffic = buffer[len++];
 out_apdu_traffic = buffer[len++];

 source_address_ = buffer[len++];
 max_frames_ = buffer[len++];
 max_masters_ = buffer[len++];

 pv_color_.r = buffer[len++];
 pv_color_.g = buffer[len++];
 pv_color_.b = buffer[len++];

 sp_color_.r = buffer[len++];
 sp_color_.g = buffer[len++];
 sp_color_.b = buffer[len++];

 ov_color_.r = buffer[len++];
 ov_color_.g = buffer[len++];
 ov_color_.b = buffer[len++];

 bbmd_enable_ = buffer[len++];

 len += gDec->decode_unsigned32(&buffer[len],&baudRate_);

 len += gDec->decode_unsigned16(&buffer[len],&ip_replyTimeout_);


 len += gDec->decode_unsigned32(&buffer[len],&rp_delay_);
 len += gDec->decode_unsigned16(&buffer[len],&mstpReplyDelay_);
 len += gDec->decode_unsigned16(&buffer[len],&mstpReplyTimeout_);
 len += gDec->decode_unsigned16(&buffer[len],&usage_timeout_);
 len += gDec->decode_unsigned16(&buffer[len],&token_timeout_);
 len += gDec->decode_unsigned16(&buffer[len],&master_poll_);




 len += gDec->decode_tag_number_and_value(&buffer[len], &tag, &len_val);
 if (len_val > 0)
    {
     len += gDec->decode_character_string(&buffer[len],len_val,&ethAdapter_);
    }

 len += gDec->decode_tag_number_and_value(&buffer[len], &tag, &len_val);
 if (len_val > 0)
    {
     comPort_ = "";
     len += gDec->decode_character_string(&buffer[len],len_val,&comPort_);
    }

 len += gDec->decode_tag_number_and_value(&buffer[len], &tag, &len_val);
 if (len_val > 0)
    {
     pipeName_ = "";
     len += gDec->decode_character_string(&buffer[len],len_val,&pipeName_);
    }

 // load bbmd table


  uint8_t *ptr = &buffer[len];
#ifdef BACAPI_USES_BBMD
  if (gBBMD)
	 {
	  uint16_t count = 0;
	  len += gDec->decode_unsigned16(&buffer[len],&count);
	  for (int c=0;c<count;c++)
		 {
		  BACNET_BVLC_BDT *bdt = gBBMD->GetBBMDTable();
		  BACNET_BVLC_BDT_ENTRY entry;
		  memcpy(&entry,&buffer[len],sizeof(BACNET_BVLC_BDT_ENTRY));
		  bdt->vTableEntryList.push_back(entry);
		  len+= sizeof(BACNET_BVLC_BDT_ENTRY);
		 }
	  }
#endif

    } catch (...)
           {
            enableMonitor_ = false;
            return -1;
           }


 // enable monitor je uvijek false kod prvog pokretanja
 enableMonitor_ = false;
 delete []buffer;
 
 return 0;
}



void I_cSettings::SaveSettings(void)
{
 int len = 0;
 //FILE *fnt = fopen(filename_.c_str(),"w");
 //fprintf(fnt,"%d,%d,%d,%d,%d,%d,%d,%d,%d\n%s",bacPort_,serviceDelay_,pollingInt_,enableMonitor_,sortDeviceTree_,whoisOnStart_,rpmDefault_,bacip_,baceth_,ethAdapter_.c_str());
 //fclose(fnt);

 uint8_t buffer[1024];
 FILE *f = fopen(filename_.c_str(),"wb");

 if (f == NULL)
    {
     //MessageBox(NULL,"Unable to open settings file.","BACnet Lookout",MB_OK);
     return;
    }

 len = gDec->encode_unsigned32(buffer,MAGIC_NUMBER_DEC);

 len += gDec->encode_unsigned16(&buffer[len],bacPort_);
 len += gDec->encode_unsigned32(&buffer[len],serviceDelay_);// instance
 buffer[len++] = (uint8_t)enableMonitor_;
 buffer[len++] = (uint8_t)sortDeviceTree_;
 buffer[len++] = (uint8_t)whoisOnStart_;
 buffer[len++] = (uint8_t)rpmDefault_;
 //buffer[len++] = (uint8_t)checkProtocolRev_;
 buffer[len++] = (uint8_t)checkPropList_;
 buffer[len++] = (uint8_t)hideErrors_;
 
 buffer[len++] = (uint8_t)bacip_;
 buffer[len++] = (uint8_t)baceth_;
 buffer[len++] = (uint8_t)mstp_;
 buffer[len++] = (uint8_t)wire_;
 buffer[len++] = (uint8_t)showDesc_;
 buffer[len++] = (uint8_t)notificationListener_;
 buffer[len++] = (uint8_t)arrayIndex_;

 buffer[len++] = (uint8_t)ip_retryCount_;
 buffer[len++] = (uint8_t)inc_token_traffic;
 buffer[len++] = (uint8_t)out_token_traffic;
 buffer[len++] = (uint8_t)inc_frame_traffic;
 buffer[len++] = (uint8_t)out_frame_traffic;
 buffer[len++] = (uint8_t)inc_apdu_traffic;
 buffer[len++] = (uint8_t)out_apdu_traffic;

 buffer[len++] = (uint8_t)source_address_;
 buffer[len++] = (uint8_t)max_frames_;
 buffer[len++] = (uint8_t)max_masters_;

 buffer[len++] = pv_color_.r ;
 buffer[len++] = pv_color_.g ;
 buffer[len++] = pv_color_.b ;

 buffer[len++] = sp_color_.r ;
 buffer[len++] = sp_color_.g ;
 buffer[len++] = sp_color_.b ;

 buffer[len++] = ov_color_.r ;
 buffer[len++] = ov_color_.g ;
 buffer[len++] = ov_color_.b ;

 buffer[len++] = (uint8_t)bbmd_enable_;


 len += gDec->encode_unsigned32(&buffer[len],baudRate_);
 len += gDec->encode_unsigned16(&buffer[len],ip_replyTimeout_);
 len += gDec->encode_unsigned32(&buffer[len],rp_delay_);

 len += gDec->encode_unsigned16(&buffer[len],mstpReplyDelay_);
 len += gDec->encode_unsigned16(&buffer[len],mstpReplyTimeout_);
 len += gDec->encode_unsigned16(&buffer[len],usage_timeout_);
 len += gDec->encode_unsigned16(&buffer[len],token_timeout_);
 len += gDec->encode_unsigned16(&buffer[len],master_poll_);

 BACNET_CHARACTER_STRING str;
 str.encoding = CHAR_SET_ANSI_X34;
 str.value8 = ethAdapter_;

 len += gDec->encode_application_character_string(&buffer[len],&str);


 BACNET_CHARACTER_STRING com;
 com.encoding = CHAR_SET_ANSI_X34;
 com.value8 = comPort_;

 len += gDec->encode_application_character_string(&buffer[len],&com);


 BACNET_CHARACTER_STRING pipe;
 pipe.encoding = CHAR_SET_ANSI_X34;
 pipe.value8 = pipeName_;

 len += gDec->encode_application_character_string(&buffer[len],&pipe);
 fwrite(buffer,1,len,f);


 // save bbmd table
#ifdef BACAPI_USES_BBMD
 if (gBBMD)
    {
     BACNET_BVLC_BDT *bdt = gBBMD->GetBBMDTable();
     size_t count = bdt->vTableEntryList.size();

     gDec->encode_unsigned16(buffer,(uint16_t)count);
     fwrite(buffer,1,2,f);

     for (size_t c=0;c<count;c++)
         {
          BACNET_BVLC_BDT_ENTRY *ptr_entry = &bdt->vTableEntryList[c];
          memcpy(buffer,ptr_entry,sizeof(BACNET_BVLC_BDT_ENTRY));
          fwrite(buffer,1,sizeof(BACNET_BVLC_BDT_ENTRY),f);
         }
    }
#endif


 //SaveBBMDTable(f);

 fclose(f);
}





I_cSettings::~I_cSettings()
{
}



void I_cSettings::SetServiceDelay(int delay)
{
 gLog->AppendLog("I_cSettings::SetServiceDelay: %d\n",delay);
 serviceDelay_ = delay;
}


int I_cSettings::GetServiceDelay()
{
 return serviceDelay_;
}




void I_cSettings::SetPollingInterval(int inter)
{
 gLog->AppendLog("I_cSettings::SetPollingInterval: %d\n",inter);

 pollingInt_ = inter;
}


int I_cSettings::GetPollingInterval()
{
 return pollingInt_;
}




void I_cSettings::SetEnablePropMonitor(bool enable)
{
 gLog->AppendLog("I_cSettings::SetEnablePropMonitor: %d\n",enable);
 enableMonitor_ = enable;
}


bool I_cSettings::GetEnablePropMonitor()
{
 return enableMonitor_;
}


void I_cSettings::SetBACPort(int port)
{
 gLog->AppendLog("I_cSettings::SetBACPort: %d\n",port);

 bacPort_ = port;
}


uint16_t I_cSettings::GetBACPort()
{


 return bacPort_;
}



void I_cSettings::SetSortDevice(bool sort)
{
 sortDeviceTree_ = sort;
}

bool I_cSettings::GetSortDevice()
{
 return sortDeviceTree_;
}



void I_cSettings::SetWhoIsOnStart(bool start)
{
gLog->AppendLog("I_cSettings::SetWhoIsOnStart: %d\n",start);
 whoisOnStart_ = start;
}

bool I_cSettings::GetWhoIsOnStart()
{
 return whoisOnStart_;
}


void I_cSettings::SetRPMDefault(bool rpm)
{
 gLog->AppendLog("I_cSettings::SetRPMDefault: %d\n",rpm);
 rpmDefault_ = rpm;
}

bool I_cSettings::GetRPMDefault()
{
 return rpmDefault_;
}



void I_cSettings::SetBACIP(bool ip)
{
 gLog->AppendLog("I_cSettings::SetBACIP: %d\n",ip);
 bacip_ = ip;
}


bool I_cSettings::GetBACIP()
{
 return bacip_;
}


void I_cSettings::SetBACETH(bool eth)
{
 gLog->AppendLog("I_cSettings::SetBACETH: %d\n",eth);
 baceth_ = eth;
}


bool I_cSettings::GetBACETH()
{
 return baceth_;
}


void I_cSettings::SetBACMSTP(bool mstp)
{
 gLog->AppendLog("I_cSettings::SetBACMSTP: %d\n",mstp);
 mstp_ = mstp;
}

bool I_cSettings::GetBACMSTP()
{

 return mstp_;
}

bool I_cSettings::GetHideErrors()
{
 return hideErrors_;
}

void I_cSettings::SetHideErrors(bool err)
{
 hideErrors_ = err;
}



void I_cSettings::SetWiresharkOutput(bool out)
{
gLog->AppendLog("I_cSettings::SetWiresharkOutput: %d\n",out);
 wire_ = out;
}

bool I_cSettings::GetWiresharkOutput()
{
 return wire_;
}


void I_cSettings::SetEthAdapter(string adpName)
{
 gLog->AppendLog("I_cSettings::SetEthAdapter: %s\n",adpName.c_str());
 ethAdapter_ = adpName;
}

string I_cSettings::GetEthAdapter(void)
{
 return ethAdapter_;
}


void I_cSettings::SetShowDesc(bool desc)
{
gLog->AppendLog("I_cSettings::SetShowDesc: %d\n",desc);
 showDesc_ = desc;
}

bool I_cSettings::GetShowDesc()
{
 return showDesc_;
}

void I_cSettings::SetNotificationListener(bool SetNotificationListener)
{
gLog->AppendLog("I_cSettings::SetNotificationListener: %d\n",SetNotificationListener);
 notificationListener_ = SetNotificationListener;
}


bool I_cSettings::GetNotificationListener()
{
 return notificationListener_;
}

string I_cSettings::GetPipeName(void)
{
 return pipeName_;
}

void I_cSettings::SetPipeName(string pipe)
{
 //pipeName_ = "";
 pipeName_ = pipe;
 //gLog->AppendLog("I_cSettings::SetPipeName: %s",pipe.c_str());
 printf("I_cSettings::SetPipeName: %s",pipe.c_str());
}

string I_cSettings::GetCOMPort(void)
{
 return comPort_;
}

void I_cSettings::SetCOMPort(string com)
{
 gLog->AppendLog("I_cSettings::SetCOMPort: %s\n",com.c_str());
 comPort_ = com;
}

bool I_cSettings::GetArrayIndex(void)
{
 return arrayIndex_;
}

void I_cSettings::SetArrayIndex(bool ind)
{
 gLog->AppendLog("I_cSettings::SetArrayIndex: %d\n",ind);
 arrayIndex_ = ind;
}



int I_cSettings::GetBaudRate(void)
{
 return baudRate_;
}

void I_cSettings::SetBaudRate(int rate)
{
 baudRate_ = rate;
}


uint8_t I_cSettings::GetIPRetryCount(void)
{
 return ip_retryCount_;
}

void I_cSettings::SetIPRetryCount(uint8_t cnt)
{
 ip_retryCount_ = cnt;
}


uint16_t I_cSettings::GetIPReplyTimeout(void)
{
 return ip_replyTimeout_;
}


void I_cSettings::SetIPReplyTimeout(uint16_t tm)
{
 ip_replyTimeout_ = tm;
}



uint32_t I_cSettings::GetRPDelay(void)
{
 return rp_delay_;
}


void I_cSettings::SetRPDelay(int dly)
{
 rp_delay_ = dly;
}


void I_cSettings::SetMasterAddress(uint8_t adr)
{
 masterAdr_ = adr;
}


uint8_t I_cSettings::GetMasterAddress(void)
{
 return masterAdr_;
}


bool I_cSettings::GetIncFrameTraffic(void)
{
 return inc_frame_traffic;
}

void I_cSettings::SetIncFrameTraffic(bool traffic)
{
 inc_frame_traffic = traffic;
}

bool I_cSettings::GetOutFrameTraffic(void)
{
 return out_frame_traffic;
}

void I_cSettings::SetOutFrameTraffic(bool traffic)
{
 out_frame_traffic = traffic;
}

bool I_cSettings::GetIncTokenTraffic(void)
{
 return inc_token_traffic;
}

void I_cSettings::SetIncTokenTraffic(bool traffic)
{
 inc_token_traffic = traffic;
}

bool I_cSettings::GetOutTokenTraffic(void)
{
 return out_token_traffic;
}

void I_cSettings::SetOutTokenTraffic(bool traffic)
{
 out_token_traffic = traffic;
}

int I_cSettings::GetMSTPReplyDelay(void)
{
 return mstpReplyDelay_;
}

void I_cSettings::SetMSTPReplyDelay(int delay)
{
 mstpReplyDelay_ = delay;
}

int I_cSettings::GetMSTPReplyTimeout(void)  // za mstp
{
 return mstpReplyTimeout_;
}

void I_cSettings::SetMSTPReplyTimeout(int timout)
{
 mstpReplyTimeout_ = timout;
}

void I_cSettings::SetUsageTimeout(int timout)
{
 usage_timeout_ = timout;
}

uint16_t I_cSettings::GetUsageTimeout(void)
{
 return usage_timeout_;
}

void I_cSettings::SetTokenTimeout(int timout)
{
 token_timeout_ = timout;
}

uint16_t I_cSettings::GetTokenTimeout(void)
{
 return token_timeout_;
}

void I_cSettings::SetMasterPoll(int mp)
{
 master_poll_ = mp;
}

uint16_t I_cSettings::GetMasterPoll(void)
{
 return master_poll_;
}



void I_cSettings::SetIncAPDU(bool apdu)
{
 inc_apdu_traffic = apdu;
}

bool I_cSettings::GetIncAPDU(void)
{
 return inc_apdu_traffic;
}

void I_cSettings::SetOutAPDU(bool apdu)
{
 out_apdu_traffic = apdu;
}

bool I_cSettings::GetOutAPDU(void)
{
 return out_apdu_traffic;
}

uint8_t I_cSettings::GetSourceAddress(void)
{
 return source_address_;
}

void I_cSettings::SetSourceAddress(uint8_t adr)
{
 source_address_ = adr;
}


uint8_t I_cSettings::GetMaxMasters(void)
{
 return max_masters_;
}

void I_cSettings::SetMaxMasters(uint8_t mm)
{
 max_masters_ = mm;
}

uint8_t I_cSettings::GetMaxFrames(void)
{
 return max_frames_;
}

void I_cSettings::SetMaxFrames(uint8_t mf)
{
 max_frames_ = mf;
}




PIDCOLOR I_cSettings::GetSPColor(void)
{
 return sp_color_;
}

void I_cSettings::SetSPColor(uint8_t r, uint8_t g, uint8_t b)
{
 sp_color_.r = r;
 sp_color_.g = g;
 sp_color_.b = b;
}

PIDCOLOR I_cSettings::GetPVColor(void)
{
return pv_color_;
}

void I_cSettings::SetPVColor(uint8_t r, uint8_t g, uint8_t b)
{
 pv_color_.r = r;
 pv_color_.g = g;
 pv_color_.b = b;
}

PIDCOLOR I_cSettings::GetOVColor(void)
{
return ov_color_;
}

void I_cSettings::SetOVColor(uint8_t r, uint8_t g, uint8_t b)
{
 ov_color_.r = r;
 ov_color_.g = g;
 ov_color_.b = b;
}


bool I_cSettings::GetBBMDEnable(void)
{
 return bbmd_enable_;
}

void I_cSettings::SetBBMDEnable(bool b)
{
 bbmd_enable_ = b;
}




 bool I_cSettings::GetCheckProtocolRev(void)
 {
  return checkProtocolRev_;
 }
 
 void I_cSettings::SetCheckProtocolRev(bool rev)
 {
  checkProtocolRev_ = rev;
 }
 
 
bool I_cSettings::GetCheckPropertyList(void)
{
return checkPropList_;
}

void I_cSettings::SetCheckPropertyList(bool pl)
{
 checkPropList_ = pl;
}

 
 
 
// ucitavanje starog settings formata.
// u ovom formatu ne postoji hideErrors_
int I_cSettings::LoadSettings279(void)
{
 int len = 0;
 uint32_t len_val = 0;
 uint32_t value = 0;
 uint8_t tag = 0;



 FILE *f = fopen(filename_.c_str(),"rb");
 string s;

 if (f == NULL)
    {
     gLog->AppendLog("I_cSettings::LoadSettings: ERROR: No setting file found\n");
     return -1;
    }


 fseek (f, 0, SEEK_END);   // non-portable
 size_t size = ftell (f);
 uint8_t *buffer = new uint8_t[size];
 rewind(f);
 // copy;
 try
 {

 fread(buffer,1,size,f);
 fclose(f);
 uint32_t tmp32 = 0;


 len += gDec->decode_unsigned32(&buffer[len],&value);
 uint32_t magicNum = value;

 // nemoramo ovdje provjeravati magicnum jer je ovo stari load.
 // ovdje znamo da je mn 279

 len += gDec->decode_unsigned16(&buffer[len],&bacPort_);
 len += gDec->decode_unsigned32(&buffer[len],&tmp32);// instance
 serviceDelay_ = tmp32;
 enableMonitor_ = buffer[len++];
 sortDeviceTree_ = buffer[len++];
 whoisOnStart_ = buffer[len++];
 rpmDefault_ = buffer[len++];
 //checkProtocolRev_ = buffer[len++];
 checkPropList_ = buffer[len++];
 bacip_ = buffer[len++];
 baceth_ = buffer[len++];
 mstp_ = buffer[len++];
 wire_ = buffer[len++];
 showDesc_ = buffer[len++];
 notificationListener_= buffer[len++];
 arrayIndex_ = buffer[len++];

 ip_retryCount_ = buffer[len++];
 inc_token_traffic = buffer[len++];
 out_token_traffic = buffer[len++];
 inc_frame_traffic = buffer[len++];
 out_frame_traffic = buffer[len++];
 inc_apdu_traffic = buffer[len++];
 out_apdu_traffic = buffer[len++];

 source_address_ = buffer[len++];
 max_frames_ = buffer[len++];
 max_masters_ = buffer[len++];

 pv_color_.r = buffer[len++];
 pv_color_.g = buffer[len++];
 pv_color_.b = buffer[len++];

 sp_color_.r = buffer[len++];
 sp_color_.g = buffer[len++];
 sp_color_.b = buffer[len++];

 ov_color_.r = buffer[len++];
 ov_color_.g = buffer[len++];
 ov_color_.b = buffer[len++];

 bbmd_enable_ = buffer[len++];

 len += gDec->decode_unsigned32(&buffer[len],&baudRate_);

 len += gDec->decode_unsigned16(&buffer[len],&ip_replyTimeout_);


 len += gDec->decode_unsigned32(&buffer[len],&rp_delay_);
 len += gDec->decode_unsigned16(&buffer[len],&mstpReplyDelay_);
 len += gDec->decode_unsigned16(&buffer[len],&mstpReplyTimeout_);
 len += gDec->decode_unsigned16(&buffer[len],&usage_timeout_);
 len += gDec->decode_unsigned16(&buffer[len],&token_timeout_);
 len += gDec->decode_unsigned16(&buffer[len],&master_poll_);




 len += gDec->decode_tag_number_and_value(&buffer[len], &tag, &len_val);
 if (len_val > 0)
    {
     len += gDec->decode_character_string(&buffer[len],len_val,&ethAdapter_);
    }

 len += gDec->decode_tag_number_and_value(&buffer[len], &tag, &len_val);
 if (len_val > 0)
    {
     comPort_ = "";
     len += gDec->decode_character_string(&buffer[len],len_val,&comPort_);
    }

 len += gDec->decode_tag_number_and_value(&buffer[len], &tag, &len_val);
 if (len_val > 0)
    {
     len += gDec->decode_character_string(&buffer[len],len_val,&pipeName_);
    }

 // load bbmd table


  uint8_t *ptr = &buffer[len];
#ifdef BACAPI_USES_BBMD
  if (gBBMD)
	 {
	  uint16_t count = 0;
	  len += gDec->decode_unsigned16(&buffer[len],&count);
	  for (int c=0;c<count;c++)
		 {
		  BACNET_BVLC_BDT *bdt = gBBMD->GetBBMDTable();
		  BACNET_BVLC_BDT_ENTRY entry;
		  memcpy(&entry,&buffer[len],sizeof(BACNET_BVLC_BDT_ENTRY));
		  bdt->vTableEntryList.push_back(entry);
		  len+= sizeof(BACNET_BVLC_BDT_ENTRY);
		 }
	  }
#endif

    } catch (...)
           {
            enableMonitor_ = false;
            return -1;
           }


 // enable monitor je uvijek false kod prvog pokretanja
 enableMonitor_ = false;
 delete []buffer;
 
 return -1;
 return 0;
} 