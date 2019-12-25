#include "bacevent.h"

//#include <mutex>


queue<BACAPI_Event*>qEventList;

#ifdef BACAPI_BUILD_LINUX
ACE_Thread_Mutex critical_section;
#endif
//std::mutex critical_section;
//CRITICAL_SECTION CriticalSection;
