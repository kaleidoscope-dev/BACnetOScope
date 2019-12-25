#pragma once
#include "bacapi.h"
#include <string>




using namespace std;

typedef struct PIDCOLOR
{
 uint8_t r,g,b;
}PIDCOLOR;

class I_cSettings
{
 public:
        I_cSettings();
       ~I_cSettings();

 void SetFilename(string path);

 void SetServiceDelay(int delay);
 int  GetServiceDelay(void);

 void SetPollingInterval(int inter);
 int GetPollingInterval();

 void SetEnablePropMonitor(bool enable);
 bool GetEnablePropMonitor();


 void SetSortDevice(bool sort);
 bool GetSortDevice();


 void SetWhoIsOnStart(bool start);
 bool GetWhoIsOnStart();

 void SetRPMDefault(bool rpm);
 bool GetRPMDefault();

 void SetBACPort(int port);
 uint16_t GetBACPort();


 int LoadSettings(void);
 void SaveSettings(void);
 
 int LoadSettings279();

 void SetBACIP(bool ip);
 bool GetBACIP();
 void SetBACETH(bool eth);
 bool GetBACETH();
 void SetBACMSTP(bool mstp);
 bool GetBACMSTP();

 bool GetHideErrors();
 void SetHideErrors(bool err);

 void SetWiresharkOutput(bool out);
 bool GetWiresharkOutput();


 uint8_t GetMasterAddress(void);
 void SetMasterAddress(uint8_t adr);

 void SetEthAdapter(string adpName);
 string GetEthAdapter(void);

 void SetShowDesc(bool desc);
 bool GetShowDesc();

 void SetNotificationListener(bool listen);
 bool GetNotificationListener();

 bool GetArrayIndex(void);
 void SetArrayIndex(bool ind);

 bool GetCheckProtocolRev(void);
 void SetCheckProtocolRev(bool rev);

 bool GetCheckPropertyList(void);
 void SetCheckPropertyList(bool rev);





 uint8_t GetIPRetryCount(void);
 void SetIPRetryCount(uint8_t cnt);

 uint16_t GetIPReplyTimeout(void);
 void SetIPReplyTimeout(uint16_t tm);

 uint32_t GetRPDelay(void);
 void SetRPDelay(int dly);

 // mstp
 string GetCOMPort(void);
 void SetCOMPort(string com);
 int GetBaudRate(void);
 void SetBaudRate(int rate);


 // packet
 string GetPipeName(void);
 void SetPipeName(string pipe);

 bool GetIncFrameTraffic(void);
 void SetIncFrameTraffic(bool traffic);
 bool GetOutFrameTraffic(void);
 void SetOutFrameTraffic(bool traffic);

 bool GetIncTokenTraffic(void);
 void SetIncTokenTraffic(bool traffic);
 bool GetOutTokenTraffic(void);
 void SetOutTokenTraffic(bool traffic);

 int GetMSTPReplyDelay(void);
 void SetMSTPReplyDelay(int delay);

 int GetMSTPReplyTimeout(void);
 void SetMSTPReplyTimeout(int timout);

 void SetUsageTimeout(int timout);
 uint16_t GetUsageTimeout(void);

 void SetTokenTimeout(int timout);
 uint16_t GetTokenTimeout(void);

 void SetMasterPoll(int mp);
 uint16_t GetMasterPoll(void);

 void SetIncAPDU(bool apdu);
 bool GetIncAPDU(void);

 void SetOutAPDU(bool apdu);
 bool GetOutAPDU(void);

 uint8_t GetSourceAddress(void);
 void SetSourceAddress(uint8_t adr);

 uint8_t GetMaxMasters(void);
 void SetMaxMasters(uint8_t mm);

 uint8_t GetMaxFrames(void);
 void SetMaxFrames(uint8_t mf);


 PIDCOLOR GetSPColor(void);
 void SetSPColor(uint8_t r, uint8_t g, uint8_t b);

 PIDCOLOR GetPVColor(void);
 void SetPVColor(uint8_t r, uint8_t g, uint8_t b);

 PIDCOLOR GetOVColor(void);
 void SetOVColor(uint8_t r, uint8_t g, uint8_t b);

 bool GetBBMDEnable(void);
 void SetBBMDEnable(bool b);

 int SaveBBMDTable(FILE *f);

 private:
         int serviceDelay_;
         int pollingInt_;
         bool enableMonitor_;
         uint16_t bacPort_;
         bool sortDeviceTree_;
         bool whoisOnStart_;
         bool rpmDefault_;
         bool checkProtocolRev_;
         bool checkPropList_;
         bool bacip_;
         bool baceth_;
         bool mstp_;
         bool wire_;
         bool hideErrors_;
         string ethAdapter_;
         string filename_;
         bool showDesc_;
         bool notificationListener_;
         string comPort_;
         bool arrayIndex_;
         uint16_t ip_replyTimeout_;  // kod citanja liste objekata
         uint8_t ip_retryCount_;
         uint32_t rp_delay_;
         // mstp
         uint8_t masterAdr_;
         uint32_t baudRate_;
         uint8_t max_masters_;
         uint8_t max_frames_;
         uint8_t source_address_;
         //pID
         PIDCOLOR pv_color_;
         PIDCOLOR sp_color_;
         PIDCOLOR ov_color_;

         float pid_tick_;
         uint8_t pid_pmin_,pid_pmax_;
         uint8_t pid_imin_,pid_imax_;
         uint8_t pid_dmin_,pid_dmax_;

         // bbmd
         bool bbmd_enable_;

         // wireshark
         string pipeName_;
         uint16_t mstpReplyDelay_;
         uint16_t mstpReplyTimeout_;
         uint16_t usage_timeout_;
         uint16_t token_timeout_;
         uint16_t master_poll_;



         bool inc_token_traffic;
         bool out_token_traffic;
         bool inc_frame_traffic;
         bool out_frame_traffic;
         bool out_apdu_traffic;
         bool inc_apdu_traffic;

};


extern I_cSettings *gSettings;
