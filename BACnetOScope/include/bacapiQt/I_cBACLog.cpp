#include "I_cBACLog.h"


#ifdef BACAPI_BUILD_WIN32
//#include "dtz.h"
#else
#include <time.h>
#endif

#ifdef BACAPI_LOG_ENABLE_MFC_LIST_LOG
#include <stdafx.h>
#endif

#include "bacapi.h"


I_cBACLog *gLog = NULL;


I_cBACLog::I_cBACLog()
{
 path_ = "debug.txt";
}


I_cBACLog::~I_cBACLog()
{
}


bool I_cBACLog::FileExists(string path)
{
 FILE *f = NULL;
 f = fopen(path.c_str(),"r");
 if (f == NULL)
    {
     return false;
    }else {
           fclose(f);
           return true;
          }
}




const std::string currentDateTime() {
    /*
		time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
		*/
		return "deprecated";
}


 string I_cBACLog::AppendLog(char *format, ...)
{

 if (debug_mode != SILENT_MODE)
	{
	 char out[256];
	 va_list arglist;

	 va_start( arglist, format );
	 vsprintf(out, format, arglist );
	 va_end( arglist );

     LogCounter++;

     string s = out;
#ifdef BACAPI_BUILD_WIN32
//     _datetime_t tim = _datetime_t::now();
//     string b = string(tim.fmt("%d/%m - %H:%M:%S"));
#else
     time_t  timev;
      time(&timev);
     string b = currentDateTime();
#endif

  switch (debug_mode)
     {
	  case  FILE_MODE :
			{
             char out_text[512];

			 logfile = fopen(path_.c_str(),"a");
			 if (!logfile)
			     return "No Log File";



			 sprintf(out_text,"#%d [] %s\n",LogCounter,/*b.c_str(),*/s.c_str());
			 string str_out_text = out_text;
			 for (int c=0;c<this->indent_;c++)
			      str_out_text.insert(str_out_text.begin(),' ');

			 fprintf(logfile,"%s",str_out_text.c_str());
			 fclose(logfile);
#ifdef BACAPI_LOG_ENABLE_MFC_LIST_LOG
                   BACAPI_Event *ev = new BACAPI_Event;  // brise se iz PollEvent
                   ev->type = BACAPI_EVENT_LOG_MESSAGE;
                   ev->logmsg = str_out_text;
                   int s = ev->logmsg.size();
                   BACAPI_PushEvent(ev);
#endif
			 break;
			}

		 case CONSOLE_MODE :
			{
			 char out_text[512];
			 sprintf(out_text,"#%d [] %s\n",LogCounter,/*b.c_str(),*/s.c_str());
			 string str_out_text = out_text;
			 for (int c=0;c<this->indent_;c++)
			      str_out_text.insert(str_out_text.begin(),' ');

			 //printf("#%d [%s] %s\n",LogCounter,b.c_str(),s.c_str());
			 printf("%s",out_text);

#ifdef BACAPI_LOG_ENABLE_MFC_LIST_LOG
                   BACAPI_Event *ev = new BACAPI_Event;  // brise se iz PollEvent
                   ev->type = BACAPI_EVENT_LOG_MESSAGE;
                   ev->logmsg = str_out_text;
                   int s = ev->logmsg.size();
                   BACAPI_PushEvent(ev);
#endif
			 break;
			}
     }
    return s;
   }

return "";
}



void I_cBACLog::AppendBuffer(uint8_t *buffer, int len)
{
 logfile = fopen(path_.c_str(),"a");
 for (int c=0;c<len;c++)
     {
      fprintf(logfile,"%2X ",buffer[c]);
     }
 fprintf(logfile,"\n");
 fclose(logfile);
}



void I_cBACLog::ResetLog(void)
{


	logfile = fopen(path_.c_str(), "w");
	if (logfile == NULL)
		return;


    fclose(logfile);

	LogCounter = 0;
    this->indent_ = 0;
}



void I_cBACLog::SetDebugFile(string path)
{
	path_ = path;

}

void I_cBACLog::IncInd()
{
    this->indent_+=3;
}

void I_cBACLog::IncInd(int i)
{
    this->indent_+=i;
}


void I_cBACLog::DecInd(int i)
{
    this->indent_-= i;
    if (this->indent_ <0)
    indent_ = 0;
}

void I_cBACLog::DecInd()
{
    this->indent_-=3;
    if (this->indent_ <0)
    indent_ = 0;
}

void I_cBACLog::SetDebugMode(unsigned char mode)
{
    this->debug_mode = mode;
}










