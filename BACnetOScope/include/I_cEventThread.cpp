#include "I_cEventThread.h"
#include <include/bacapiQt/bacapi.h>
#include <include/bacapiQt/bacevent.h>

I_cEventThread::I_cEventThread(QObject *parent)
{
}




void I_cEventThread::run() 
{
	BACAPI_Event bac_event;
	while (1)
	{
#ifdef __ANDROID__
        usleep(1000);
#else
		Sleep(1);
#endif
		while (BACAPI_PollEvent(&bac_event))
		{
			switch (bac_event.type)
			{
				case BACAPI_EVENT_I_AM:
			  {
				 BACNET_DEVICE *dev = bac_event.device;
				 if (dev)
 				     emit handleIAM(dev);
				 break;
			  }

				case BACAPI_EVENT_RP_ACK:
				{
				 BACNET_OBJECT_PROPERTY *prop = bac_event.property;
				 if (prop)
				     emit handleRP_ACK(prop, bac_event.invokeId);
				 break;
				}


				case BACAPI_EVENT_RP_ACK_ARRAY_ELEMENT:
				{
				 //printf("BACAPI_EVENT_RP_ACK_ARRAY_ELEMENT\n");
					BACNET_OBJECT_PROPERTY *prop = bac_event.property;
					if (prop)
						emit handleRP_ACK_ArrayElement(prop, bac_event.invokeId);

				 break;
				}
				

				case BACAPI_EVENT_ERROR:
				{
					QString str = QString::asprintf("%s\n%s", BACAPI_BACnetErrorClassName(bac_event.error.errClass), BACAPI_BACnetErrorCodeName(bac_event.error.errCode));
					emit handle_Error(str);
					break;
				}


				case BACAPI_EVENT_SIMPLE_ACK:
				{
				 emit handle_Success();
				 break;
				}


				case BACAPI_EVENT_BVLC_RESULT_SUCCESS:
				{
					emit handle_RFD_Success();
					break;
				}






			}


			}

			}


}

